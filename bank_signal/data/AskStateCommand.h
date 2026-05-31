#pragma once
#include "../domain/BaseCommand.h"
#include "../domain/CashierInfo.hpp"
#define ASK_STATE_COMMAND_TYPE "ask_state"

class AskStateCommand final : public BaseCommand {
public:
    AskStateCommand() = default;
    virtual ~AskStateCommand() override {};
public:
    virtual CommandsType getType() override { return CommandsType::Heartbeat; };
    virtual std::string getCommandTypeName() override { return ASK_STATE_COMMAND_TYPE; };
    virtual nlohmann::json toJson() override;
    static AskStateCommand fromJson(const nlohmann::json& json);
};