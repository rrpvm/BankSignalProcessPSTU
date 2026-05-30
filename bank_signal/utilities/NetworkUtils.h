#pragma once

#include <string>
#include <nlohmann/json.hpp>

#include <winsock2.h>
#include <ws2tcpip.h>



namespace NetworkUtils
{
    bool sendAll(SOCKET socket, const std::string& message);

    bool sendLine(SOCKET socket, const std::string& line);

    bool sendJson(SOCKET socket, const nlohmann::json& message);

  
}