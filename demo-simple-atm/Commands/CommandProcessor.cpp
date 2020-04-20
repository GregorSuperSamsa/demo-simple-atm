#include "CommandProcessor.h"
#include "Login.h"
#include "Logout.h"
#include "History.h"
#include "Balance.h"
#include "Transfer.h"
#include "Withdraw.h"
#include "Deposit.h"
#include <algorithm>


const std::string CommandProcessor::CMD_LOGIN_STRING         = "login";
const std::string CommandProcessor::CMD_LOGIN_DESCRIPTION    = "login     - Create new user sesssion: login [username] [password]";
const std::string CommandProcessor::CMD_LOGOUT_STRING        = "logout";
const std::string CommandProcessor::CMD_LOGOUT_DESCRIPTION   = "logout    - Close current user session";
const std::string CommandProcessor::CMD_BALANCE_STRING       = "balance";
const std::string CommandProcessor::CMD_BALANCE_DESCRIPTION  = "balance   - Check your current balance";
const std::string CommandProcessor::CMD_DEPOSIT_STRING       = "deposit";
const std::string CommandProcessor::CMD_DEPOSIT_DESCRIPTION  = "deposit   - Deposit amount, as a whole number only, to your account: deposit [amount]";
const std::string CommandProcessor::CMD_WITHDRAW_STRING      = "withdraw";
const std::string CommandProcessor::CMD_WITHDRAW_DESCRIPTION = "withdraw  - Withdraw amount, as a whole number only, from your account: withdraw [amount]";
const std::string CommandProcessor::CMD_TRANSFER_STRING      = "transfer";
const std::string CommandProcessor::CMD_TRANSFER_DESCRIPTION = "transfer  - Transfer amount, as a whole number only, to another user account: transfer [amount] [username]";
const std::string CommandProcessor::CMD_HISTORY_STRING       = "history";
const std::string CommandProcessor::CMD_HISTORY_DESCRIPTION  = "history   - Show list of previous actions";
const std::string CommandProcessor::CMD_HELP_STRING          = "help";
const std::string CommandProcessor::CMD_HELP_DESCRIPTION     = "help      - Show help";


CommandProcessor::CommandProcessor(std::shared_ptr<Account> account)
{
    // Set the Account instance this command processor will operate on
    this->account = account;
    //
    Initialize();
}

void CommandProcessor::Initialize()
{
    // Login
    std::shared_ptr<Login>login = std::make_shared<Login>(CMD_LOGIN_STRING, CMD_LOGIN_DESCRIPTION);
    login->Initialize(account);
    commands.push_back(login);
    // Logout
    std::shared_ptr<Logout> logout = std::make_shared<Logout>(CMD_LOGOUT_STRING, CMD_LOGOUT_DESCRIPTION);
    logout->Initialize(account);
    commands.push_back(logout);
    // History
    std::shared_ptr<History> history = std::make_shared<History>(CMD_HISTORY_STRING, CMD_HISTORY_DESCRIPTION);
    history->Initialize(account);
    commands.push_back(history);
    // Balance
    std::shared_ptr<Balance> balance = std::make_shared<Balance>(CMD_BALANCE_STRING, CMD_BALANCE_DESCRIPTION);
    balance->Initialize(account);
    commands.push_back(balance);
    // Transfer
    std::shared_ptr<Transfer> transfer = std::make_shared<Transfer>(CMD_TRANSFER_STRING, CMD_TRANSFER_DESCRIPTION);
    transfer->Initialize(account);
    commands.push_back(transfer);
    // Withdraw
    std::shared_ptr<Withdraw> withdraw = std::make_shared<Withdraw>(CMD_WITHDRAW_STRING, CMD_WITHDRAW_DESCRIPTION);
    withdraw->Initialize(account);
    commands.push_back(withdraw);
    // Deposit
    std::shared_ptr<Deposit> deposit = std::make_shared<Deposit>(CMD_DEPOSIT_STRING, CMD_DEPOSIT_DESCRIPTION);
    deposit->Initialize(account);
    commands.push_back(deposit);
    // Help
    help = std::make_shared<Help>(CMD_HELP_STRING, CMD_HELP_DESCRIPTION);
    help->Initialize(account);
    commands.push_back(help);

    std::for_each(begin(commands), end(commands), [&](auto command)
    {
        help->Append(command->Description());
    });
}

std::shared_ptr<CommandInterface> CommandProcessor::Parse(const std::string& input, std::vector<std::string>& params)
{
    std::shared_ptr<CommandInterface> command(nullptr);
    const std::string delimiter = " ";

    // Split input by delimiter
    auto split = [&]()
    {
        std::vector<std::string> output;
        auto start = 0U;
        auto end = input.find(" ");
        while (end != std::string::npos)
        {
            output.push_back(input.substr(start, end - start));
            start = end + delimiter.length();
            end = input.find(delimiter, start);
        }
        output.push_back(input.substr(start, end));

        return output;
    };
    params = split();

    // Any found strings
    if (params.size() > 0)
    {
        auto result = find_if(begin(commands), end(commands), [&](auto c)
        {
            return c->String() == params.at(0);
        });

        // If a command was not found, just return help
        if (result != end(commands))
        {
            // Clear the first elemet of the split string, as it contains the command string we have already found
            params.erase(params.begin());
            command = *result;
        }
        else
            command = help;
    }

    return command;
}
