#include "Balance.h"


Balance::Balance(std::string cmd_string, std::string cmd_description)
    : string(cmd_string), description(cmd_description)
{
}

void Balance::Initialize(const std::shared_ptr<Account> &account)
{
    this->account = account;
}

void Balance::Execute(std::vector<std::string> params, std::string& output)
{
    output.clear();
    if (0 == params.size())
    {
        int balance;
        if (account->Balance(balance))
            output = std::to_string(balance);
        else
            output = "Error! Unable to get balance!";
    }
    else
        output = "Error! Invalid parameters.";
}

std::string Balance::String() const
{
    return string;
}

std::string Balance::Description() const
{
    return description;
}
