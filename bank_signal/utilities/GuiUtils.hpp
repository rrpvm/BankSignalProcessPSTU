#include <imgui.h>
#include <string>
#include "../domain/CashierState.hpp"
namespace GuiUtils {
	static const char* StateToText(CashierState state);
	static const char* StateToDescription(CashierState state);
	 ImVec4 StateToColor(CashierState state);
};