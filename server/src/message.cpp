#include "message.hpp"

using namespace gsdk_server;

void message::printType()
{
    std::cout << "Message type: " << to_underlying<MESSAGE_TYPE>(type_) << std::endl;
}
