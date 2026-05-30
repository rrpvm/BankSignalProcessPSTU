
#include "ServerState.hpp"

ServerState::ServerState(uintptr_t timeout) : mHeartbeatTimeout(timeout) {};

void ServerState::registerCashier(const std::string& id, const std::string& name)
{
    std::lock_guard guard(this->mLock);
    CashierInfo info;
    info.cashierId = id;
    info.mName = name;
    info.mState = CashierState::Ready;
    info.lastHeartBeat = std::chrono::steady_clock::now();

    mGlobalData[id] = info;
}

void ServerState::unregisterCashier(const std::string& id)
{
    std::lock_guard guard(this->mLock);
    mGlobalData.erase(id);
}

void ServerState::updateCashierState(const std::string& id, CashierState state)
{
    std::lock_guard guard(this->mLock);
    auto cashier = mGlobalData.find(id);

    if (cashier == mGlobalData.end())
    {
        registerCashier(id, id);
        cashier = mGlobalData.find(id);
        if (cashier == mGlobalData.end())
        {
            throw std::exception("unknown error with global data by update cashier state");
        }
    }
    cashier->second.mState = state;
    cashier->second.lastHeartBeat = std::chrono::steady_clock::now();
}

void ServerState::updateHeartbeat(const std::string& id)
{
    std::lock_guard guard(this->mLock);
    auto it = mGlobalData.find(id);

    if (it != mGlobalData.end())
    {
        it->second.lastHeartBeat = std::chrono::steady_clock::now();
    }
}

void ServerState::checkTimeouts()
{
    std::lock_guard guard(this->mLock);
    auto now = std::chrono::steady_clock::now();

    for (auto& [id, cashier] : mGlobalData)
    {
        auto secondsPassed = std::chrono::duration_cast<std::chrono::seconds>(
            now - cashier.lastHeartBeat
        ).count();

        if (secondsPassed > this->mHeartbeatTimeout)
        {
            cashier.mState = CashierState::Offline;
        }
    }
}

void ServerState::reset() {
    std::lock_guard guard(this->mLock);
    this->mGlobalData.clear();
}

std::vector<CashierInfo> ServerState::getCashiersSnapshot()
{
    std::lock_guard guard(this->mLock);

    std::vector<CashierInfo> result;

    for (const auto& [id, cashier] : mGlobalData)
    {
        result.push_back(cashier);
    }

    return result;
}
