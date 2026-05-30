#include "CashierRepository.h"

void CashierRepository::setSnapshot(const std::vector<CashierInfo>& cashiers)
{
    std::lock_guard<std::mutex> lock(mutex_);
    cashiers_ = cashiers;
}

void CashierRepository::setSnapshot(std::vector<CashierInfo>&& cashiers)
{
    std::lock_guard<std::mutex> lock(mutex_);
    cashiers_ = std::move(cashiers);
}

std::vector<CashierInfo> CashierRepository::getSnapshot() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return cashiers_;
}

void CashierRepository::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    cashiers_.clear();
}