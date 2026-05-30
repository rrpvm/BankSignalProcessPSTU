#pragma once
#include <string>
#include <chrono>
#include "CashierState.hpp"
struct CashierInfo {
	std::string cashierId;
	std::string mName;
	CashierState mState = CashierState::Offline;
	std::chrono::steady_clock::time_point lastHeartBeat;
};