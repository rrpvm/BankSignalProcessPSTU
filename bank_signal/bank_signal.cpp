// bank_signal.cpp: определяет точку входа для приложения.
//

#include "bank_signal.h"

using namespace std;
using json = nlohmann::json;
int main()
{
    json message;

    message["type"] = "STATUS";
    message["workstation"] = "W01";
    message["status"] = "FREE";
    message["ticket"] = 0;

    std::string serialized = message.dump();

    std::cout << "Serialized message:" << std::endl;
    std::cout << serialized << std::endl;

    json parsed = json::parse(serialized);

    std::cout << std::endl;
    std::cout << "Parsed message:" << std::endl;
    std::cout << "type = " << parsed["type"].get<std::string>() << std::endl;
    std::cout << "workstation = " << parsed["workstation"].get<std::string>() << std::endl;
    std::cout << "status = " << parsed["status"].get<std::string>() << std::endl;
    std::cout << "ticket = " << parsed["ticket"].get<int>() << std::endl;

    return 0;
}
