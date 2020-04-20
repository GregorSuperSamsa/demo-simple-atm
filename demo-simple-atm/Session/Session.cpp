#include "Session.h"
#include "../Utils/Debug.h"
#include "../Communication/Defines.h"
#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <deque>


// Debug logging flag
const bool dbg = false;


void Session::Run(int socket)
{
    account = std::make_shared<Account>();
    command_processor = std::make_shared<CommandProcessor>(account);
    char rx_buffer[RX_TX_BUFFER_SIZE_MAX];


    bool running = true;
    while (running)
    {
        int bytes_count = 0;
        std::string input = "";
        std::string output = "";

        // Receive from socket
        while(1)
        {
            bytes_count = recv(socket, &rx_buffer[0], sizeof(rx_buffer), 0);
            if (bytes_count > 0)
            {
                // Copy received bytes to input string
                input.append(std::begin(rx_buffer), std::begin(rx_buffer) + bytes_count);

                // Reset rx_buffer
                memset(rx_buffer, 0, sizeof(rx_buffer));

                // Check if EOF message is received
                auto it = std::find_if(std::begin(input), std::end(input), [](auto& c)
                {
                    return ('\r' == c) || ('\n' == c);
                });

                if (it != std::end(input))
                {
                    input = std::string(std::begin(input), it);
                    break;
                }
            }
            else
            {
                DEBUG(dbg, __PRETTY_FUNCTION__, "Connection closed . . .");

                close(socket);
                return;
            }
        }

        // Process received command
        Process(input, output);

        // Add server prompt to the output
        output.append("\r\n" + std::string(sizeof(SERVER_EOF_MESSAGE_PROMPT), SERVER_EOF_MESSAGE_PROMPT));

        // Send to socket
        const char* tx_data  = output.data();
        std::size_t tx_data_size = output.size();

        while (tx_data_size > 0)
        {
            bytes_count = send(socket, tx_data, tx_data_size, 0);
            if (bytes_count > 0)
            {
                tx_data += bytes_count;
                tx_data_size -= bytes_count;
            }
            else
            {
                DEBUG(dbg, __PRETTY_FUNCTION__, "Connection closed . . .");

                close(socket);
                return;
            }
        }
    }
}

void Session::Process(const std::string& input, std::string& output)
{
    std::vector<std::string> params;
    std::shared_ptr<CommandInterface> cmd = command_processor->Parse(input, params);
    if (cmd != nullptr)
    {
        cmd->Execute(params, output);
    }

    //    auto thread_id = std::this_thread::get_id();
    //    DEBUG(dbg, __PRETTY_FUNCTION__, "Thread id:", thread_id);
    //    DEBUG(dbg, __PRETTY_FUNCTION__, "Input: " + input, "Output: " + output);
}
