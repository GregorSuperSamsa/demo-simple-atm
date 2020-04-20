#include "Account.h"
#include "../Utils/Debug.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <cstring>
#include <sstream>
#include <limits>


// Debug logging
const bool dbg = false;


const std::string Account::STORAGE_DELIMITER       = ":";
const std::string Account::TRANSFERS_STORAGE       = "transfers";
const std::string Account::CREDENTIALS_STORAGE     = "credentials";
const std::string Account::BALANCE_STORAGE_POSTFIX = "balance";
const std::string Account::HISTORY_STORAGE_POSTFIX = "history";
const std::string Account::TRANSFER_PENDING_KEY    = "<pending>";
const std::string Account::TRANSFER_CLOSED_KEY     = "<closed>";
const int Account::SCHEDULED_TASK_ITNERVAL_MS      = 5000;
//
std::vector<std::string> Account::active_sessions;
//
std::mutex Account::mutex_active_sessions;
std::mutex Account::mutex_transfers;
std::mutex Account::mutex_credentials;

Account::Account()
{
    scheduler_thread_active = false;
    username = "";
    password = "";
    balance = 0;


    auto thread_id = std::this_thread::get_id();
    DEBUG(dbg, __PRETTY_FUNCTION__, "Thread id:", thread_id, "Constructor called!");
}

Account::~Account()
{
    Logout();

    auto thread_id = std::this_thread::get_id();
    DEBUG(dbg, __PRETTY_FUNCTION__, "Thread id:", thread_id, "Destructor called!");
}

std::string Account::USER_ID(const std::string& username, const std::string& password) const
{
    return username + STORAGE_DELIMITER + password;
}

std::string Account::USER_ID() const
{
    return USER_ID(this->username, this->password);
}

std::string Account::BALANCE_STORAGE() const
{
    return USER_ID() + STORAGE_DELIMITER + BALANCE_STORAGE_POSTFIX;
}
std::string Account::HISTORY_STORAGE() const
{
    return USER_ID() + STORAGE_DELIMITER + HISTORY_STORAGE_POSTFIX;
}

bool Account::ValidUser(const std::string& username)
{
    bool result = false;
    std::string dummy;
    std::string line;
    std::ifstream file;;

    // Synchronized access to the shared credentials storage
    std::unique_lock<std::mutex> locker(mutex_credentials, std::defer_lock);
    locker.lock();
    file.open(CREDENTIALS_STORAGE, std::ios_base::in);
    if (!file.fail() && file.is_open())
    {
        while(std::getline(file, line))
        {
            if (ExtractAccountEntry(USER_ID(username), line, dummy))
            {
                result = true;
                break;
            }
        }
        // Just in case
        file.close();
    }
    locker.unlock();

    return result;
}

void Account::WriteHistory(std::string event)
{
    std::lock_guard<std::mutex>locker(mutex_history);

    if (!event.empty())
    {
        event = "[" + DateTimeNow() + "] " + event;
        WriteRaw(HISTORY_STORAGE(), event);
    }
}

std::string Account::ReadHistory()
{
    std::lock_guard<std::mutex>locker(mutex_history);
    std::string history;
    std::vector<std::string> content;

    ReadRaw(HISTORY_STORAGE(), content);
    if (!content.empty())
    {
        for(const auto& line: content)
        {
            history.append(line + "\n");
        }
    }

    return history;
}

std::string Account::DateTimeNow() const
{
    // Exception handling?
    auto tt = std::time(nullptr);

    std::string now = std::asctime(std::localtime(&tt));
    now.erase (std::strcspn (now.c_str(), "\n\r"));

    return now;
}

// Generate UUID based on the time since Epoch in milliseconds
std::string Account::UUID() const
{
    std::ostringstream os;
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    os << millis;

    return "[" + os.str() + "]";
}

