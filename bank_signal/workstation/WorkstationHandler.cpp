#include "WorkstationHandler.h"
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
#include "../utilities/NetworkUtils.h"
#include "../data/RegisterCommand.h"
#include "../data/CommandFactory.h"
using json = nlohmann::json;
WorkstationHandler::WorkstationHandler() {
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

void WorkstationHandler::registerForServer()
{
    RegisterCommand command = RegisterCommand(this->mInfo.cashierId, mInfo.mName);
    NetworkUtils::sendJson(this->mConnectionSocket, std::move(command.toJson()));
}

void WorkstationHandler::mainLoop()
{
    char buffer[1024];

    while (isRunning.load())
    {
        /*
        if (state_ && state_->consumeDirty())
        {
            sendCurrentState();
        }*/

       
        int received = recv(mConnectionSocket, buffer, sizeof(buffer) - 1, 0);

        if (received > 0)
        {
            buffer[received] = '\0';

            std::string message(buffer);
            std::cout << "got: " << message << std::endl;
           // handleServerMessage(message);
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
