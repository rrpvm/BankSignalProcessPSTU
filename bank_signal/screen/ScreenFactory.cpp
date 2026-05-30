#include "ScreenFactory.hpp"
#include "ServerScreen.hpp"
#include "ClientScreen.hpp"

#include <stdexcept>

std::unique_ptr<IScreen> ScreenFactory::create(const AppConfig& config, std::shared_ptr<CashierRepository> repository, std::shared_ptr<WorkstationController> clientController)
{
    switch (config.mode)
    {
    case AppMode::Server:
        return std::make_unique<ServerScreen>(config, repository);
    case AppMode::Workstation:
        return std::make_unique<ClientScreen>(config,clientController);
    default:
        throw std::runtime_error("Unsupported application mode");
    }
}