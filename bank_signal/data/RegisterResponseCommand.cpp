#include "RegisterResponseCommand.h"

nlohmann::json RegisterResponseCommand::toJson()
{
    nlohmann::json json;

    json[BASE_COMMAND_TYPE_FIELD] = this->getCommandTypeName();
    json[STATION_FIELD_ID] = this->workstationId;
    json[STATION_FIELD_NAME] = this->mName;
    json[CASHIER_STATE_FIELD] = this->mState;
    return json;
}

RegisterResponseCommand RegisterResponseCommand::fromJson(const nlohmann::json& json) {
    return RegisterResponseCommand(
        json.at(STATION_FIELD_ID).get<std::string>(),
        json.at(STATION_FIELD_NAME).get<std::string>(),
        json.at(CASHIER_STATE_FIELD).get<CashierState>()
    );

}