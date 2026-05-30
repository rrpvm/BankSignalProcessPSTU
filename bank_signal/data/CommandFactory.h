#pragma once
#include "RegisterCommand.h"

#include <memory>
#include <nlohmann/json.hpp>

class CommandFactory final
{
public:
    static std::unique_ptr<BaseCommand> fromJson(const nlohmann::json& json)
    {
        if (!json.contains("type") || !json["type"].is_string())
        {
            return nullptr;
        }

        const std::string type = json["type"].get<std::string>();

        try
        {
           
            if (type == REGISTER_COMMAND_TYPE)
            {
                return std::make_unique<RegisterCommand>(
                    RegisterCommand::fromJson(json)
                );
            }

            if (type == "state")
            {
                return std::make_unique<RegisterCommand>(
                    RegisterCommand::fromJson(json)
                );
            }

            if (type == "heartbeat")
            {
                return std::make_unique<RegisterCommand>(
                    RegisterCommand::fromJson(json)
                );
            }

            if (type == "get_state")
            {
                return std::make_unique<RegisterCommand>(
                    RegisterCommand::fromJson(json)
                );
            }
        }
        catch (...)
        {
            return nullptr;
        }

        return nullptr;
    }

    static std::unique_ptr<BaseCommand> fromText(const std::string& text)
    {
        try
        {
            nlohmann::json json = nlohmann::json::parse(text);
            return fromJson(json);
        }
        catch (...)
        {
            return nullptr;
        }
    }
};