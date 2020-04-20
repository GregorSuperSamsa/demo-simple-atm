#ifndef ACCOUNT_H
#define ACCOUNT_H


#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>


class Account
{
public:
    Account();
    ~Account();

    // Supported actions per user commands
    bool Login(const std::string &username, const std::string &password);
    bool Logout();
    bool Balance(int &balance);
    bool Withdraw(const int& amount);
    bool Deposit(const int& amount);
    bool Transfer(const int& amount, const std::string& to_user);
    bool History(std::string &content);

private:
    // Whether this instance of an account is already initalized
    bool Active() const;


    // Extract data from an account based on the provided key value
    bool ExtractAccountEntry(const std::string& key, const std::string& input, std::string& output);


    // Write a history entry to the in-memory collection and persistent account storage
    void WriteHistory(std::string event);
    // Get current history, if in-memory collection is empty, load from persistent account storage
    std::string ReadHistory();


    // Transfer helper functions
    // Load new pending transfers from persistent storage
    std::vector<std::string> LoadPendingTransfers();
    //
    // Close pending transfers and save them to the persistent storage
    void CloseTransfers(const std::vector<std::string>& pending_transfers);
    //
    // Open new transfer and save it to the persistent storage
    void OpenTransfer(const std::string& to_user, const int& amount);
    //
    // Parse a transfer entry from the persistent storage
    bool ParseTransferEntry(const std::string& input, std::string& to_user, std::string &from_user, std::string &amount);
    //
    // Generate new transfer entry string in transfer persistent storage format
    // Do not pass references in case of re-entrant code!
    std::string GenerateTransferEntry(std::string transfer_state, std::string to_user, std::string from_user, std::string amount);
    //


    // Helper functions to keep track of the logged users
    // A user can not have more than one active session in order to simplify
    // usage of the persistent data storages
    void DeactivateSession();
    bool ActivateSession(const std::string& username, const std::string& password);
    bool HasActiveSession(const std::string& username, const std::string& password);


    // Raw read write operations to the filesystem
    void ReadRaw(const std::string& filename, std::vector<std::string>& out_content);
    bool WriteRaw(const std::string& filename, const std::string& content);
    bool WriteRaw(const std::string& filename, const std::vector<std::string> &content);


    // Update account current balance variable with the specified amount and save it to persistent storage
    void UpdateBalance(const int& amount);
    // Get account current balance
    int GetBalance(bool from_storage = false);


    // Check if there is such user in the credentials persistent storage
    bool ValidUser(const std::string& username);


    // Get current date time in string format (Used for history entries)
    std::string DateTimeNow() const;


    // *Not used* Generate uuid string based on the milliseconds from Epoch
    std::string UUID() const;


    // Scheduled task thread
    std::thread scheduler_thread;
    // Atomic flag to stop thread
    std::atomic_bool scheduler_thread_active;
    // Scheduled execution in a thread in order to fetch and process pending transfers asynchronously
    void ScheduledTask(std::atomic_bool& active);
    void ScheduledTaskStart();
    void ScheduledTaskStop();


    // Persistent storage
    std::string USER_ID() const;
    std::string USER_ID(const std::string& username, const std::string& password = "") const;
    std::string BALANCE_STORAGE() const;
    std::string HISTORY_STORAGE() const;
    static const std::string CREDENTIALS_STORAGE;
    static const std::string TRANSFERS_STORAGE;
    static const std::string BALANCE_STORAGE_POSTFIX;
    static const std::string HISTORY_STORAGE_POSTFIX;
    static const std::string STORAGE_DELIMITER;
    static const std::string TRANSFER_PENDING_KEY;
    static const std::string TRANSFER_CLOSED_KEY;
    static const int SCHEDULED_TASK_ITNERVAL_MS;


    // Static vector to keep track of the currently active users
    static std::vector<std::string> active_sessions;
    // Synch access to the common active sessions vector
    static std::mutex mutex_active_sessions;
    // Synch access to the common transfers storage
    static std::mutex mutex_transfers;
    // Synch access to the common credentials storage
    static std::mutex mutex_credentials;
    // Synch access to the local (per account instance) history
    std::mutex mutex_history;
    // Synch access to the local (per account instance) balance variable
    std::mutex mutex_balance;


    // Local variables
    std::string username;
    std::string password;
    int balance;
};

#endif // ACCOUNT_H
