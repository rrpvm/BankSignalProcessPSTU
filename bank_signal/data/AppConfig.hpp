#pragma once

#include "AppMode.hpp"

#include <cstdint>
#include <string>

struct AppConfig final
{
    AppMode mode = AppMode::Server;

    std::string workstationId = "W01";
    std::uint16_t signalPort = 6100;

    std::string serverHost = "127.0.0.1";
    std::uint16_t serverPort = 6000;
};