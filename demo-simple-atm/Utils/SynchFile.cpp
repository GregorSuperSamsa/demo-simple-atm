#include "SynchFile.h"
#include <fstream>
#include <algorithm>



SynchFile::SynchFile(const std::string& filename) : filename_(filename)
{}

void SynchFile::Write(const std::string &content)
{
    std::vector<std::string> v;
    v.push_back(content);

    Write(v);
}

void SynchFile::Write(const std::vector<std::string> &content)
{
    bool result = false;
    std::ofstream file;

    std::lock_guard<std::mutex> lock(write_mutex);
    if (!content.empty())
    {
        file.open(filename_, std::ios_base::out | std::ios_base::app);
        result = !file.fail() && file.is_open();
        if (result)
        {
            std::for_each(begin(content), end(content),
                          [&](std::string line) { file << line << std::endl;}
            );
            file.close();
        }
    }
}

std::string SynchFile::Filename() const
{
    return filename_;
}
