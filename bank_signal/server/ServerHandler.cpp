
#include "ServerHandler.h"

ServerHandler::ServerHandler()
{
	this->mState = std::make_unique<ServerState>(ServerState(5u));
}

void ServerHandler::start()
{

}

