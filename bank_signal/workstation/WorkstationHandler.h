#pragma once

#include <memory>
#include "../domain/IHandler.h"
#include "../domain/CashierInfo.hpp"
#include "WorkstationController.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mutex>

class WorkstationHandler final : public IHandler {
public:
	explicit WorkstationHandler(std::shared_ptr<WorkstationController> controller);
	~WorkstationHandler() override;
	virtual void start() override;
	virtual void stop() override;
private:
	bool connectToServer();
	void sendStateToServer(const CashierInfo& state);
	void registerForServer();
	void handleServerMessage(const std::string& msg);
	void mainLoop();
private:
	std::shared_ptr<WorkstationController> mController;
	bool isInitialisedNetwork{ false };
	SOCKET mConnectionSocket = INVALID_SOCKET;
	DWORD dwReadTimeoutMs = 200u;
	std::mutex mMutex;
};