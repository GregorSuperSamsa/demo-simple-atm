#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <thread>
#include <mutex>


// Debug logging
template <typename T, typename ... Ts>
void DEBUG(T arg, Ts... args)
{
    static std::mutex mu;

    if (std::is_same<T, bool>::value)
    {
        auto enabled = arg;
        if (enabled)
        {
            int dummy[] = { 0, ((std::cerr << args << std::endl), 0)... };
            static_cast<void>(dummy); // avoid warning for unused variable

            std::lock_guard<std::mutex> locker(mu);
            std::cerr << std::endl;

        }
    }
}

#endif // DEBUG_H
