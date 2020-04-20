#include "Deposit.h"


Deposit::Deposit(std::string cmd_string, std::string cmd_description)
    : string(cmd_string), description(cmd_description)
{
}

void Deposit::Initialize(const std::shared_ptr<Account>& account)
{
    this->account = account;
}

void Deposit::Execute(std::vector<std::string> params, std::string& output)
{
    int amount;
    bool valid_params = (1 == params.size()) && TryInt(params.at(0), amount) && (amount > 0);

    output.clear();
    if (valid_params)
    {
        if (account->Deposit(amount))
            output = "OK";
        else
            output = "Error! Unable to deposit amount!";
    }
    else  output = "Error! Invalid parameters.";
}

std::string Deposit::String() const
{
    return string;
}

std::string Deposit::Description() const
{
    return description;
}
