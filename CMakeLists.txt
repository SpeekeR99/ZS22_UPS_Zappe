cmake_minimum_required(VERSION 3.23)
project(server)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        server
        server/main.cpp
        server/game/Game.cpp
        server/game/Game.h
        server/game/Player.cpp
        server/game/Player.h
        server/game/MyRandom.cpp
        server/game/MyRandom.h
        server/game/StateMachine.cpp
        server/game/StateMachine.h
        server/Server.cpp
        server/Server.h
)
