cmake_minimum_required(VERSION 3.16)
project(Lexica VERSION 1.0 LANGUAGES CXX)

# Enforce C++17 (required for std::filesystem and structured bindings)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Required internally by cpp-httplib's server thread pool
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Executable target
add_executable(lexica
    src/main.cpp
    src/3_frontEnd/2_webGUI/gui.cpp
)

# Header search paths
target_include_directories(lexica PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/3_frontEnd/2_webGUI/include
)

# Link threading library required by cpp-httplib
target_link_libraries(lexica PRIVATE Threads::Threads)

# Link Windows socket libraries required by cpp-httplib (especially on MinGW)
if(WIN32)
    target_link_libraries(lexica PRIVATE ws2_32 crypt32)
    target_compile_definitions(lexica PRIVATE _WIN32_WINNT=0x0A00)
endif()

# Enable standard compiler warnings
if(MSVC)
    target_compile_options(lexica PRIVATE /W4)
else()
    target_compile_options(lexica PRIVATE -Wall -Wextra -Wpedantic)
endif()