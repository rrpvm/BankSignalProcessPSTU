#pragma once
#include <imgui.h>
#include <string>
struct CashierModel final {
    std::string id;
    std::string name;
    std::string status;
    std::string description;
    std::string currentTicket;
    std::string timeout;//test
    ImVec4 fColor;
};