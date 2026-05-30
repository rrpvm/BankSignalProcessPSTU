#pragma once

#include "../render/IScreen.hpp"
#include "../data/AppConfig.hpp"
#include "../domain/CashierInfo.hpp"
#include "../workstation/WorkstationController.h"

#include <string>
#include <memory>
#include <vector>
class ClientScreen  final : public IScreen {
public:
	explicit ClientScreen(const AppConfig& config, std::shared_ptr<WorkstationController> controller);
	~ClientScreen() override = default;

	void render() override;

private:
	void drawHeader() const;
	void drawStatusPanel() const;
	void drawActionPlate() const;
	const char* actionButtonText() const;
	CashierState nextState(CashierState current) const;
private:
	std::shared_ptr<WorkstationController> mController;

};