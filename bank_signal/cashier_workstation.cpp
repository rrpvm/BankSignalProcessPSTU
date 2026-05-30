
#include "bank_signal.h"
#include "utilities/CommandLineParser.hpp"
#include <nlohmann/json.hpp>
#include "Application.hpp"


int main(int argc, char** argv)
{
    try
    {
        AppConfig config = CommandLineParser::parse(argc, argv);
        config.mode = AppMode::Workstation;
        Application app(config);
        app.run();

        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Fatal error: " << exception.what() << std::endl;
        return 1;
    }

    return 0;
}
