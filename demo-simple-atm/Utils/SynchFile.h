#ifndef SYNCH_FILE_H
#define SYNCH_FILE_H

#include <queue>
#include <thread>
#include <mutex>
#include <string>
#include <condition_variable>


class SynchFile
{
public:
    SynchFile (const std::string& path);

    void Write (const std::vector<std::string> &content);
    void Write (const std::string &content);
    std::string Filename() const;

private:
    std::string filename_;
    std::mutex write_mutex;
};


#endif // SYNCH_FILE_H
