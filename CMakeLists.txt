cmake_minimum_required(VERSION 3.23)
project(server)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        server
        server/game/main.cpp
        server/game/GameCPP.cpp
        server/game/GameCPP.h
        server/game/PlayerCPP.cpp
        server/game/PlayerCPP.h
        server/game/RandomCPP.cpp
        server/game/RandomCPP.h
        server/game/StateMachine.cpp
        server/game/StateMachine.h
)
