#include "Application.hpp"
#include "screen/ScreenFactory.hpp"
#include "server/ServerHandler.h"

Application::Application(const AppConfig& config)
    : window_(1280, 720, "Bank Signal System")
    , imguiLayer_(window_.nativeHandle())
    , mRunning(false)
{
    if (config.mode == AppMode::Server)
    {
        mCashierRepository = std::make_shared<CashierRepository>();

        mHandler = std::make_unique<ServerHandler>(
            mCashierRepository
        );

        _screen = ScreenFactory::create(
            config,
            mCashierRepository
        );
    }
    else
    {
        _screen = ScreenFactory::create(config, nullptr);
    }
}

void Application::stop()
{
    if (!mRunning) {
        return;
    }
    mRunning = false;
    if (mHandler.get()) {
        this->mHandler->stop();
    }
    if (networkHandlerThread.joinable()) {
        networkHandlerThread.join();
    }
}

void Application::run()
{
    this->mRunning = true;
    this->networkHandlerThread = std::thread([this]()
        {
            if (mHandler.get())
                mHandler->start();
        });

    while (!window_.shouldClose())
    {
        window_.pollEvents();

        imguiLayer_.beginFrame();

        this->_screen->render();


        window_.beginFrame(0.10f, 0.10f, 0.12f, 1.00f);

        imguiLayer_.endFrame();

        window_.swapBuffers();
    }
    stop();
}