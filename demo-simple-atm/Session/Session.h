#ifndef SESSION_H
#define SESSION_H

#include <string>
#include <memory>
#include "../Commands/CommandProcessor.h"
#include "../Account/Account.h"


class Session
{
public:
    void Run(int socket);

private:
    std::shared_ptr<Account> account;
    std::shared_ptr<CommandProcessor> command_processor;

    void Process(const std::string &input, std::string &output);
};


#endif // SESSION_H
