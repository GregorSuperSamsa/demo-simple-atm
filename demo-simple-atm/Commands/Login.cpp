#include "Login.h"
#include <algorithm>
#include "../Utils/Debug.h"


const bool dbg = false;


Login::Login(std::string cmd_string, std::string cmd_description)
    : string(cmd_string), description(cmd_description)
{
    auto thread_id = std::this_thread::get_id();
    DEBUG(dbg, __PRETTY_FUNCTION__, "Thread id:", thread_id);
}

void Login::Initialize(const std::shared_ptr<Account>& account)
{
    this->account = account;
}

void Login::Execute(std::vector<std::string> params, std::string& output)
{
    output.clear();
    if (2 == params.size())
    {
        std::string username = params.at(0);
        std::string password = params.at(1);

        if (account->Login(username, password))
            output = "Wellcome, " + username + "!";
        else
            output = "Error! Unable to login.";
    }
    else
        output = "Error! Invalid parameters.";
}

std::string Login::String() const
{
    return string;
}

std::string Login::Description() const
{
    return description;
}
