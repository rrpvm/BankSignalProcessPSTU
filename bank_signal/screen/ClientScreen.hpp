#pragma once

#include "../render/IScreen.hpp"
#include "../data/AppConfig.hpp"


#include <string>
#include <memory>
#include <vector>
class ClientScreen  final : public IScreen {
public:
	explicit ClientScreen(const AppConfig& config);
	~ClientScreen() override = default;


	void render() override;
private:
	void drawHeader() const;
	void drawStatusPanel() const;
};