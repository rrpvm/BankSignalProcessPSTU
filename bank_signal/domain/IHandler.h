#pragma once

class IHandler {
public:
	IHandler() {};
	virtual ~IHandler() {};
	virtual void start() = 0;
private:
};