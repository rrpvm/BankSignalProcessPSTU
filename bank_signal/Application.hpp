#pragma once

#include "render/GuiWindow.hpp"
#include "render/ImGuiLayer.hpp"
#include "render/IScreen.hpp"
#include "data/AppConfig.hpp"

#include <memory>

class Application final
{
public:
    explicit Application(const AppConfig& config);
    ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    void run();

private:
    GuiWindow window_;
    ImGuiLayer imguiLayer_;
    std::unique_ptr<IScreen>_screen;
   
};