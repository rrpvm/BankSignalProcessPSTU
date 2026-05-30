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

    if (equals(modeArgument, "workstation"))
    {
        config.mode = AppMode::Workstation;
        config.serverPort = static_cast<std::uint16_t>(6000);
        if (argc >= 3)
        {
            config.workstationId = argv[2];
        }
        return config;
    }

    throw std::runtime_error(
        "Unknown mode. Use: server | workstation <id>"
    );
}

bool CommandLineParser::equals(const char* left, const char* right)
{
    return std::strcmp(left, right) == 0;
}