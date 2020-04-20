#ifndef BALANCE_H
#define BALANCE_H

#include "CommandInterface.h"


class Balance : public CommandInterface
{
public:
    Balance(std::string cmd_string, std::string cmd_description);

    void Initialize(const std::shared_ptr<Account>& account) override;
    void Execute(std::vector<std::string> params, std::string& output) override;
    std::string String() const override;
    std::string Description() const override;

private:
    std::string string;
    std::string description;
    std::shared_ptr<Account> account;

};

#endif // BALANCE_H
