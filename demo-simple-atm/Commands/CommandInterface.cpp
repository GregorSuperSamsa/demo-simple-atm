#include "CommandInterface.h"


bool CommandInterface::TryInt(const std::string& input, int& output)
{
    bool success = false;

    success = (!input.empty());
    if (success)
    {
        try
        {
            size_t last_char;
            output = std::stoi(input, &last_char);
            // Hack in order to figure out if the amount was truncated
            success = (input.size() == last_char);
        }
        catch (...)
        {
            success = false;
        }
    }
    return success;
}
