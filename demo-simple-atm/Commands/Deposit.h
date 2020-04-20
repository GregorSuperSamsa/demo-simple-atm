#ifndef DEPOSIT_H
#define DEPOSIT_H

#include "CommandInterface.h"


class Deposit : public CommandInterface
{
public:
    Deposit(std::string cmd_string, std::string cmd_description);

    void Initialize(const std::shared_ptr<Account>& account) override;
    void Execute(std::vector<std::string> params, std::string& output) override;
    std::string String() const override;
    std::string Description() const override;

private:
    std::string string;
    std::string description;
    std::shared_ptr<Account> account;

};

#endif // DEPOSIT_H
