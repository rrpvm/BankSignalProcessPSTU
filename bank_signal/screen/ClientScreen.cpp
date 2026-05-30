#include "ClientScreen.hpp"
#include "../utilities/GuiUtils.hpp"
#include <imgui.h>


ClientScreen::ClientScreen(const AppConfig& config, std::shared_ptr<WorkstationController> controller)
{
    this->mController = std::move(controller);
}

void ClientScreen::render()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("WorkstationScreen", nullptr, flags);
    drawHeader();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    drawStatusPanel();

    ImGui::Spacing();

    drawActionPlate();

    ImGui::End();
}

void ClientScreen::drawHeader() const
{
    static CashierInfo state = this->mController->getLocalState();
    if (state != this->mController->getLocalState()) {
        state = this->mController->getLocalState();
    }

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.98f, 1.0f));
    ImGui::SetWindowFontScale(1.5f);
    ImGui::Text(reinterpret_cast<const char*>(u8"Рабочее место кассира"));
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();
    ImGui::TextDisabled("ID: %s", state.cashierId.c_str());
    ImGui::SameLine();
    ImGui::Text("|");
    ImGui::SameLine();
    ImGui::TextDisabled(reinterpret_cast<const char*>(u8"Имя: %s"), state.mName.c_str());
}

void ClientScreen::drawStatusPanel() const
{
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.10f, 0.10f, 0.12f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f,1.0f,1.0f,1.0f));

    ImGui::BeginChild("status_panel", ImVec2(0, 110), true);

    ImGui::Text(reinterpret_cast<const char*>(u8"Текущее состояние"));

    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::SetWindowFontScale(1.6f);
    ImGui::Text("%s", GuiUtils::StateToDescription(this->mController->getLocalState().mState));
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();

    ImGui::Spacing();

    ImGui::EndChild();

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);
}

void ClientScreen::drawActionPlate() const
{
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);

    ImGui::BeginChild(
        "controls_panel",
        ImVec2(0.0f, 130.0f),
        true,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
    );

    ImGui::Text(reinterpret_cast<const char*>(u8"Управление кассой"));
    ImGui::Spacing();

    ImVec2 buttonSize(260.0f, 55.0f);

    float availableWidth = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availableWidth - buttonSize.x) * 0.5f);
    if (mController->getLocalState().mState == CashierState::Offline) {
        ImGui::BeginDisabled();
    }

    if (ImGui::Button(actionButtonText(), buttonSize))
    {
        this->mController->onActionButtonClicked();
    }
    if (mController->getLocalState().mState == CashierState::Offline) {
        ImGui::EndDisabled();
    }

    ImGui::EndChild();

    ImGui::PopStyleVar(2);
}

const char* ClientScreen::actionButtonText() const
{
    switch (mController->getLocalState().mState)
    {
    case CashierState::Ready:
        return (const char*)u8"Приступить к работе";

    case CashierState::Free:
        return (const char*)u8"Принять клиента";

    case CashierState::Busy:
        return (const char*)u8"Завершить обслуживание клиента";

    case CashierState::Offline:
        return (const char*)u8"Недоступно";

    default:
        return (const char*)u8"Действие";
    }
}

CashierState ClientScreen::nextState(CashierState current) const
{
    switch (current)
    {
    case CashierState::Ready:
        return CashierState::Free;

    case CashierState::Free:
        return CashierState::Busy;

    case CashierState::Busy:
        return CashierState::Free;

    case CashierState::Offline:
        return CashierState::Ready;

    default:
        return CashierState::Ready;
    }
}
