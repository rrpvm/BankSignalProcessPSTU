#include "WorkstationController.h"

WorkstationController::WorkstationController(const AppConfig& config){
    this->localState_ = CashierInfo{};
    localState_.cashierId = config.workstationId;
    localState_.mName = config.workstationName;
}

void WorkstationController::onActionButtonClicked()
{
    if (localState_.mState == CashierState::Offline)return;
    if (hasPendingState_ || isWaitingForServerAccept)return;
    std::lock_guard<std::mutex> lock(mutex_);

    localState_.mState = nextState(localState_.mState);
    localState_.lastHeartBeat = std::chrono::steady_clock::now();

    ++localVersion_;

    pendingVersion_ = localVersion_;
    hasPendingState_ = true;

    syncStatus_ = WorkstationSyncStatus::PendingState;
}

std::optional<PendingWorkstationState> WorkstationController::consumePendingState()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!hasPendingState_)
    {
        return std::nullopt;
    }

    PendingWorkstationState pending;
    pending.info = localState_;
    pending.version = pendingVersion_;

    hasPendingState_ = false;
    isWaitingForServerAccept = true;
    return pending;
}

CashierInfo WorkstationController::getLocalState() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return localState_;
}

CashierInfo WorkstationController::getServerState() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return serverState_;
}

void WorkstationController::onUpdateServerSide(CashierInfo info)
{
    std::lock_guard lock(mutex_);
    this->serverState_ = info;
    localState_ = info;
    isWaitingForServerAccept = false;
}

CashierState WorkstationController::nextState(CashierState current) const
{
    switch (current)
    {
    case CashierState::Ready:
        return CashierState::Free;

    case CashierState::Free:
        return CashierState::Busy;

    case CashierState::Busy:
        return CashierState::Free;

    case CashierState::Offline:
        return CashierState::Ready;

    default:
        return CashierState::Ready;
    }
}


