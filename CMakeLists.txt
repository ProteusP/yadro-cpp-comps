cmake_minimum_required(VERSION 3.10)
project(ClubManager)

set(CMAKE_CXX_STANDARD 20)

add_executable(ClubManager
    main.cpp
    ClubManager.cpp
    utils.cpp
)
target_compile_options(ClubManager PRIVATE -Wall -Wextra -O2 -Werror -Wpedantic)