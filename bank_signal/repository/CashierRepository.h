#pragma once

#include <vector>
#include <mutex>

#include "../domain/CashierInfo.hpp"

class CashierRepository final
{
public:
    CashierRepository() = default;
    ~CashierRepository() = default;

    CashierRepository(const CashierRepository&) = delete;
    CashierRepository& operator=(const CashierRepository&) = delete;

    void setSnapshot(const std::vector<CashierInfo>& cashiers);
    void setSnapshot(std::vector<CashierInfo>&& cashiers);

    std::vector<CashierInfo> getSnapshot() const;

    void clear();

private:
    mutable std::mutex mutex_;
    std::vector<CashierInfo> cashiers_;
};