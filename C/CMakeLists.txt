cmake_minimum_required(VERSION 3.28)
project(CLogo)

set(CMAKE_CXX_STANDARD 17)

IF(WIN32)
    add_compile_definitions(WINSOCK)
ENDIF()

add_executable(CLogo
        src/CLogo.h
        src/CLogo.c
        src/CLogo++.cpp
        src/CLogo++.hpp
        src/Clients/SocketLogoClient.hpp
        src/Clients/SocketLogoClient.cpp
        main.cpp)