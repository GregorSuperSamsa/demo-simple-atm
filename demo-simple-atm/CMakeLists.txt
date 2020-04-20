cmake_minimum_required(VERSION 3.5)

project(demo-simple-atm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (UNIX)
    SET(CMAKE_CXX_FLAGS -pthread)
endif(UNIX)

if (MSVC)
    # warning level 4
    add_compile_options(/W4)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Create a list with all .cpp source files
set( SOURCES
    main.cpp
    #
    Communication/Defines.h
    Communication/Server.h
    Communication/Server.cpp
    Communication/Client.h
    Communication/Client.cpp
    #
    Session/Session.h
    Session/Session.cpp
    #
    Commands/CommandInterface.cpp
    Commands/CommandInterface.h
    #
    Commands/CommandProcessor.h
    Commands/CommandProcessor.cpp
    #
    Commands/Login.h
    Commands/Login.cpp
    #
    Commands/Logout.h
    Commands/Logout.cpp
    #
    Commands/History.h
    Commands/History.cpp
    #
    Commands/Help.h
    Commands/Help.cpp
    #
    Commands/Balance.h
    Commands/Balance.cpp
    #
    Commands/Deposit.h
    Commands/Deposit.cpp
    #
    Commands/Withdraw.h
    Commands/Withdraw.cpp
    #
    Commands/Transfer.h
    Commands/Transfer.cpp
    #
    Account/Account.h
    Account/Account.cpp
    #
    Utils/Debug.h
    )

add_executable(demo-simple-atm ${SOURCES})
