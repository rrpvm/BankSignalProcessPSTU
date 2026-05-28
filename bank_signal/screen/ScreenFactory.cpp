#include "ScreenFactory.hpp"
#include "ServerScreen.hpp"

#include <stdexcept>

std::unique_ptr<IScreen> ScreenFactory::create(const AppConfig& config)
{
    switch (config.mode)
    {
    case AppMode::Server:
        return std::make_unique<ServerScreen>(config);

        /*case AppMode::Workstation:
             return std::make_unique<WorkstationScreen>(config);

         case AppMode::Billboard:
             return std::make_unique<BillboardScreen>(config);
             */
    default:
        throw std::runtime_error("Unsupported application mode");
    }
}