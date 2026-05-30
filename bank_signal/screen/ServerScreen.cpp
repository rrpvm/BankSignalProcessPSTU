#include "ServerScreen.hpp"


#include <format>
#include "../utilities/GuiUtils.hpp"


ServerScreen::ServerScreen(const AppConfig& config)
    : config_(config)
{
    workstations_.push_back({ "W01", "FREE", "-",GuiUtils::StateToColor(CashierState::Free), 6101});
    workstations_.push_back({ "W02", "BUSY", "A001",GuiUtils::StateToColor(CashierState::Busy), 6102 });
    workstations_.push_back({ "W03", "OFFLINE", "-",GuiUtils::StateToColor(CashierState::Offline), 6103 });
    workstations_.push_back({ "W04", "OFFLINE", "-",GuiUtils::StateToColor(CashierState::Offline), 6103 });
    workstations_.push_back({ "W05", "OFFLINE", "-",GuiUtils::StateToColor(CashierState::Offline), 6103 });

    waitingQueue_.push_back("A002");
    waitingQueue_.push_back("A003");

    log_.push_back("Server GUI initialized");
    log_.push_back("Waiting for workstation connections");
}

void ServerScreen::render()
{
    /*
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
  

    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("MainRootWindow", nullptr, window_flags);

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
    */
    alternateRender();
}

void ServerScreen::alternateRender() const
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);


    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("MainRootWindow", nullptr, window_flags);

    renderHeader();
    ImGui::Separator();

    DrawCashierCardsGrid(this->workstations_);

    ImGui::End();
}


void ServerScreen::renderCashierCard(const WorkstationViewModel& cashier, const CashierInfo& info, float cardHeight) const
{
    ImVec4 stateColor = cashier.fColor;

    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.10f, 0.10f, 0.11f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, stateColor);

    std::string childId = "cashier_card_" + cashier.id;

    ImGui::BeginChild(childId.c_str(), ImVec2(0.0f, cardHeight), true);

    ImGui::Text("%s", cashier.id.c_str());
    ImGui::TextDisabled("ID: %s", cashier.id.c_str());

    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Text, stateColor);
    ImGui::SetWindowFontScale(1.6f);
    ImGui::Text("%s", cashier.status);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();

    ImGui::Spacing();

    //ImGui::Text("Порт: %d", cashier.signalPort);
   // ImGui::Text("Талон: %s", cashier.currentTicket.c_str());

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (info.mState == CashierState::Free)
    {
        ImGui::TextWrapped("Можно направить клиента.");
    }
    else if (info.mState == CashierState::Busy)
    {
        ImGui::TextWrapped("Идёт обслуживание клиента.");
    }
    else if (info.mState == CashierState::Ready)
    {
        ImGui::TextWrapped("Рабочее место готово.");
    }
    else
    {
        ImGui::TextWrapped("Нет связи с процессом кассы.");
    }

    ImGui::EndChild();

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);
}

void ServerScreen::DrawCashierCardsGrid(const std::vector<WorkstationViewModel>& cashiers) const
{
    float availableWidth = ImGui::GetContentRegionAvail().x;

    float minCardWidth = 260.0f;
    float cardHeight = 180.0f;

    int columns = static_cast<int>(availableWidth / minCardWidth);

    if (columns < 1)
    {
        columns = 1;
    }

    if (columns > 5)
    {
        columns = 5;
    }

    ImGuiTableFlags tableFlags =
        ImGuiTableFlags_SizingStretchSame |
        ImGuiTableFlags_NoSavedSettings;

    if (ImGui::BeginTable("cashier_cards_grid", columns, tableFlags))
    {
        for (int i = 0; i < columns; i++)
        {
            ImGui::TableSetupColumn(nullptr);
        }

        for (int i = 0; i < static_cast<int>(cashiers.size()); i++)
        {
            if (i % columns == 0)
            {
                ImGui::TableNextRow();
            }

            ImGui::TableSetColumnIndex(i % columns);
            auto info = CashierInfo();
            info.cashierId = "13376";
            renderCashierCard(cashiers[i],info, cardHeight);
        }

        ImGui::EndTable();
    }
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