#include "Transfer.h"


Transfer::Transfer(std::string cmd_string, std::string cmd_description)
    : string(cmd_string), description(cmd_description)
{
}

void Transfer::Initialize(const std::shared_ptr<Account>& account)
{
    this->account = account;
}

void Transfer::Execute(std::vector<std::string> params, std::string& output)
{
    int amount;
    bool valid_params = (2 == params.size()) && TryInt(params.at(0), amount) && (amount > 0);;

    output.clear();
    if (valid_params)
    {
        if (account->Transfer(amount, params.at(1)))
            output = "OK";
        else
            output = "Error! Unable to transfer amount!";
    }
    else  output = "Error! Invalid parameters.";
}

std::string Transfer::String() const
{
    return string;
}

std::string Transfer::Description() const
{
    return description;
}
