#include <iostream>
#include "Session/Session.h"
#include "Communication/Server.h"
#include "Communication/Client.h"
#include <thread>



int main()
{
    std::cout << "Wellcome to simple ATM!" << std::endl;

    std::thread server_thread(&Server::Run, Server());

    Client client;
    if (client.Initialize(DEFAULT_SERVER, DEFAULT_SERVER_PORT))
    {
        std::string input;
        std::string output;

        // TODO: Terminate by command
        while(std::getline(std::cin, input))
        {
            client.Send(input);
            output = client.Receive();

            std::cout << output;
        }
    }

    // Try to clean up
    if (server_thread.joinable())
    {
        server_thread.join();
    }

    return 0;
}
