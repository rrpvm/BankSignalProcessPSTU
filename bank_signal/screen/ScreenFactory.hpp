#pragma once

#include "../data/AppConfig.hpp"
#include "../render/IScreen.hpp"

#include <memory>

class ScreenFactory final
{
public:
    static std::unique_ptr<IScreen> create(const AppConfig& config);
};