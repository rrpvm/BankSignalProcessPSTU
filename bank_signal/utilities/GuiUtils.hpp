#include <imgui.h>
#include <string>
#include "../domain/CashierInfo.hpp"
#include "../presentation/CashierModel.h"
namespace GuiUtils {
	static const char* StateToText(CashierState state);
	static const char* StateToDescription(CashierState state);
	ImVec4 StateToColor(CashierState state);
	CashierModel CashierInfoToPresentationModel(const CashierInfo& info);
};