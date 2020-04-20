#ifndef HISTORY_H
#define HISTORY_H

#include "CommandInterface.h"


class History : public CommandInterface
{
public:
    History(std::string cmd_string, std::string cmd_description);

    void Initialize(const std::shared_ptr<Account>& account) override;
    void Execute(std::vector<std::string> params, std::string& output) override;
    std::string String() const override;
    std::string Description() const override;

private:
    std::string string;
    std::string description;
    std::shared_ptr<Account> account;

};

#endif // HISTORY_H
