#pragma once
#include "RegisterCommand.h"
#include "SendStateCommand.h"
#include "RegisterResponseCommand.h"

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

            if (type == REGISTER_RESPONSE_COMMAND_TYPE)
            {
                return std::make_unique<RegisterResponseCommand>(
                    RegisterResponseCommand::fromJson(json)
                );
            }

            if (type == "heartbeat")
            {
                return std::make_unique<RegisterCommand>(
                    RegisterCommand::fromJson(json)
                );
            }

            if (type == SEND_STATE_COMMAND_TYPE)
            {
                return std::make_unique<SendStateCommand>(
                    SendStateCommand::fromJson(json)
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