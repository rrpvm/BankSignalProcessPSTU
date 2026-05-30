#pragma once
#include "../domain/BaseCommand.h"
#include "../domain/CashierInfo.hpp"
#define SEND_STATE_COMMAND_TYPE "send_state"

class SendStateCommand final : public BaseCommand {
public:
    SendStateCommand() = default;

    SendStateCommand(const CashierInfo& info)
        : mInfo(info) {
    };
    virtual ~SendStateCommand() override {};
public:
    virtual CommandsType getType() override { return CommandsType::Register; };
    virtual std::string getCommandTypeName() override { return SEND_STATE_COMMAND_TYPE; };
    virtual nlohmann::json toJson() override;
    static SendStateCommand fromJson(const nlohmann::json& json);
public:
    CashierInfo takeInfo() { return mInfo; };
private:
    CashierInfo mInfo;
};