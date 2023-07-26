cmake_minimum_required(VERSION "3.27.0")
project(eugineEngine)

# Add the executable target for main.cpp
add_executable(eugineEngine main.cpp)

target_compile_features(eugineEngine PRIVATE cxx_std_11)