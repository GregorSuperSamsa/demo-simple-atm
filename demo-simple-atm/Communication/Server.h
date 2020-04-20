#ifndef SERVER_H
#define SERVER_H


#include <arpa/inet.h>


class Server
{
public:
    void Run();

private:
    // Initialize and set up server socket
    bool Initialize();
    //
    void Deinitialize();

    // Local variables
    int server_socket;
    int client_sock_fd;
    int server_port_number;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

};

#endif // SERVER_H
