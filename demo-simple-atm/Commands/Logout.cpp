#include "Logout.h"


Logout::Logout(std::string cmd_string, std::string cmd_description)
    : string(cmd_string), description(cmd_description)
{
}

void Logout::Initialize(const std::shared_ptr<Account>& account)
{
    this->account = account;
}

void Logout::Execute(std::vector<std::string> params, std::string& output)
{
    output.clear();
    if (0 == params.size())
    {
        if (account->Logout())
            output = "Goodbye!";
        else
            output = "Error! Unable to logout!";
    }
    else
        output = "Error! Invalid parameters.";
}

std::string Logout::String() const
{
    return string;
}

std::string Logout::Description() const
{
    return description;
}