bool Account::ExtractAccountEntry(const std::string& key, const std::string& input, std::string& output)
{
    bool result = false;

    output.clear();
    if (input.length() >= key.length())
    {
        if (std::equal(begin(key), end(key), begin(input)))
        {
            if (input.length() != key.length())
            {
                output = input.substr(key.length());
            }

            result = true;
        }
    }
    return result;
}

bool Account::Active() const
{
    return (!username.empty() && !password.empty());
}

bool Account::Login(const std::string &username, const std::string &password)
{
    bool result = false;
    bool valid_credentials = false;

    if (!Active())
    {
        std::vector<std::string> content;

        // Synchronized access to the shared credentials storage
        std::unique_lock<std::mutex> locker(mutex_credentials, std::defer_lock);
        locker.lock();
        ReadRaw(CREDENTIALS_STORAGE, content);
        locker.unlock();

        if (!content.empty())
        {
            std::string user_id = USER_ID(username, password);

            auto it = std::find(begin(content), end(content), user_id);
            if (it != end(content))
                valid_credentials = true;
        }
    }
    // Load all user data and process any pending transfers
    if (valid_credentials)
    {
        if (ActivateSession(username, password))
        {
            UpdateBalance(GetBalance(true));
            WriteHistory("Logged in");
            result = true;
        }
    }


    return result;
}

bool Account::Logout()
{
    bool result = Active();

    if (result)
    {
        WriteHistory("Logged out");
        DeactivateSession();
    }

    return result;
}

bool Account::History(std::string &content)
{
    bool result = Active();

    content.clear();
    if (result)
    {
        content = ReadHistory();
    }

    return result;
}

bool Account::Balance(int &balance)
{
    bool result = Active();

    if (result)
    {
        balance = GetBalance();
    }

    return result;
}

bool Account::Deposit(const int& amount)
{
    bool result = Active();

    if (result)
    {
        UpdateBalance(amount);
        WriteHistory("Deposited " + std::to_string(amount));
    }

    return result;
}

bool Account::Withdraw(const int& amount)
{
    bool result = Active() && (balance >= amount);

    if (result)
    {
        UpdateBalance(amount*(-1));
        WriteHistory("Withdrawed " + std::to_string(amount));
    }

    return result;
}

bool Account::Transfer(const int& amount, const std::string& to_user)
{
    bool result = Active()
            && (balance >= amount)
            && (to_user != username)
            && ValidUser(to_user);

    if (result)
    {
        OpenTransfer(to_user, amount);
        UpdateBalance(amount*(-1));
        WriteHistory("Sent transfer to " + to_user + ": " + std::to_string(amount));
    }

    return result;
}

void Account::OpenTransfer(const std::string& to_user, const int& amount)
{
    std::string entry = GenerateTransferEntry(TRANSFER_PENDING_KEY, to_user, this->username, std::to_string(amount));

    // Synchronized access to the shared transfers storage
    std::unique_lock<std::mutex> locker(mutex_transfers, std::defer_lock);
    locker.lock();
    WriteRaw(TRANSFERS_STORAGE, entry);
    locker.unlock();
    //
}

std::string Account::GenerateTransferEntry(std::string transfer_state, std::string to_user, std::string from_user, std::string amount)
{
    // Transfer entry format:
    // [to_user]:[<pending><closed>]:[from_user]:[amount]

    std::string entry;

    entry.append(to_user).append(STORAGE_DELIMITER);
    entry.append(transfer_state).append(STORAGE_DELIMITER);
    entry.append(from_user).append(STORAGE_DELIMITER);
    entry.append(amount);

    return entry;
}

