
#include "RegisterCommand.h"

nlohmann::json RegisterCommand::toJson()
{
    nlohmann::json json;

    json[BASE_COMMAND_TYPE_FIELD] = this->getCommandTypeName();
    json[STATION_FIELD_ID] = this->mStationId;
    json[STATION_FIELD_NAME] = this->mStationName;
    return json;
}

RegisterCommand RegisterCommand::fromJson(const nlohmann::json& json)
{
    return RegisterCommand(
        json.at(STATION_FIELD_ID).get<std::string>(),
        json.at(STATION_FIELD_NAME).get<std::string>()
    );
}
