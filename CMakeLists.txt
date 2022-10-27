cmake_minimum_required(VERSION 3.23)
project(server)

set(CMAKE_CXX_STANDARD 20)

add_executable(server server/game/main.c server/game/game.c server/game/game.h server/game/player.c server/game/player.h server/game/typedefs.h server/game/random.c server/game/random.h server/game/GameCPP.cpp server/game/GameCPP.h server/game/PlayerCPP.cpp server/game/PlayerCPP.h server/game/RandomCPP.cpp server/game/RandomCPP.h)