void Account::CloseTransfers(const std::vector<std::string>& pending_transfers)
{
    std::vector<std::string> closed_transfers;

    if (pending_transfers.size() > 0)
    {
        std::string to_user;
        std::string from_user;
        std::string amount;
        for(auto item : pending_transfers)
        {
            if (ParseTransferEntry(item, to_user, from_user, amount))
            {
                std::string entry = GenerateTransferEntry(TRANSFER_CLOSED_KEY, to_user, from_user, amount);
                closed_transfers.push_back(entry);
                //
                WriteHistory("Received transfer from " + from_user + ": " + amount);
                //
                // Update current balance
                // There is no need for additional string-to-int
                // check as data has already been validated when written
                UpdateBalance(std::stoi(amount));
            }
        }
        if (closed_transfers.size() > 0)
        {
            // Synchronized access to the shared transfers storage
            std::unique_lock<std::mutex> locker(mutex_transfers, std::defer_lock);
            locker.lock();
            WriteRaw(TRANSFERS_STORAGE, closed_transfers);
            locker.unlock();
            //
        }
    }
}

std::vector<std::string> Account::LoadPendingTransfers()
{
    // TODO: Using a list should be faster when removing items
    // TODO: Using static local variable to remember last file ofset will decrement read lines count

    // Transfer entry format:
    // [to_user]:[<pending><closed>]:[from_user]:[amount]

    std::vector<std::string> pending;
    std::vector<std::string> content;

    // Synchronized access to the shared transfers storage
    std::unique_lock<std::mutex> locker(mutex_transfers, std::defer_lock);
    locker.lock();
    ReadRaw(TRANSFERS_STORAGE, content);
    locker.unlock();

    if (!content.empty())
    {
        // Remove any entries that are not for the current user
        content.erase(std::remove_if(
                          content.begin(),
                          content.end(),
                          [&](const std::string &s)
        {
            return (s.substr(0, username.size()) != username);
        }) , content.end());

        // Get latest non closed transfers
        if (!content.empty())
        {
            // Finds the reverse iterator pointing to the lambda's result
            // but using base() to convert back to a forward iterator
            // also 'advances' the resulting forward iterator.
            auto it = std::find_if(
                        std::make_reverse_iterator(end(content)),
                        std::make_reverse_iterator(begin(content)),
                        [](auto& item)
            {
                // Entry contains TRANSFER_CLOSED_KEY
                return (item.find(TRANSFER_CLOSED_KEY) != std::string::npos);
            }).base();

            if (it != end(content))
            {
                std::copy(it, end(content), std::back_inserter(pending));
            }
        }
    }

    return pending;
}

bool Account::ParseTransferEntry(const std::string& input, std::string& to_user, std::string& from_user, std::string& amount)
{
    // Transfer entry format:
    // [to_user]:[<pending><closed>]:[from_user]:[amount]

    bool result = false;
    const int items_count          = 4;
    const int to_user_index        = 0;
    const int transfer_state_index = 1;
    const int from_user_index      = 2;
    const int amount_index         = 3;

    if (!input.empty())
    {
        // TODO: This is a duplicate code snippet, should be external helper function
        // Split input by delimiter
        auto split = [&]()
        {
            std::vector<std::string> output;
            auto start = 0U;
            auto end = input.find(STORAGE_DELIMITER);
            while (end != std::string::npos)
            {
                output.push_back(input.substr(start, end - start));
                start = end + STORAGE_DELIMITER.length();
                end = input.find(STORAGE_DELIMITER, start);
            }
            output.push_back(input.substr(start, end));

            return output;
        };
        //
        std::vector<std::string> items = split();
        if (items.size() == items_count)
        {
            if (items.at(transfer_state_index) == TRANSFER_PENDING_KEY)
            {
                to_user   = items.at(to_user_index);
                from_user = items.at(from_user_index);
                amount    = items.at(amount_index);
                result = true;
            }
        }
    }

    return result;
}

void Account::UpdateBalance(const int& amount)
{
    std::lock_guard<std::mutex>locker(mutex_balance);

    balance = balance + amount;
    WriteRaw(BALANCE_STORAGE(), std::to_string(balance));
}

