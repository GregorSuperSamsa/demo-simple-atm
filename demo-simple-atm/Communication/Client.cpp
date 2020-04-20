#include "Client.h"
#include "Defines.h"
#include <algorithm>
#include <cstring>
#include <unistd.h>


bool Client::Initialize(const std::string& hostname, const int& port)
{
    initialized = false;
    bool success = false;

    client_socket =  socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    success = (client_socket > 0);

    if (success)
    {
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port);

        success = (inet_pton(AF_INET, hostname.c_str(), &server_address.sin_addr) > 0);

        if (success)
        {
            success = (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) >= 0);
        }
    }

    initialized = success;

    return success;
}

void Client::Send(std::string &data)
{
    if (!initialized) return;

    // Add "termination" to the input data
    data.append("\n");

    // Send to socket
    int bytes_count;
    const char* tx_data  = data.data();
    std::size_t tx_data_size = data.size();

    while (tx_data_size > 0)
    {
        bytes_count = send(client_socket, tx_data, tx_data_size, 0);
        if (bytes_count > 0)
        {
            tx_data += bytes_count;
            tx_data_size -= bytes_count;
        }
        else
        {
            close(client_socket);
            return;
        }
    }
}

std::string Client::Receive()
{
    std::string output = "";

    if (!initialized) return output;

    // Reset rx_buffer
    memset(rx_buffer, 0, sizeof (rx_buffer));

    // Receive message in chuncks
    int bytes_received = 0;
    int data_index = 0;
    while(1)
    {
        bytes_received = recv(client_socket, &rx_buffer[data_index], sizeof (rx_buffer), 0);
        if (bytes_received > 0)
        {
            output.append(std::begin(rx_buffer), std::begin(rx_buffer) + bytes_received);

            auto it = std::find_if(std::begin(output), std::end(output), [](auto& c)
            {
                return (SERVER_EOF_MESSAGE_PROMPT == c);
            });

            if (it != std::end(output))
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    return output;
}
