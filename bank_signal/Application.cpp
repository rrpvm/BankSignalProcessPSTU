#include "Application.hpp"
#include "screen/ScreenFactory.hpp"


Application::Application(const AppConfig& config)
    : window_(1100, 750, "Bank Signal System")
    , imguiLayer_(window_.nativeHandle())
    , _screen(ScreenFactory::create(config))
{
}

void Application::run()
{
    while (!window_.shouldClose())
    {
        window_.pollEvents();

        imguiLayer_.beginFrame();

     //   mainScreen_.render();

        window_.beginFrame(0.10f, 0.10f, 0.12f, 1.00f);

        imguiLayer_.endFrame();

        window_.swapBuffers();
    }
}