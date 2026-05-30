#pragma comment(lib, "Ws2_32.lib")
#include "ServerHandler.h"
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>
#include <iostream>


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
                    handleInputMessage(message);
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

    std::cout << "Client disconnected" << std::endl;
}

void ServerHandler::handleInputMessage(const std::string& msg)
{
    json inputMessage;
    try
    {
        inputMessage = json::parse(msg);
    }
    catch (const std::exception& e)
    {
        std::cout << "JSON parse error: " << e.what() << std::endl;
       // sendError(clientSocket, "bad_json");
        return;
    }
    if (!inputMessage.contains("type") || !inputMessage["type"].is_string())
    {
        std::cout << "missing type" << std::endl;
       // sendError(clientSocket, "missing_type");
        return;
    }

}

