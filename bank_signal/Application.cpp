#include "Application.hpp"
#include "screen/ScreenFactory.hpp"
#include "server/ServerHandler.h"

Application::Application(const AppConfig& config)
    : window_(1280, 720, "Bank Signal System")
    , imguiLayer_(window_.nativeHandle())
    , _screen(ScreenFactory::create(config))
    , mRunning(false)

{
    this->mHandler = std::make_unique<ServerHandler>();

}

void Application::stop()
{
    if (!mRunning) {
        return;
    }
    mRunning = false;
    if (networkHandlerThread.joinable()) {
        networkHandlerThread.join();
    }
}

void Application::run()
{
    this->mRunning = true;
    this->networkHandlerThread = std::thread([this]()
        {
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