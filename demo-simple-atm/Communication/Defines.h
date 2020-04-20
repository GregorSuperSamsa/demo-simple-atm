#ifndef DEFINES_H
#define DEFINES_H

#include <string>

// TODO: Give appropriate namespace name
namespace
{

const std::string DEFAULT_SERVER       = "127.0.0.1";
const int DEFAULT_SERVER_PORT          = 8889;
const char SERVER_EOF_MESSAGE_PROMPT   = '>';
const int SERVER_LISTEN_QUEUE_MAX_SIZE = 5;
const int RX_TX_BUFFER_SIZE_MAX        = 1024;

}

#endif // DEFINES_H
