#include "ServerScreen.hpp"

#include <format>
#include "../utilities/BaseUtils.h"
#include "../utilities/GuiUtils.hpp"
#include "../shared.h"

ServerScreen::ServerScreen(const AppConfig& config,  std::shared_ptr<CashierRepository> repository)
    : config_(config)
    , mCashierRepository(std::move(repository))
{
   
}

void ServerScreen::render()
{
    if (mCashierRepository.get()) {
        const auto& data = mCashierRepository->getSnapshot();
        this->mPresentData = mapVector<CashierInfo, CashierModel>(data, [this](const CashierInfo& info) {
            return GuiUtils::CashierInfoToPresentationModel(info);
        });
    }
  
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

    drawCashierCardsGrid(this->mPresentData);

    ImGui::Spacing();
    renderLog();


    ImGui::End();
}

void ServerScreen::renderCashierCard(const CashierModel& cashier, const CashierInfo& info, float cardHeight) const
{
    ImVec4 stateColor = cashier.fColor;

    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.10f, 0.10f, 0.11f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, stateColor);

    std::string childId = "cashier_card_" + std::string(cashier.id);

    ImGui::BeginChild(childId.c_str(), ImVec2(0.0f, cardHeight), true);

    ImGui::Text("%s", cashier.id);
    ImGui::TextDisabled("ID: %s", cashier.id);

    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Text, stateColor);
    ImGui::SetWindowFontScale(1.6f);
    ImGui::Text("%s", cashier.status);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();

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

void ServerScreen::drawCashierCardsGrid(const std::vector<CashierModel>& cashiers) const
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

void ServerScreen::renderLog() const
{
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);

    ImGui::BeginChild("server_log_panel", ImVec2(0, 220), true);

    ImGui::Text(reinterpret_cast<const char*>(u8"Журнал событий"));
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::BeginChild(
        "server_log_inner",
        ImVec2(0, 0),
        false,
        ImGuiWindowFlags_HorizontalScrollbar
    );
    const auto& logs = appLogger.messages();
    if (logs.empty())
    {
        ImGui::TextDisabled(reinterpret_cast<const char*>(u8"Событий пока нет"));
    }
    else
    {
        for (const std::string& item : logs)
        {
            ImGui::BulletText("%s", item.c_str());
        }

        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }
    }

    ImGui::EndChild();

    ImGui::EndChild();

    ImGui::PopStyleVar(2);
    
}