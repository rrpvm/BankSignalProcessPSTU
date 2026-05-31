#pragma comment(lib, "Ws2_32.lib")
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>
#include <iostream>
#include "ServerHandler.h"
#include "../data/CommandFactory.h"
#include "../shared.h"
#include "../utilities/NetworkUtils.h"
using json = nlohmann::json;

ServerHandler::ServerHandler(std::shared_ptr<CashierRepository> repository)
{
	this->mState = std::make_unique<ServerState>(5u);
	this->mRepository = std::move(repository);

    WSADATA wsaData{};

    const int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0)
    {
        throw std::runtime_error(
            "WSAStartup failed. Error code: " + std::to_string(result)
        );
    }
    else {
        isInitialisedNetwork = true;
    }
}
ServerHandler::~ServerHandler()
{
   
    WSACleanup();
}
void ServerHandler::start()
{
    if (isRunning || !isInitialisedNetwork)return;
    this->isRunning = true;
    std::thread postProcess(&ServerHandler::service, this);
    mClientThreads.push_back(std::move(postProcess));
    incomingConnectionsLoop();
    {
        std::lock_guard<std::mutex> lock(mClientThreadsMutex);

        for (std::thread& clientThread : mClientThreads)
        {
            if (clientThread.joinable())
            {
                clientThread.join();
            }
        }
        mClientThreads.clear();
    }
    
}
void ServerHandler::stop()
{
	this->isRunning = false;
    if (mListenSocket != INVALID_SOCKET)
    {
        shutdown(mListenSocket, SD_BOTH);
        closesocket(mListenSocket);
        mListenSocket = INVALID_SOCKET;
    }
}
void ServerHandler::initSocket()
{
    mListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (mListenSocket == INVALID_SOCKET)
    {
        int error = WSAGetLastError();
        std::cout << "Server socket creation failed. WSA error: " << error << "\n";
        return;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(6000);
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    const int bindResult = bind(
        mListenSocket,
        reinterpret_cast<sockaddr*>(&serverAddress),
        sizeof(serverAddress)
    );

    if (bindResult == SOCKET_ERROR)
    {
        std::cout << "Bind failed. Error: " << WSAGetLastError() << "\n";
        closesocket(mListenSocket);
        mListenSocket = INVALID_SOCKET;
        return;
    }

    const int listenResult = listen(mListenSocket, SOMAXCONN);

    if (listenResult == SOCKET_ERROR)
    {
        std::cout << "Listen failed. Error: " << WSAGetLastError() << "\n";
        closesocket(mListenSocket);
        mListenSocket = INVALID_SOCKET;
        return;
    }

    u_long nonBlocking = false;
    ioctlsocket(mListenSocket, FIONBIO, &nonBlocking);

    std::cout << "Server started on 127.0.0.1:6000\n";
}
void ServerHandler::incomingConnectionsLoop()
{
    initSocket();
    while (isRunning.load())
    {
        
        SOCKET clientSocket = accept(mListenSocket, nullptr, nullptr);

        if (clientSocket == INVALID_SOCKET)
        {
            int error = WSAGetLastError();

            if (error == WSAEWOULDBLOCK)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }

            if (!isRunning.load())
            {
                break;
            }

            std::cout << "Accept error: " << error << "\n";
            continue;
        }

        setsockopt(
            clientSocket,
            SOL_SOCKET,
            SO_RCVTIMEO,
            reinterpret_cast<const char*>(&dwTimeOut),
            sizeof(dwTimeOut)
        );

        {
            std::lock_guard<std::mutex> lock(mClientThreadsMutex);

            mClientThreads.emplace_back(
                &ServerHandler::clientLoop,
                this,
                clientSocket
            );
        }

        std::cout << "New cashier connected\n";
    }

    if (mListenSocket != INVALID_SOCKET)
    {
        closesocket(mListenSocket);
        mListenSocket = INVALID_SOCKET;
    }

    std::cout << "Accept loop stopped\n";
}
void ServerHandler::clientLoop(SOCKET clientSocket)
{
    std::uint64_t loopId = mNextConnectionId.fetch_add(1);
    appLogger() << "client attemp to connect with {socket,loopId}={" << clientSocket << ","<<loopId<<"}";
    char buffer[1024];
    std::string receiveBuffer;
    constexpr size_t maxMessageSize = 1024 * 1024;//1mb
    while (isRunning.load())
    {
        int received = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (received > 0)
        {
            receiveBuffer.append(buffer, received);
            if (receiveBuffer.size() > maxMessageSize)
            {
                std::cout << "receive buffer is too large. client disconnected.\n";
                break;
            }
            size_t newlinePos = std::string::npos;
            while ((newlinePos = receiveBuffer.find('\n')) != std::string::npos)
            {
                std::string message = receiveBuffer.substr(0, newlinePos);

                receiveBuffer.erase(0, newlinePos + 1);

                if (!message.empty() && message.back() == '\r')
                {
                    message.pop_back();
                }

                if (!message.empty())
                {
                    std::cout << "message recv: " << message << std::endl;
                    handleInputMessage(clientSocket,loopId, message);
                }
            }
        }
        else if (received == 0)
        {
            break;
        }
        else
        {
            int error = WSAGetLastError();

            if (error == WSAETIMEDOUT)
            {
                //2000мс не получал ничего - время прозвонить
                doHeartbeat(loopId,clientSocket);
                continue;
            }

            if (!isRunning.load())
            {
                break;
            }

            std::cout << "Recv error: " << error << "\n";
            break;
        }
    }

    shutdown(clientSocket, SD_BOTH);
    closesocket(clientSocket);
    cleanupWorkerSession(loopId);
    appLogger() << "client disconnected with {socket,loopId}={" << clientSocket << "," << loopId << "}";
}
void ServerHandler::handleInputMessage(SOCKET clientSocket, ConnectionId loopId, const std::string& msg)
{
    json inputMessage;
    try
    {
        inputMessage = json::parse(msg);
    }
    catch (const std::exception& e)
    {
        std::cout << "JSON parse error: " << e.what() << std::endl;
        return;
    }
    if (!inputMessage.contains("type") || !inputMessage["type"].is_string())
    {
        std::cout << "missing type, bad msg{" << msg << "}" << std::endl;
        return;
    }
    refreshSessionTimeout(loopId);
    auto command = CommandFactory::fromJson(inputMessage);
    if (!command.get()) {
        std::cout << "from json factory error" << std::endl;
        return;
    }
    switch (command->getType())
    {
    case CommandsType::Register: {
        auto info = handleRegisterCommand(clientSocket, loopId, dynamic_cast<RegisterCommand*>(command.get()));
        if (info.has_value()) {
            CashierInfo value = info.value();
            auto response = RegisterResponseCommand(value.cashierId, value.mName, value.mState);
            NetworkUtils::sendJson(clientSocket,response.toJson());
        }
        break;
    }

    case CommandsType::State: {
        handleGetState(clientSocket, loopId, dynamic_cast<SendStateCommand*>(command.get()));
        sendServerSideState(loopId);
        break;
    }
    default:
        std::cout << "unhandled type " << command->getCommandTypeName() << std::endl;
    }

}
void ServerHandler::killConnection(SOCKET socket)
{
    shutdown(socket, SD_BOTH);
}
std::optional<CashierInfo> ServerHandler::handleRegisterCommand(SOCKET clientSocket, ConnectionId loopId, RegisterCommand* command)
{
    if (command == nullptr) {
        std::cout << "loop id: " << loopId << " kill connection cuz unsuccess registration" << std::endl;
        killConnection(clientSocket);
        return std::nullopt;
    }
    const WorkstationId workerId = command->cashierId();
    const auto alreadyHasSession = [this](WorkstationId workerId) {
        std::lock_guard lock(this->_mutex);
        const auto& conditional = this->mSessions.find(workerId);
        if (conditional == mSessions.end())return false;
        return true;
        };
    if (!alreadyHasSession(workerId)) {
        appLogger() << "new accepted registration request with workerId:{" << workerId << "} clientSocket:{" << clientSocket << "}";
        registerWorkstationConnection(workerId, command->cashierName(), loopId,clientSocket);
        return mState->getWorkstationState(workerId);
    }
    //has a session: do validation
    const auto isNewConnectionPrefer = [&](WorkstationId workerId) {
        WorkerSession firstRecord;
        {
            std::lock_guard lock(this->_mutex);
            //поиск старых соединений по workerId
            const auto& conditional = this->mSessions.find(workerId);
            if (conditional == mSessions.end())return true;//старое пропало
            const auto& workerIdSessionList = conditional->second;
            if (workerIdSessionList.empty())return true;//пусто, нет сессий
            firstRecord = workerIdSessionList.front();
        }
         const auto timeNow = std::chrono::steady_clock::now();
         const auto inactiveFor = std::chrono::duration_cast<std::chrono::milliseconds>(
             timeNow - firstRecord.lastActivity
         );
        
         //timeout bring for connection(афк сессии отдельно от афк кассы): 
         if (inactiveFor.count() > 1e+4) {
             appLogger() << "replace old connection, socket:{" << firstRecord.mConnectedSocket << "} by:{" << clientSocket << "} due to inactive()";
             removeWorkstation(workerId);
             killConnection(firstRecord.mConnectedSocket);
             return true;
         }
         return false;
    };
    if (isNewConnectionPrefer(workerId)) {
        registerWorkstationConnection(workerId, command->cashierName(), loopId,clientSocket);
        return mState->getWorkstationState(workerId);
    }
    //cancel connection
    appLogger() << "denied in registration with socket {" << clientSocket << "} & ConnectionId {" << loopId << "} due to exist connection";
    killConnection(clientSocket);
    return std::nullopt;
}
void ServerHandler::handleGetState(SOCKET clientSocket, uint64_t loopId, SendStateCommand* command)
{
    const auto& workerState = command->takeInfo();
    appLogger() << "client handleGetState() {socket,loopId,workerId}={" << clientSocket << "," << loopId << "," << workerState.cashierId << "}";
    appLogger() << "workerId new state: " << (int) workerState.mState << " worker id = " << workerState.cashierId;

    mState->updateCashierState(workerState.cashierId, workerState.mState);
    publishStateSnapshotLocked();
}
void ServerHandler::sendServerSideState(ConnectionId loopId)
{
    std::lock_guard lock(this->_mutex);
    WorkerSession* currentSession = getWorkerSessionByConnectionId(loopId);
    if (!currentSession) {
        std::cout << "sendServerSideState(): session is null" << std::endl;
        return;
    }
    
    const auto& state = mState->getWorkstationState(currentSession->workerId);
    if (!state.has_value()) {
        std::cout << "sendServerSideState(): null state" << std::endl;
        return;
    }
    SendStateCommand command = SendStateCommand(state.value());
    NetworkUtils::sendJson(currentSession->mConnectedSocket, std::move(command.toJson()));
}
void ServerHandler::registerWorkstationConnection(const WorkstationId& mainId, const std::string& workstationName, ConnectionId connectionId,SOCKET clientSocket)
{
    WorkerSession session{};
    session.connectionId = connectionId;
    session.workerId = mainId;
    session.mConnectedSocket = clientSocket;
    session.mSessionStatus = WorkerSessionStatus::Registered;
    {
        std::lock_guard guard(this->_mutex);
        mSessions[mainId].emplace_front(session);
        workerIdByConnections[connectionId] = mainId;
    }
    mState->registerCashier(mainId, workstationName);
    publishStateSnapshotLocked();
}
void ServerHandler::cleanupWorkerSession(ConnectionId connectionId)
{
    std::lock_guard guard(_mutex);
    const auto workerIterator = this->workerIdByConnections.find(connectionId);
    if (workerIterator == workerIdByConnections.end()) {
        return;
    }
    const WorkstationId workerId = workerIterator->second;

    auto sessionIt = mSessions.find(workerId);//iterator
    if (sessionIt == mSessions.end())return;
    std::deque<WorkerSession>& deque = sessionIt->second;
    if (deque.empty())return;
    WorkerSession record = deque.front();
    if (record.connectionId == connectionId) {
        deque.pop_front();
        this->workerIdByConnections.erase(connectionId);
        if (deque.empty()) {
            mSessions.erase(workerId);
            this->mState->unregisterCashier(workerId);
            publishStateSnapshotLocked();
        }
    }
}
void ServerHandler::removeWorkstation(const WorkstationId& workerId)
{
    std::lock_guard guard(_mutex);
    auto session = mSessions.find(workerId);
    if (session == mSessions.end())return;
    if (session->second.empty())return;
    auto& record = session->second.front();
    if (record.mSessionStatus != WorkerSessionStatus::Registered) {
        throw std::exception("removeWorkstation");
    }
    session->second.pop_front();
    this->workerIdByConnections.erase(record.connectionId);
    this->mState->unregisterCashier(workerId);
    publishStateSnapshotLocked();

}
void ServerHandler::refreshSessionTimeout(ConnectionId connectionId)
{
    std::lock_guard guard(_mutex);

    auto workerIt = workerIdByConnections.find(connectionId);
    if (workerIt == workerIdByConnections.end()) {
        return ;
    }

    auto sessionIt = mSessions.find(workerIt->second);
    if (sessionIt == mSessions.end()) {
        return ;
    }

    for (WorkerSession& record : sessionIt->second) {
        if (record.connectionId == connectionId) {
            record.lastActivity = std::chrono::steady_clock::now();
            return;
        }
    }
}
void ServerHandler::publishStateSnapshotLocked()
{
    if (!mState || !mRepository)
    {
        return;
    }

    auto snapshot = mState->getCashiersSnapshot();

    mRepository->setSnapshot(std::move(snapshot));
}
void ServerHandler::doHeartbeat(ConnectionId connectionId, SOCKET socket)
{
    NetworkUtils::sendJson(socket, std::move(AskStateCommand().toJson()));
}
void ServerHandler::service()
{
    while (isRunning.load()) {
        mState->checkTimeouts();
        this->publishStateSnapshotLocked();
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}
WorkerSession* ServerHandler::getWorkerSessionByConnectionId(ConnectionId connectionId)
{
    const auto possibleWorkerId = workerIdByConnections.find(connectionId);
    if (possibleWorkerId == workerIdByConnections.end()) {
        return nullptr;
    }
    const std::string workerId = possibleWorkerId->second;
    const auto possibleSession = mSessions.find(workerId);
    if (possibleSession == mSessions.end()) {
        return nullptr;
    }
    for ( WorkerSession& record : possibleSession->second) {
        if (record.connectionId == connectionId)return &record;
    }
    return nullptr;
}


