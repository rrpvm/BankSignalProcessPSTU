#pragma once
#include <atomic>
#include <winsock2.h>
#include <chrono>
#include <string>
using WorkstationId = std::string;
using ConnectionId = std::uint64_t;
enum class WorkerSessionStatus {
	New,
	Registered,
	Shutdown
};
//Содержит инфу о 'запущенном потоке-соединении'
struct WorkerSession {
	ConnectionId connectionId = 0u;//local id of loop(порядковый номер)
	WorkstationId workerId;//id of worker station(.exe)
	SOCKET mConnectedSocket = INVALID_SOCKET;//сокет соединения
	WorkerSessionStatus mSessionStatus = WorkerSessionStatus::New;

	std::chrono::steady_clock::time_point lastActivity = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point connectedAt = std::chrono::steady_clock::now();
};