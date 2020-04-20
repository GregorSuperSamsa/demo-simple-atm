#include "Withdraw.h"


Withdraw::Withdraw(std::string cmd_string, std::string cmd_description)
    : string(cmd_string), description(cmd_description)
{
}

void Withdraw::Initialize(const std::shared_ptr<Account>& account)
{
    this->account = account;
}

void Withdraw::Execute(std::vector<std::string> params, std::string& output)
{
    int amount;
    bool valid_params = (1 == params.size()) && TryInt(params.at(0), amount) && (amount > 0);

    if (valid_params)
    {
        if (account->Withdraw(amount))
            output = "OK";
        else
            output = "Error! Unable to withdraw amount!";
    }
    else  output = "Error! Invalid parameters.";
}

std::string Withdraw::String() const
{
    return string;
}

std::string Withdraw::Description() const
{
    return  description;
}
