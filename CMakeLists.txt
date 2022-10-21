cmake_minimum_required(VERSION 3.20)
project(server C)

set(CMAKE_C_STANDARD 99)

add_executable(server server/game/main.c server/game/game.c server/game/game.h server/game/player.c server/game/player.h server/game/typedefs.h server/game/random.c server/game/random.h)
