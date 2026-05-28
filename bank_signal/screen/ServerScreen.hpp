#pragma once

#include "../data/AppConfig.hpp"
#include "../render/IScreen.hpp"

#include <string>
#include <vector>

struct WorkstationViewModel final
{
    std::string id;
    std::string status;
    std::string currentTicket;
    int signalPort = 0;
};

class ServerScreen final : public IScreen
{
public:
    explicit ServerScreen(const AppConfig& config);
    ~ServerScreen() override = default;

    void render() override;

private:
    void renderHeader() const;
    void renderControls();
    void renderWorkstationsTable() const;
    void renderQueue() const;
    void renderLog() const;

private:
    AppConfig config_;

    int nextTicketNumber_ = 1;
    std::vector<std::string> waitingQueue_;

    std::vector<WorkstationViewModel> workstations_;
    std::vector<std::string> log_;
};