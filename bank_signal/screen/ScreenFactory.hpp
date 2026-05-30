#pragma once

#include "../data/AppConfig.hpp"
#include "../render/IScreen.hpp"
#include "../repository/CashierRepository.h"
#include <memory>

class ScreenFactory final
{
public:
    static std::unique_ptr<IScreen> create(const AppConfig& config, std::shared_ptr<CashierRepository> repository );
};