#include "WorkstationHandler.h"
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
#include "../utilities/NetworkUtils.h"
#include "../data/RegisterCommand.h"
#include "../data/SendStateCommand.h"
#include "../data/CommandFactory.h"
using json = nlohmann::json;


WorkstationHandler::WorkstationHandler(std::shared_ptr<WorkstationController> controller)
{
    this->mController = std::move(controller);
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

WorkstationHandler::~WorkstationHandler()
{
	WSACleanup();
}

void WorkstationHandler::start()
{
    if (isRunning || !isInitialisedNetwork)return;
    this->isRunning.store(true);
    if (!connectToServer()) {
        isRunning.store(false);
        stop();
        return;
    }

    registerForServer();
    mainLoop();
}

void WorkstationHandler::stop()
{
	this->isRunning = false;
    exit(1);
}

bool WorkstationHandler::connectToServer()
{
    mConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (mConnectionSocket == INVALID_SOCKET)
    {
        std::cout << "Client socket creation failed. Error: "
            << WSAGetLastError()
            << "\n";

        return false;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(static_cast<u_short>(6000u));

    inet_pton(
        AF_INET,
        "127.0.0.1",
        &serverAddress.sin_addr
    );

    int result = connect(
        mConnectionSocket,
        reinterpret_cast<sockaddr*>(&serverAddress),
        sizeof(serverAddress)
    );

    if (result == SOCKET_ERROR)
    {
        std::cout << "Connect failed. Error: "
            << WSAGetLastError()
            << "\n";

        closesocket(mConnectionSocket);
        mConnectionSocket = INVALID_SOCKET;

        return false;
    }

    setsockopt(
        mConnectionSocket,
        SOL_SOCKET,
        SO_RCVTIMEO,
        reinterpret_cast<const char*>(&this->dwReadTimeoutMs),
        sizeof(dwReadTimeoutMs)
    );

    std::cout << "Connected to server "
        << "127.0.0.1"
        << ":"
        << "6000"
        << "\n";

    return true;
}

void WorkstationHandler::sendStateToServer(const CashierInfo& state)
{
    SendStateCommand command = SendStateCommand(state);
    NetworkUtils::sendJson(this->mConnectionSocket, std::move(command.toJson()));
}

void WorkstationHandler::registerForServer()
{
    CashierInfo info = mController->getLocalState();
    RegisterCommand command = RegisterCommand(info.cashierId, info.mName);
    NetworkUtils::sendJson(this->mConnectionSocket, std::move(command.toJson()));
}

void WorkstationHandler::handleServerMessage(const std::string& msg)
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
        return;
    }
    switch (command->getType())
    {
    case CommandsType::RegisterResponse: {
        handleRegisterResponse(dynamic_cast<RegisterResponseCommand*>(command.get()));
            break;
    }
                                       //Ack
                                  /* case CommandsType::Ack:
                                       handleGetState(clientSocket, loopId, dynamic_cast<SendStateCommand*>(command.get()));
                                       break;*/

    default:
        std::cout << "unhandled type" << command->getCommandTypeName() << std::endl;
    }
}

void WorkstationHandler::handleRegisterResponse(RegisterResponseCommand* command)
{
    if (!command)return;
    CashierInfo info{};
    info.cashierId = command->cashierId();
    info.mName = command->cashierName();
    info.mState = command->state();
    info.lastHeartBeat = std::chrono::steady_clock::now();

    this->mController->onUpdateServerSide(info);
}

void WorkstationHandler::mainLoop()
{
    char buffer[1024];

    while (isRunning.load())
    {
        if (mController)
        {
            auto pendingState = mController->consumePendingState();

            if (pendingState.has_value())
            {
                sendStateToServer(pendingState->info);
            }
        }
       
        int received = recv(mConnectionSocket, buffer, sizeof(buffer) - 1, 0);

        if (received > 0)
        {
            buffer[received] = '\0';

            std::string message(buffer);
            std::cout << "got: " << message << std::endl;
            handleServerMessage(message);
        }
        else if (received == 0)
        {
            std::cout << "Server closed connection\n";
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

            std::cout << "Client recv error: " << error << "\n";
            break;
        }
    }

    stop();
}
