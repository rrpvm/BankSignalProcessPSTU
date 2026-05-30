#include "SendStateCommand.h"

nlohmann::json SendStateCommand::toJson()
{
	nlohmann::json json;
	json[BASE_COMMAND_TYPE_FIELD] = this->getCommandTypeName();
	json["cashierId"] = this->mInfo.cashierId;
	json["mName"] = this->mInfo.mName;
	json["mState"] = this->mInfo.mState;
	return json;
}
SendStateCommand SendStateCommand::fromJson(const nlohmann::json& json) {
	CashierInfo info{};
	info.cashierId = json.at("cashierId").get<std::string>();
	info.mName = json.at("mName").get<std::string>();
	info.mState = json.at("mState").get<CashierState>();
	info.lastHeartBeat = std::chrono::steady_clock::now();
	return SendStateCommand(info);
}