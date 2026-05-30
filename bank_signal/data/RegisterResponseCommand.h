#pragma once
#include "../domain/BaseCommand.h"
#include "../domain/CashierState.hpp"
#define STATION_FIELD_ID "mStationId"
#define STATION_FIELD_NAME "mStationName"
#define CASHIER_STATE_FIELD "mState"
#define REGISTER_RESPONSE_COMMAND_TYPE "register_response"

class RegisterResponseCommand final : public BaseCommand {
public:
    RegisterResponseCommand() = default;

    RegisterResponseCommand(std::string mStationId, std::string mStationName, CashierState state)
        : workstationId(std::move(mStationId))
        , mName(std::move(mStationName))
        , mState(state)
    {
    };
    virtual ~RegisterResponseCommand() override {};
public:
    virtual CommandsType getType() override { return CommandsType::Register; };
    virtual std::string getCommandTypeName() override { return REGISTER_RESPONSE_COMMAND_TYPE; };
    virtual nlohmann::json toJson() override;
    static RegisterResponseCommand fromJson(const nlohmann::json& json);
public:
    std::string cashierId() const
    {
        return workstationId;
    }

    std::string cashierName() const
    {
        return mName;
    }
    CashierState state() const {
        return mState;
    }
private:
    std::string workstationId;
    std::string mName;
    CashierState mState = CashierState::Offline;

};