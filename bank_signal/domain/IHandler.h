#pragma once
#include <atomic>
class IHandler {
public:
	IHandler() {};
	virtual ~IHandler() {};
	virtual void start() = 0;
	virtual void stop() = 0;
protected:
	std::atomic_bool isRunning{ false };
private:
};