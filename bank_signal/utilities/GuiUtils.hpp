#pragma once
#include <imgui.h>
#include <string>
#include "../domain/CashierInfo.hpp"
#include "../presentation/CashierModel.h"
namespace GuiUtils {
	 const char* StateToText(CashierState state);
	 const char* StateToDescription(CashierState state);
	ImVec4 StateToColor(CashierState state);
	CashierModel CashierInfoToPresentationModel(const CashierInfo& info);
};