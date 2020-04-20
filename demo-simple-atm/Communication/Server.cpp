#include "Server.h"
#include "Defines.h"
#include "../Session/Session.h"
#include <cstring>
#include <unistd.h>
#include <thread>


bool Server::Initialize()
{
    bool success = false;

    server_socket =  socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    success = (server_socket > 0);

    if (success)
    {
        memset((char *) &server_address, 0, sizeof(server_address));

        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = INADDR_ANY;
        server_address.sin_port = htons(DEFAULT_SERVER_PORT);

        // Try setup socket as reusable due to OS specifics when terminating the application
        int reuse = 1;
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        {
            // TODO:
        }
#ifdef SO_REUSEPORT
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0)
        {
            // TODO:
        }
#endif

        success = (bind(server_socket, (struct sockaddr *) &server_address, sizeof(struct sockaddr_in)) >= 0);
        if (success)
        {
            listen(server_socket, SERVER_LISTEN_QUEUE_MAX_SIZE);
        }
    }

    return success;
}

void Server::Run()
{
    if (Initialize())
    {
        while(1)
        {
            struct sockaddr_in client;
            socklen_t client_len = sizeof (client);

            // On incomming connection start new session thread
            int client_sock = accept(server_socket, (struct sockaddr *) &client, &client_len);
            if (client_sock >= 0)
            {
                std::thread t(&Session::Run, Session(), client_sock);
                t.detach();
            }
        }
        Deinitialize();
    }
}

void Server::Deinitialize()
{
    close(server_socket);
}
