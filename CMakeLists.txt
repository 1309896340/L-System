cmake_minimum_required(VERSION 3.30)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

project(L-System CXX)

include_directories(
    "include"
)

add_executable(test 
    "src/test.cpp"
)
