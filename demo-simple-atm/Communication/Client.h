#ifndef CLIENT_H
#define CLIENT_H

#include "Defines.h"
#include <string>
#include <arpa/inet.h>
#include <netdb.h>


class Client
{
  public:
    // Initialize and set up client socket
    bool Initialize(const std::string &hostname, const int &port);
    //
    void Deinitialize();
    //
    void Send(std::string &data);
    std::string Receive();

private:
    bool initialized;
    int client_socket;
    struct sockaddr_in server_address;
    struct hostent * host;

    char rx_buffer[RX_TX_BUFFER_SIZE_MAX];

};

#endif // CLIENT_H
