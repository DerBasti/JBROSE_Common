#include "NetworkMessageHandler.h"
#include <iostream>


NetworkMessageHandler::NetworkMessageHandler()
{
}


NetworkMessageHandler::~NetworkMessageHandler()
{
}

void NetworkMessageHandler::accept(const NetworkMessageFragment& fragment) {
	std::cout << fragment.getMessage() << "\n";
}