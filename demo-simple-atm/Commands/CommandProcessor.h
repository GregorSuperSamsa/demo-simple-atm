#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include "CommandInterface.h"
#include "../Account/Account.h"
#include "../Commands/Help.h"


class CommandProcessor
{

public:
    CommandProcessor(std::shared_ptr<Account> account);
    // Parse input string and try to find command to be executed
    std::shared_ptr<CommandInterface> Parse(const std::string &input, std::vector<std::string> &params);

private:
    // Initialize the supported predefined commands
    void Initialize();
    //
    std::shared_ptr<Account> account;
    std::shared_ptr<Help> help;
    // Container for the supported  predefined commands
    std::vector<std::shared_ptr<CommandInterface>> commands;
    //
    static const std::string CMD_LOGIN_STRING;
    static const std::string CMD_LOGOUT_STRING;
    static const std::string CMD_BALANCE_STRING;
    static const std::string CMD_DEPOSIT_STRING;
    static const std::string CMD_WITHDRAW_STRING;
    static const std::string CMD_TRANSFER_STRING;
    static const std::string CMD_HISTORY_STRING;
    static const std::string CMD_HELP_STRING;
    static const std::string CMD_LOGIN_DESCRIPTION;
    static const std::string CMD_LOGOUT_DESCRIPTION;
    static const std::string CMD_BALANCE_DESCRIPTION;
    static const std::string CMD_DEPOSIT_DESCRIPTION;
    static const std::string CMD_WITHDRAW_DESCRIPTION;
    static const std::string CMD_TRANSFER_DESCRIPTION;
    static const std::string CMD_HISTORY_DESCRIPTION;
    static const std::string CMD_HELP_DESCRIPTION;

};

#endif // COMMAND_PROCESSOR_H
