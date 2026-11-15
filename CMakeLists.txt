cmake_minimum_required(VERSION 3.12)
project(Lexica CXX)                                         # project name

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)                                    # the header files

add_executable(lexica                                       # the cpp files
    src/main.cpp
)

# cpphttplib requires pthread support on Linux/macOS
    find_package(Threads REQUIRED)
    target_link_libraries(lexica PRIVATE Threads::Threads)