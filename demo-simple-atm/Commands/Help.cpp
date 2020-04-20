#include "Help.h"
#include <algorithm>


Help::Help(std::string cmd_string, std::string cmd_description)
    : string(cmd_string), description(cmd_description)
{
}

void Help::Initialize(const std::shared_ptr<Account>& account)
{
    this->account = account;
}

void Help::Execute(std::vector<std::string> params, std::string& output)
{
    // Suppress warning for unused variable
    (void)params;
    output.clear();

    std::for_each(begin(content), end(content),
                  [&](std::string line) { output.append(line + "\n"); });
}

std::string Help::String() const
{
    return string;
}

std::string Help::Description() const
{
    return description;
}

void Help::Append(const std::string& command_description)
{
    if (!command_description.empty())
    {
        content.push_back(command_description);
    }
}
