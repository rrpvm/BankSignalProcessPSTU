#include "ClientScreen.hpp"
#include <imgui.h>
ClientScreen::ClientScreen(const AppConfig& config)
{

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

    ImGui::End();
}

void ClientScreen::drawHeader() const
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.98f, 1.0f));
    ImGui::SetWindowFontScale(1.5f);
    ImGui::Text(reinterpret_cast<const char*>(u8"Рабочее место кассира"));
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();

    ImGui::TextDisabled("ID: %s", "model_.cashierId.c_str()");
    ImGui::SameLine();
    ImGui::Text("|");
    ImGui::SameLine();
    ImGui::TextDisabled("Имя: %s", "model_.cashierName.c_str()");
}

void ClientScreen::drawStatusPanel() const
{
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.10f, 0.10f, 0.12f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f,1.0f,1.0f,1.0f));

    ImGui::BeginChild("status_panel", ImVec2(0, 145), true);

    ImGui::Text(reinterpret_cast<const char*>(u8"Текущее состояние"));

    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::SetWindowFontScale(1.6f);
    ImGui::Text("%s", "stateText()");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();

    ImGui::Spacing();
    ImGui::TextWrapped(reinterpret_cast<const char*>(u8"Текущее состояние кассы передаётся на сервер и отображается на табло."));

    ImGui::EndChild();

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);
}
