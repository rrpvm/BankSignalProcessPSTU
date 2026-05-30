#pragma once
#include <memory>
#include "../data//ServerState.hpp"
#include "../domain/IHandler.h"
#include "../repository/CashierRepository.h"

#include <winsock2.h>
#include <ws2tcpip.h>

class ServerHandler : public IHandler {
public:
	ServerHandler(std::shared_ptr<CashierRepository> repository);
	~ServerHandler() override;
	virtual void start() override;
	virtual void stop() override;
private:
	void incomingConnectionsLoop();
	void initSocket();
	void clientLoop(SOCKET clientSocket);
private:
	std::shared_ptr<CashierRepository> mRepository;
	std::unique_ptr<ServerState> mState;
	std::mutex _mutex;
	SOCKET mListenSocket = INVALID_SOCKET;

	std::mutex mClientThreadsMutex;
	std::vector<std::thread> mClientThreads;
	bool isInitialisedNetwork{ false };
};