#pragma once
#include "../domain/BaseCommand.h"
#define STATION_FIELD_ID "mStationId"
#define STATION_FIELD_NAME "mStationName"
#define REGISTER_COMMAND_TYPE "register"

class RegisterCommand final : public BaseCommand {
public:
    RegisterCommand() = default;

    RegisterCommand( std::string mStationId, std::string mStationName)
        : mStationId(std::move(mStationId))
        , mStationName(std::move(mStationName))
    {};
	virtual ~RegisterCommand() override {};
public:
    virtual CommandsType getType() override  { return CommandsType::Register; };
    virtual std::string getCommandTypeName() override { return REGISTER_COMMAND_TYPE; };
    virtual nlohmann::json toJson() override;
    static RegisterCommand fromJson(const nlohmann::json& json);
public:
    const std::string& cashierId() const
    {
        return mStationId;
    }

    const std::string& cashierName() const
    {
        return mStationName;
    }
private:
    std::string mStationId;
    std::string mStationName;
};