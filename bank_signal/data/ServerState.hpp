#pragma once
#include <string>
#include <unordered_map>
#include <mutex>
#include "../domain/CashierInfo.hpp"

class ServerState {
public:
	explicit ServerState(uintptr_t timeout);
	void registerCashier(const std::string& id, const std::string& name);
	void updateCashierState(const std::string& id, CashierState state);
	void updateHeartbeat(const std::string& id);

	void checkTimeouts();

	void reset();
	std::vector<CashierInfo> getCashiersSnapshot();
private:
	std::unordered_map<std::string, CashierInfo> mGlobalData;
	mutable std::mutex mLock;
	const uintptr_t mHeartbeatTimeout;
};