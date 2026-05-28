
#include "bank_signal.h"
#include "utilities/CommandLineParser.hpp"
#include <nlohmann/json.hpp>
#include "Application.hpp"

using namespace std;
using json = nlohmann::json;
int main(int argc, char** argv)
{
    try
    {
        const AppConfig config = CommandLineParser::parse(argc, argv);


        std::cout << config.serverPort;
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
