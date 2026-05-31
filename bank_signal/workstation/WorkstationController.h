#pragma once

#include "../domain/CashierInfo.hpp"
#include "../data/AppConfig.hpp"

#include <mutex>
#include <string>

enum class WorkstationSyncStatus
{
    NotRegistered,
    Registered,
    PendingState,
    Synced,
    Error,
    Disconnected
};
struct PendingWorkstationState
{
    CashierInfo info;
    std::uint64_t version = 0;
};
class WorkstationController final
{
public:
    WorkstationController(const AppConfig& config);
    void onActionButtonClicked();
    std::optional<PendingWorkstationState> consumePendingState();
    CashierInfo getLocalState() const;
    CashierInfo getServerState() const;
    void onUpdateServerSide(CashierInfo info);
private:
    CashierState nextState(CashierState current) const;

private:
    CashierInfo localState_;
    CashierInfo serverState_;
    mutable std::mutex mutex_;

    WorkstationSyncStatus syncStatus_ = WorkstationSyncStatus::NotRegistered;

    bool hasPendingState_ = false;
    bool isWaitingForServerAccept = false;

    std::uint64_t localVersion_ = 0;
    std::uint64_t pendingVersion_ = 0;
    std::uint64_t confirmedVersion_ = 0;
};