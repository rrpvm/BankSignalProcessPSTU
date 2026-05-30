#pragma once
#include <string>
#include <chrono>
#include "CashierState.hpp"
struct CashierInfo {
	std::string cashierId;
	std::string mName;
	CashierState mState = CashierState::Offline;
	std::chrono::steady_clock::time_point lastHeartBeat;
    bool operator==(const CashierInfo& other) const {
        return cashierId == other.cashierId
            && mName == other.mName
            && mState == other.mState
            && lastHeartBeat == other.lastHeartBeat;
    }

    bool operator!=(const CashierInfo& other) const {
        return !(*this == other);
    }
};