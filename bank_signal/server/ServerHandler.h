#pragma once
#include <memory>
#include "../data//ServerState.hpp"
#include "../domain/IHandler.h"
#include "../repository/CashierRepository.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <atomic>
#include <deque>
#include <optional>
class RegisterCommand;
class SendStateCommand;
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
	std::optional<CashierInfo> handleRegisterCommand(SOCKET clientSocket, uint64_t loopId, RegisterCommand* command);
	void handleGetState(SOCKET clientSocket, uint64_t loopId, SendStateCommand* command);
	struct WorkerSession {
		std::string workerId;//id of exe
		std::uint64_t loopId = 0;//local id of loop
		SOCKET mConnectedSocket = INVALID_SOCKET;//local socket
		std::chrono::steady_clock::time_point lastActivity = std::chrono::steady_clock::now();
		std::chrono::steady_clock::time_point connectedAt =std::chrono::steady_clock::now();
	};
	void killConnection(SOCKET socket,const std::string& workerId);
	void addWorker(WorkerSession session,CashierInfo info, uint64_t loopId );
	void cleanupWorkerSession(uint64_t loopId);

	void publishStateSnapshotLocked()
	{
		if (!mState || !mRepository)
		{
			return;
		}

		const auto& snapshot = mState->getCashiersSnapshot();

		mRepository->setSnapshot(std::move(snapshot));
	}
private:
	std::shared_ptr<CashierRepository> mRepository;
	std::unique_ptr<ServerState> mState;
	std::unordered_map<std::string, WorkerSession> mSessions;
	std::unordered_map<uint64_t, std::string > mSessionsBinding;//привязка connectionId k SOCKET

	std::mutex _mutex;//for class
	SOCKET mListenSocket = INVALID_SOCKET;

	std::mutex mClientThreadsMutex;//for connections
	std::vector<std::thread> mClientThreads;
	bool isInitialisedNetwork{ false };
	std::atomic_uint64_t mNextConnectionId = 1;
};