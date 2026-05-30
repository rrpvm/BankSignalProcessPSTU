#include "NetworkUtils.h"

bool NetworkUtils::sendAll(SOCKET socket, const std::string& message)
{
    if (socket == INVALID_SOCKET)
    {
        return false;
    }

    const char* data = message.c_str();
    int totalSize = static_cast<int>(message.size());
    int totalSent = 0;

    while (totalSent < totalSize)
    {
        int sent = send(
            socket,
            data + totalSent,
            totalSize - totalSent,
            0
        );

        if (sent == SOCKET_ERROR || sent == 0)
        {
            return false;
        }

        totalSent += sent;
    }

    return true;
}

bool NetworkUtils::sendLine(SOCKET socket, const std::string& line)
{
    return sendAll(socket, line + "\n");
}

bool NetworkUtils::sendJson(SOCKET socket, const nlohmann::json& message)
{
    return sendLine(socket, message.dump());
}
