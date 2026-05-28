#include "ServerScreen.hpp"

#include "imgui.h"

#include <format>


ServerScreen::ServerScreen(const AppConfig& config)
    : config_(config)
{
    workstations_.push_back({ "W01", "FREE", "-", 6101 });
    workstations_.push_back({ "W02", "BUSY", "A001", 6102 });
    workstations_.push_back({ "W03", "OFFLINE", "-", 6103 });

    waitingQueue_.push_back("A002");
    waitingQueue_.push_back("A003");

    log_.push_back("Server GUI initialized");
    log_.push_back("Waiting for workstation connections");
}

void ServerScreen::render()
{
    ImGui::SetNextWindowSize(ImVec2(1000.0f, 680.0f), ImGuiCond_FirstUseEver);

    ImGui::Begin("Server - central queue controller");

    renderHeader();

    ImGui::Separator();
    renderControls();

    ImGui::Separator();
    renderWorkstationsTable();

    ImGui::Separator();
    renderQueue();

    ImGui::Separator();
    renderLog();

    ImGui::End();
}

void ServerScreen::renderHeader() const
{
    ImGui::Text("Mode: SERVER");
    ImGui::Text("Address: %s:%d", config_.serverHost.c_str(), config_.serverPort);
    ImGui::Text("Purpose: queue storage, workstation status processing, billboard updates");
}

void ServerScreen::renderControls()
{
    if (ImGui::Button("Add client"))
    {
        const std::string ticket = std::format("A{:03}", nextTicketNumber_++);
        waitingQueue_.push_back(ticket);
        log_.push_back("Client added: " + ticket);
    }

    ImGui::SameLine();

    if (ImGui::Button("Reset queue"))
    {
        waitingQueue_.clear();
        nextTicketNumber_ = 1;
        log_.push_back("Queue was reset");
    }
}

void ServerScreen::renderWorkstationsTable() const
{
    ImGui::Text("Connected workstations");

    if (ImGui::BeginTable("workstations_table", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Signal port");
        ImGui::TableSetupColumn("Status");
        ImGui::TableSetupColumn("Current ticket");
        ImGui::TableHeadersRow();

        for (const auto& workstation : workstations_)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", workstation.id.c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%d", workstation.signalPort);

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%s", workstation.status.c_str());

            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%s", workstation.currentTicket.c_str());
        }

        ImGui::EndTable();
    }
}

void ServerScreen::renderQueue() const
{
    ImGui::Text("Waiting queue");

    if (waitingQueue_.empty())
    {
        ImGui::Text("Queue is empty");
        return;
    }

    for (const auto& ticket : waitingQueue_)
    {
        ImGui::BulletText("%s", ticket.c_str());
    }
}

void ServerScreen::renderLog() const
{
    ImGui::Text("Server log");

    for (const auto& record : log_)
    {
        ImGui::BulletText("%s", record.c_str());
    }
}