int Account::GetBalance(bool from_storage)
{
    std::lock_guard<std::mutex>locker(mutex_balance);
    int result;

    if (from_storage)
    {
        // As we are interested only in the last value in the balance persistent storage
        // It might be faster to read the whole content of the file in memory and
        // get only the last line
        std::vector<std::string> content;
        ReadRaw(BALANCE_STORAGE(), content);
        if (!content.empty())
        {
            result = std::stoi(content.back());
        }
        else
            result = 0;
    }
    else
        result = balance;

    return result;
}

bool Account::ActivateSession(const std::string& username, const std::string& password)
{
    bool result = false;

    if (!HasActiveSession(username, password))
    {
        this->username = username;
        this->password = password;

        //
        std::unique_lock<std::mutex> locker(mutex_active_sessions, std::defer_lock);
        locker.lock();
        active_sessions.push_back(USER_ID());
        locker.unlock();

        //
        ScheduledTaskStart();

        result = true;
    }

    return result;
}

void Account::DeactivateSession()
{
    std::string session_key = USER_ID();

    ScheduledTaskStop();

    //
    std::unique_lock<std::mutex> locker(mutex_active_sessions, std::defer_lock);
    locker.lock();
    auto it = std::find_if(begin(active_sessions), end(active_sessions), [&](auto& item)
    {
        return (session_key == item);
    });
    if (it != end(active_sessions))
    {
        active_sessions.erase(it);
    }
    locker.unlock();
    //

    this->username.clear();
    this->password.clear();
}

bool Account::HasActiveSession(const std::string& username, const std::string& password)
{
    bool result = false;
    std::string pattern = USER_ID(username, password);

    //
    std::unique_lock<std::mutex> locker(mutex_active_sessions, std::defer_lock);
    locker.lock();
    auto it = std::find_if(begin(active_sessions), end(active_sessions), [&](auto& item)
    {
        return (pattern == item);
    });
    if (it != end(active_sessions))
    {
        result = true;
    }
    locker.unlock();
    //

    return result;
}

void Account::ReadRaw(const std::string &filename, std::vector<std::string> &content)
{
    // TODO: Based on the line count, new file should be created at some point
    // TODO: Exception handling pending, only in such situations then file.close() would be needed

    bool success = false;
    std::string line;
    std::ifstream file;;

    content.clear();

    file.open(filename, std::ios_base::in);
    success = !file.fail() && file.is_open();
    if (success)
    {
        while(std::getline(file, line))
        {
            if (!line.empty())
            {
                content.push_back(line);
            }
        }
        // Just in case
        file.close();
    }
}

bool Account::WriteRaw(const std::string &filename, const std::vector<std::string> &content)
{
    bool result = false;
    std::ofstream file;

    if (!content.empty())
    {
        file.open(filename, std::ios_base::out | std::ios_base::app);
        result = !file.fail() && file.is_open();
        if (result)
        {
            std::for_each(begin(content), end(content), [&](std::string line)
            {
                file << line << std::endl;
            });
            // Just in case
            file.close();
        }
    }
    return result;
}

bool Account::WriteRaw(const std::string& filename, const std::string& content)
{
    std::vector<std::string> tmp;
    tmp.push_back(content);

    return WriteRaw(filename, tmp);
}

void Account::ScheduledTask(std::atomic_bool& active)
{
    DEBUG(dbg, __PRETTY_FUNCTION__, "Scheduler started!");
    while(active)
    {
        CloseTransfers(LoadPendingTransfers());
        // Sleep
        std::this_thread::sleep_for(std::chrono::milliseconds(SCHEDULED_TASK_ITNERVAL_MS));
    }
    DEBUG(dbg, __PRETTY_FUNCTION__, "Scheduler stopped!");
}

void Account::ScheduledTaskStart()
{
    scheduler_thread_active = true;
    scheduler_thread = std::thread(&Account::ScheduledTask, this, std::ref(scheduler_thread_active));
}

void Account::ScheduledTaskStop()
{
    scheduler_thread_active = false;
    if(scheduler_thread.joinable())
    {
        scheduler_thread.join();
    }
}
