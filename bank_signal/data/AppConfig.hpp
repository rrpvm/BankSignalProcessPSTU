#pragma once

#include "AppMode.hpp"

#include <cstdint>
#include <string>

struct AppConfig final
{
    AppMode mode = AppMode::Server;
    std::string serverHost = "127.0.0.1";
    std::string workstationId = "W01";
    std::string workstationName = "Kassa-00";   
    std::uint16_t serverPort = 6000;
};