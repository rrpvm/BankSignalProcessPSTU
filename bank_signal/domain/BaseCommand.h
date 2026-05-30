#pragma once
#include "CommandsType.h"
#include <string>
#include <nlohmann/json.hpp>
#define BASE_COMMAND_TYPE_FIELD  "type"

class BaseCommand {
public:
	BaseCommand() {};
	virtual ~BaseCommand() = default;
	virtual CommandsType getType() { return CommandsType::Unknown; };
	virtual std::string getCommandTypeName() = 0;
	virtual nlohmann::json toJson() = 0;
private:

protected:

};