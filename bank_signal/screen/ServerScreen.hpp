#pragma once

#include "../data/AppConfig.hpp"
#include "../render/IScreen.hpp"
#include "../domain/CashierInfo.hpp"
#include <imgui.h>
#include <string>
#include <vector>

struct WorkstationViewModel final
{
    std::string id;
    std::string status;
    std::string currentTicket;
    ImVec4 fColor;
    int signalPort = 0;
};

class ServerScreen final : public IScreen
{
public:
    explicit ServerScreen(const AppConfig& config);
    ~ServerScreen() override = default;

    void render() override;

private:
    void alternateRender() const;
    void renderCashierCard(const WorkstationViewModel& cash,const CashierInfo& info, float cardHeight) const;
    void DrawCashierCardsGrid(const std::vector<WorkstationViewModel>& cashiers) const;

    void renderHeader() const;
    void renderControls();
    void renderWorkstationsTable() const;
    void renderQueue() const;
    void renderLog() const;

private:
    AppConfig config_;
    //data
    std::vector<CashierInfo> _backendInfo;
    std::vector<std::string> waitingQueue_;
    std::vector<std::string> log_;
    int nextTicketNumber_ = 1;
    //presentation
    std::vector<WorkstationViewModel> workstations_;
  
};