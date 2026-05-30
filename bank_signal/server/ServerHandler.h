#pragma once
#include <memory>
#include "../data//ServerState.hpp"
#include "../domain/IHandler.h"

class ServerHandler : public IHandler {
public:
	ServerHandler();
	~ServerHandler() override  = default;
	virtual void start() override;
private:
	std::unique_ptr<ServerState> mState;
	std::mutex _mutex;
};