#include "History.h"


History::History(std::string cmd_string, std::string cmd_description)
    : string(cmd_string), description(cmd_description)
{
}

void History::Initialize(const std::shared_ptr<Account>& account)
{
    this->account = account;
}

void History::Execute(std::vector<std::string> params, std::string& output)
{
    output.clear();
    if (0 == params.size())
    {
        if (!account->History(output))
            output = "Error! Unable to get account history";
    }
    else
        output = "Error! Invalid parameters.";
}

std::string History::String() const
{
    return  string;
}

std::string History::Description() const
{
    return description;
}
