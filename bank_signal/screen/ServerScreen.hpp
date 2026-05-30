#pragma once

#include "../data/AppConfig.hpp"
#include "../render/IScreen.hpp"
#include "../domain/CashierInfo.hpp"
#include "../repository/CashierRepository.h"
#include "../presentation/CashierModel.h"
#include <string>
#include <memory>
#include <vector>


class ServerScreen final : public IScreen
{
public:
    explicit ServerScreen(const AppConfig& config, std::shared_ptr<CashierRepository> repository);
    ~ServerScreen() override = default;

    void render() override;

private:
    void alternateRender() const;
    void renderCashierCard(const CashierModel& cash,const CashierInfo& info, float cardHeight) const;
    void DrawCashierCardsGrid(const std::vector<CashierModel>& cashiers) const;

    void renderHeader() const;
    void renderControls();
    void renderWorkstationsTable() const;
    void renderQueue() const;
    void renderLog() const;

private:
    //domain
    AppConfig config_;
    std::shared_ptr<CashierRepository>mCashierRepository;
    //presentation
    std::vector<CashierModel> mPresentData;
  
};