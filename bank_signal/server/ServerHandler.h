#pragma once
#include <memory>
#include "../data//ServerState.hpp"
#include "../domain/IHandler.h"
#include "../repository/CashierRepository.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <atomic>
class RegisterCommand;
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
	void handleInputMessage(SOCKET clientSocket, uint64_t loopId, const std::string& msg);
	void handleRegisterCommand(SOCKET clientSocket, uint64_t loopId, RegisterCommand* command);
	struct WorkerSession {
		SOCKET socket = INVALID_SOCKET;
		std::string cashierId;
		std::uint64_t connectionId = 0;
		std::chrono::steady_clock::time_point lastActivity;
		std::chrono::steady_clock::time_point connectedAt =std::chrono::steady_clock::now();
	};
	void killConnection(SOCKET socket,const std::string& workerId);
	void addWorker(WorkerSession session,CashierInfo info );
private:
	std::shared_ptr<CashierRepository> mRepository;
	std::unique_ptr<ServerState> mState;
	std::unordered_map<std::string, WorkerSession> mSessions;
	std::unordered_map<uint64_t, std::string > mSessionsBinding;//привязка connectionId k workerId 

	std::mutex _mutex;//for class
	SOCKET mListenSocket = INVALID_SOCKET;

	std::mutex mClientThreadsMutex;//for connections
	std::vector<std::thread> mClientThreads;
	bool isInitialisedNetwork{ false };
	std::atomic_uint64_t mNextConnectionId = 1;
};