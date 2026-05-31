#pragma once
#include <memory>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <atomic>
#include <deque>
#include <optional>
#include "WorkerSession.h"
#include "../data//ServerState.hpp"
#include "../domain/IHandler.h"
#include "../repository/CashierRepository.h"

class RegisterCommand;
class SendStateCommand;


class ServerHandler : public IHandler {
public:
	ServerHandler(std::shared_ptr<CashierRepository> repository);
	~ServerHandler() override;
	virtual void start() override;
	virtual void stop() override;
private:
	void initSocket();
	//обработка вход€щих соединений(клиентов)
	void incomingConnectionsLoop();
	//обработчик конкретного соединени€ после accept
	void clientLoop(SOCKET clientSocket);
	//обработчик сообщений внутри соединени€
	void handleInputMessage(SOCKET clientSocket, ConnectionId loopId, const std::string& msg);
	//запрос на прерывание соединени€
	void killConnection(SOCKET socket);
	//обработка команды на регистрацию
	std::optional<CashierInfo> handleRegisterCommand(SOCKET clientSocket, ConnectionId loopId, RegisterCommand* command);
	//обработка получени€ состо€ни€ клиента
	void handleGetState(SOCKET clientSocket, ConnectionId loopId, SendStateCommand* command);
	//отправка состо€ни€ с сервера клиенту - после регистрации\получени€ состо€ни€ от него
	void sendServerSideState(ConnectionId connectionId);
	//служебна€ функци€ - 'официальна€' регистраци€
	void registerWorkstationConnection(const WorkstationId& mainId,const std::string& workstationName, ConnectionId connectionId,SOCKET clientSocket);
	//служебна€ функци€ - очистка соединени€ из сесии
	void cleanupWorkerSession(ConnectionId connectionId);
	//удал€ет worksession из списка + сам  cashier из mState
	void removeWorkstation(const WorkstationId& workerId);
	void refreshSessionTimeout(ConnectionId connectionId);

	//служебна€ функци€ - каждое изменение mState должно дергать repository
	void publishStateSnapshotLocked();

	//@SingleThread!
	WorkerSession * getWorkerSessionByConnectionId(ConnectionId connectionId);
private:
	std::unique_ptr<ServerState> mState;
	std::shared_ptr<CashierRepository> mRepository;
private:
	std::unordered_map< WorkstationId, std::deque<WorkerSession>> mSessions;
	std::unordered_map<ConnectionId, WorkstationId> workerIdByConnections;
	


	SOCKET mListenSocket = INVALID_SOCKET;
	bool isInitialisedNetwork{ false };
	std::vector<std::thread> mClientThreads;
	mutable std::mutex mClientThreadsMutex;//for connections
	mutable std::mutex _mutex;//for class: session etc
	std::atomic_uint64_t mNextConnectionId = 1;
};