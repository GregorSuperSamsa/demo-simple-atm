#ifndef COMMAND_INTERFACE_H
#define COMMAND_INTERFACE_H


#include "../Account/Account.h"
#include <memory>


class CommandInterface
{
public:
    virtual void Initialize(const std::shared_ptr<Account>& account) = 0;

    // Try to execute the command and return the result in the referenced output
    virtual void Execute(std::vector<std::string> params, std::string& output) = 0;

    // Get string representation of the description of the command
    virtual std::string Description() const = 0;

    // Get string representation of the command
    virtual std::string String() const = 0;

    // Helper function to check whether the input string is a valid integer number
    bool TryInt(const std::string &input, int &output);
};

#endif // COMMAND_INTERFACE_H
