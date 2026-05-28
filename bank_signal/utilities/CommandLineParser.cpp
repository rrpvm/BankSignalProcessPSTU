#include "CommandLineParser.hpp"

#include <cstring>
#include <stdexcept>
#include <string>

AppConfig CommandLineParser::parse(int argc, char** argv)
{
    AppConfig config;

    if (argc < 2)
    {
        config.mode = AppMode::Server;
        return config;
    }

    const char* modeArgument = argv[1];

    if (equals(modeArgument, "server"))
    {
        config.mode = AppMode::Server;
        return config;
    }

    if (equals(modeArgument, "billboard"))
    {
        config.mode = AppMode::Billboard;
        return config;
    }

    if (equals(modeArgument, "workstation"))
    {
        config.mode = AppMode::Workstation;

        if (argc >= 3)
        {
            config.workstationId = argv[2];
        }

        if (argc >= 4)
        {
            const int parsedPort = std::stoi(argv[3]);

            if (parsedPort <= 0 || parsedPort > 65535)
            {
                throw std::runtime_error("Invalid workstation signal port");
            }

            config.signalPort = static_cast<std::uint16_t>(parsedPort);
        }

        return config;
    }

    throw std::runtime_error(
        "Unknown mode. Use: server | workstation <id> <port> | billboard"
    );
}

bool CommandLineParser::equals(const char* left, const char* right)
{
    return std::strcmp(left, right) == 0;
}