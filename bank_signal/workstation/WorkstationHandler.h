#pragma once

#include <memory>
#include "../domain/IHandler.h"
#include "../domain/CashierInfo.hpp"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mutex>

class WorkstationHandler final : public IHandler {
public:
	WorkstationHandler();
	~WorkstationHandler() override;
	virtual void start() override;
	virtual void stop() override;
private:
	bool connectToServer();
	void registerForServer();
	void mainLoop();
private:
	CashierInfo mInfo{};
	bool isInitialisedNetwork{ false };
	SOCKET mConnectionSocket = INVALID_SOCKET;
	DWORD dwReadTimeoutMs = 200u;
	std::mutex mMutex;
};