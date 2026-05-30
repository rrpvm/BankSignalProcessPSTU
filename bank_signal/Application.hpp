#pragma once

#include "render/GuiWindow.hpp"
#include "render/ImGuiLayer.hpp"
#include "render/IScreen.hpp"
#include "data/AppConfig.hpp"
#include "domain/IHandler.h"
#include "repository/CashierRepository.h"
#include "workstation/WorkstationController.h"

#include <thread>
#include <memory>
#include <atomic>

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
    void stop();
private:
    GuiWindow window_;
    ImGuiLayer imguiLayer_;
    std::shared_ptr<CashierRepository>mCashierRepository;
    std::shared_ptr<WorkstationController> mWorkstationController;
    std::unique_ptr<IScreen>_screen;
    std::unique_ptr<IHandler>mHandler;
    std::thread networkHandlerThread;
    std::atomic_bool mRunning;
};