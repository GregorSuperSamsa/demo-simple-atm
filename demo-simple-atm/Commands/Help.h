#ifndef HELP_H
#define HELP_H

#include "CommandInterface.h"


class Help : public CommandInterface
{
public:
    Help(std::string cmd_string, std::string cmd_description);

    void Initialize(const std::shared_ptr<Account>& account) override;
    void Execute(std::vector<std::string> params, std::string& output) override;
    std::string String() const override;
    std::string Description() const override;
    void Append(const std::string& command_description);

private:
    std::string string;
    std::string description;
    std::shared_ptr<Account> account;
    std::vector<std::string> content;

};

#endif // HELP_H
