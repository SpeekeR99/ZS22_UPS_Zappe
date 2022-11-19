# Dice Poker

## Description

This is a simple multiplayer dice poker game written in C++ and Python using socket programming

## Game

The game is heavily inspired from the Witcher 1 mini-game

The game is played by two players. Each game consists of up to five rounds. In each round, each player rolls five dice once. The player with the highest poker combination wins the round. The player with three rounds won, wins the game.

### Combinations

The combinations are ranked from the highest to the lowest:

* `Five of a kind` (5 dice with the same value)
* `Four of a kind` (4 dice with the same value)
* `Full house` (3 dice with the same value and 2 dice with the same value)
* `Big straight` (5 dice with consecutive values starting from 2)
* `Small straight` (5 dice with consecutive values starting from 1)
* `Three of a kind` (3 dice with the same value)
* `Two pairs` (2 dice with the same value and 2 dice with the same value)
* `One pair` (2 dice with the same value)
* `High card` (the highest card is always the six)

## Server

The server is written in C++ and is only compatible with Linux

The server application takes one argument from the command line, the port number to listen on

The server not only listens for connections and messages from clients, but also reads the standard input for commands. Possible standard input commands are:

* `exit` - exits the server

### Build

Project is built using CMake. So the only requirement is to have CMake installed. To build the project, run the following commands:

#### Linux

```bash
mkdir build
cd build
cmake ..
make
```

### Run

To run the server, run the following command:

```bash
./server <port>
```

## Client

The client is written in Python and is compatible with both Linux and Windows

The client application takes three arguments from the command line, the server IP address, the server port number and the player chosen nickname

### Build

Project requirement is to have Python 3 installed and the `pygame` module.

To install the `pygame` module, run the following command:

```bash
pip install requirements.txt
```

### Run

To run the client on both Linux and Windows, run the following command:

```bash
python client.py <server_ip> <server_port> <nickname>
```

## Communication protocol

The communication protocol is text based and is not encrypted. The protocol is designed to be simple and easy to implement in any language.

Known commands and their arguments and responses are:

### Ping

C -> S: `ALIVE`

S -> C: `ALIVE`

or

S -> C: `ALIVE|ERR|<error_message>`

Possible error messages are:

* Invalid number of parameters

### Handshake

C -> S: `HELLO`

S -> C: `HELLO`

or

S -> C: `HELLO|ERR|<error_message>`

Possible error messages are:

* Invalid number of parameters

### Login

C -> S: `LOGIN|<nickname>`

S -> C: `LOGIN|OK`

or

S -> C: `LOGIN|ERR|<error_message>`

Possible error messages are:

* Invalid number of parameters
* You are already logged in
* Nickname cannot be longer than XX characters
* Nickname is already taken

### Logout

C -> S: `LOGOUT`

S -> C: `GOODBYE`

or

S -> C: `GOODBYE|ERR|<error_message>`

Possible error messages are:

* Invalid number of parameters

### Reconnect

C -> S: `RECONNECT|<nickname>`

S -> C: `RECONNECT|OK`

or

S -> C: `RECONNECT|ERR|<error_message>`

Possible error messages are:

* Invalid number of parameters
* Name does not exist
* Player is not disconnected

### Opponent Reconnect

S -> C: `OPPONENT_RECONNECTED`

C -> S: `OK`

### Create Game

C -> S: `CREATE_GAME`

S -> C: `CREATE_GAME|OK|<game_id>`

or

S -> C: `CREATE_GAME|ERR|<error_message>`

Possible error messages are:

* Invalid number of parameters
* You are not in main menu

### Join Game

C -> S: `JOIN_GAME|<game_id>`

S -> C: `JOIN_GAME|OK|<opponent_nickname>`

or

S -> C: `JOIN_GAME|ERR|<error_message>`

Possible error messages are:

* Invalid number of parameters
* You are not in main menu
* Game does not exist
* Game is full

### Leave Game

C -> S: `LEAVE_GAME`

S -> C: `LEAVE_GAME|OK`

or

S -> C: `LEAVE_GAME|ERR|<error_message>`

Possible error messages are:

* Invalid number of parameters
* You are not in game

### Opponent Leave Game

S -> C: `LEAVE_GAME_OPPONENT|OK`

C -> S: `OK`

### List Games

C -> S: `LIST_GAMES`

S -> C: `LIST_GAMES|OK|<game_id_1>,<game_id_2>,...,<game_id_n>`

or

S -> C: `LIST_GAMES|ERR|<error_message>`

Possible error messages are:
* Invalid number of parameters
* You are not in main menu

### Game Status

C -> S: `GAME_STATUS`

S -> C: `GAME_STATUS|OK|<player_dice>|<opponent_dice>|<player_score>|<opponent_score>|<player_turn>|<opponent_turn>|<opponent_nickname>`

or

S -> C: `GAME_STATUS|ERR|<error_message>`

Possible error messages are:
* Invalid number of parameters
* You are not in game
* Opponent already left, cannot access information about him

### Reroll

C -> S: `REROLL|<flag_bit_1>,<flag_bit_2>,<flag_bit_3>,<flag_bit_4>,<flag_bit_5>`

S -> C: `REROLL|OK|<dice_1>,<dice_2>,<dice_3>,<dice_4>,<dice_5>`

or

S -> C: `REROLL|ERR|<error_message>`

Possible error messages are:
* Invalid number of parameters
* You are not in game
* You cannot play
* Invalid flag bit XX
* Invalid number of flag bits

### Opponent Reroll

S -> C: `REROLL_OPPONENT|OK|<dice_1>,<dice_2>,<dice_3>,<dice_4>,<dice_5>`

C -> S: `OK`

### End Round

C -> S: `ACCEPT_END_OF_ROUND`

S -> C: `ACCEPT_END_OF_ROUND|OK`

or

S -> C: `ACCEPT_END_OF_ROUND|ERR|<error_message>`

Possible error messages are:
* Invalid number of parameters
* You are not in game
* Game is not at the end of round
* You already accepted the end of round

### Game Over

S -> C: `GAME_OVER|OK|<WIN/LOSS/DRAW>`

C -> S: `OK`
