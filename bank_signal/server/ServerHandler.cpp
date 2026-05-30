#pragma comment(lib, "Ws2_32.lib")
#include "ServerHandler.h"
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>
#include <iostream>
#include "../data/CommandFactory.h"

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
    incomingConnectionsLoop();
    /*
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
    */
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

        DWORD timeoutMs = 200;

        setsockopt(
            clientSocket,
            SOL_SOCKET,
            SO_RCVTIMEO,
            reinterpret_cast<const char*>(&timeoutMs),
            sizeof(timeoutMs)
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
    std::uint64_t connectionId = mNextConnectionId.fetch_add(1);
    char buffer[1024];
    std::string receiveBuffer;
    constexpr size_t maxMessageSize = 1024 * 1024;
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
                    handleInputMessage(clientSocket,connectionId, message);
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
    {
        std::lock_guard guard(this->_mutex);
        auto result = mSessionsBinding.find(connectionId);
        if (result != mSessionsBinding.end()) {
            auto sessionWorkerId = result->second;
            mSessions.erase(sessionWorkerId);
        }
    }
   

    std::cout << "Client disconnected" << std::endl;
}

void ServerHandler::handleInputMessage(SOCKET clientSocket, uint64_t loopId, const std::string& msg)
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

    auto command = CommandFactory::fromJson(inputMessage);
    if (!command.get()) {
        std::cout << "from json factory error" << std::endl;
    }
    switch (command->getType())
    {
    case CommandsType::Register:
        handleRegisterCommand(clientSocket,loopId, dynamic_cast<RegisterCommand*>(command.get()));
        break;
    default:
        std::cout << "unhandled type" << command->getCommandTypeName() << std::endl;
    }

}

void ServerHandler::handleRegisterCommand(SOCKET clientSocket, uint64_t loopId, RegisterCommand* command)
{
    const auto& workerId = command->cashierId();
    bool hasAlreadySession = false;
    {
        std::lock_guard guard(this->_mutex);
        hasAlreadySession = mSessions.contains(workerId);
    }

    if (!hasAlreadySession) {
        WorkerSession session = {};
        session.cashierId = workerId;
        session.socket = clientSocket;
        CashierInfo info = {};
        info.cashierId = workerId;
        info.lastHeartBeat = std::chrono::steady_clock::now();
        info.mName = command->cashierName();
        info.mState = CashierState::Ready;
        return  addWorker(session, info);
    }
    //has:
    WorkerSession session;
    {
        std::lock_guard guard(this->_mutex);
        auto result = mSessions.find(workerId);
        if (result != mSessions.end()) {
            session = result->second;
        }
        else {
            std::cout << "непредвиденная ошибка" << std::endl;
            //closeConnection
            return;
        }
    }
    const auto now = std::chrono::steady_clock::now();
    const auto inactiveFor = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - session.lastActivity
    );
    if (inactiveFor.count() > 5000) {
        //shutdown old
        //add new worker
        killConnection(session.socket, workerId);
       // handleRegisterCommand(clientSocket, loopId, command);
    }
    else {
        std::cout << "отмена регистрации" << std::endl;
        killConnection(clientSocket, workerId);
        return;
    }
}

void ServerHandler::killConnection(SOCKET socket, const std::string& workerId)
{
    shutdown(socket, SD_BOTH);
}

void ServerHandler::addWorker(WorkerSession session, CashierInfo info)
{
    {
        std::lock_guard guard(this->_mutex);
        mSessions[info.cashierId] = session;
    }
    mState->registerCashier(info.cashierId,info.mName);
    this->mRepository->setSnapshot(mState->getCashiersSnapshot());
}

