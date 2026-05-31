#include "AskStateCommand.h"

nlohmann::json AskStateCommand::toJson()
{
	nlohmann::json json;
	json[BASE_COMMAND_TYPE_FIELD] = this->getCommandTypeName();
	return json;
}

AskStateCommand AskStateCommand::fromJson(const nlohmann::json& json) {
	return AskStateCommand();
}