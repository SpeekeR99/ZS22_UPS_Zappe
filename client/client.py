import socket
import select
import time
import pygame
from gui.gui_scenes import *

LOG_FILE = "client.log"
args = []
current_scene = main_menu
game_id = -1
listed_games = []
curr_list_page = 0
opponent_nick = ""
player_dice = []
opponent_dice = []
player_score = "0"
opponent_score = "0"
can_play = False
enemy_played = False
accepted_end_of_round = False
game_over = ""


def send_and_log(client_sock, data):
    print("Sending to server: {}".format(data))
    with open(LOG_FILE, "a") as f:
        f.write("Sending to server: {}\n".format(data))
    client_sock.send(data.encode())


def recv_and_log(client_sock):
    data = client_sock.recv(1024)
    print("Received from server: {}".format(data.decode()))
    with open(LOG_FILE, "a") as f:
        f.write("Received from server: {}".format(data.decode()))
    return data.decode().strip()


def logout(client_sock, gui_input):
    send_and_log(client_sock, gui_input)
    recv_and_log(client_sock)
    pygame.quit()
    sys.exit(0)


def create_game(client_sock, gui_input):
    global current_scene, args, game_id
    send_and_log(client_sock, gui_input)
    data = recv_and_log(client_sock).split("|")
    if data[0] == "CREATE_GAME" and data[1] == "OK":
        game_id = int(data[2])
        current_scene = waiting_for_players
    args = []


def list_games(client_sock, gui_input):
    global current_scene, args, listed_games
    send_and_log(client_sock, gui_input)
    data = recv_and_log(client_sock).split("|")
    if data[0] == "LIST_GAMES" and data[1] == "OK":
        listed_games = data[2:]
        current_scene = list_of_games
    args = [listed_games, curr_list_page]


def join_game(client_sock, gui_input):
    global current_scene, args, game_id, opponent_nick
    game_id = int(gui_input.split("|")[1])
    send_and_log(client_sock, gui_input)
    data = recv_and_log(client_sock).split("|")
    if data[0] == "JOIN_GAME" and data[1] == "OK":
        opponent_nick = data[2]
        current_scene = play
        args = []


def leave_game(client_sock, gui_input):
    global current_scene, args, game_id, player_score, opponent_score, game_over
    player_score, opponent_score = 0, 0
    game_over = ""
    send_and_log(client_sock, gui_input)
    data = recv_and_log(client_sock)
    if data == "LEAVE_GAME|OK":
        current_scene = main_menu
        args = []
        game_id = -1


def reroll(client_sock, gui_input):
    global args, player_dice, can_play
    can_play = False
    send_and_log(client_sock, gui_input)
    data = recv_and_log(client_sock).split("|")
    if data[0] == "REROLL" and data[1] == "OK":
        player_dice = [int(x) for x in data[2].split(",")]
        args = [player_dice, opponent_dice, player_score, opponent_score, nickname, opponent_nick, can_play, enemy_played, game_over]


def accept_end_of_round(client_sock, gui_input):
    global accepted_end_of_round
    if accepted_end_of_round:
        return
    send_and_log(client_sock, gui_input)
    recv_and_log(client_sock).split("|")
    accepted_end_of_round = True


def process_gui_input(gui_input, client_sock):
    global current_scene, args, curr_list_page
    if gui_input == "":
        return
    elif gui_input == "BACK_TO_MAIN_MENU":
        current_scene = main_menu
        args = []
        return
    elif gui_input == "NEXT_PAGE":
        if (curr_list_page + 1) * 5 < len(listed_games):
            curr_list_page += 1
        else:
            curr_list_page = 0
        args = [listed_games, curr_list_page]
        return
    elif gui_input == "PREVIOUS_PAGE":
        if curr_list_page > 0:
            curr_list_page -= 1
        else:
            curr_list_page = len(listed_games) // 5
        args = [listed_games, curr_list_page]
        return
    else:
        FUNCTION_MAP[gui_input.split("|")[0]](client_sock, gui_input)


FUNCTION_MAP = {
    "LOGOUT": logout,
    "CREATE_GAME": create_game,
    "LIST_GAMES": list_games,
    "JOIN_GAME": join_game,
    "LEAVE_GAME": leave_game,
    "REROLL": reroll,
    "ACCEPT_END_OF_ROUND": accept_end_of_round,
}

if len(sys.argv) != 4:
    print("Usage: python {} <ip> <port> <nickname>".format(sys.argv[0]))
    sys.exit(1)

ip = sys.argv[1]
port = int(sys.argv[2])
nickname = sys.argv[3]

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
    client_socket.connect((ip, port))
except socket.error as e:
    print("Failed to connect to server: {}".format(e))
    sys.exit(1)
print("Successfully connected to {}:{}".format(ip, port))

send_and_log(client_socket, "HELLO")
data = recv_and_log(client_socket)
if data != "HELLO":
    print("Server didn't respond with HELLO")
    sys.exit(1)

send_and_log(client_socket, "LOGIN|{}".format(nickname))
data = recv_and_log(client_socket)
if data != "LOGIN|OK":
    print("Server didn't respond with LOGIN|OK")

    if data == "LOGIN|ERR|Nickname is already taken":
        send_and_log(client_socket, "RECONNECT|{}".format(nickname))
        data = recv_and_log(client_socket)
        accepted_end_of_round = False
        game_over = ""
        if data != "RECONNECT|OK":
            print("Server didn't respond with RECONNECT|OK")
            sys.exit(1)

    else:
        sys.exit(1)

socket_fd = client_socket.fileno()

read_fds = [socket_fd]
write_fds = []
exception_fds = []

while True:
    from gui.gui_scenes import gui_input
    readReady, writeReady, exceptionReady = select.select(read_fds, write_fds, exception_fds, 0.01)

    for fd in readReady:
        if fd is socket_fd:
            data = recv_and_log(client_socket)
            if len(data) == 0:
                print("Server is unavailable")
                time.sleep(1)
            else:
                data = data.split("|")
                if data[0] == "JOIN_GAME" and data[1] == "OK":
                    opponent_nick = data[2]
                    current_scene = play
                    args = []
                elif data[0] == "GAME_STATUS" and data[1] == "OK":
                    current_scene = play
                    player_dice = [int(x) for x in data[2].split(",")]
                    opponent_dice = [int(x) for x in data[3].split(",")]
                    if data[6] == "1":
                        can_play = True
                    else:
                        can_play = False
                    if data[7] == "1":
                        enemy_played = False
                    else:
                        enemy_played = True
                    if can_play and not enemy_played:
                        accepted_end_of_round = False
                        game_over = ""
                    player_score = data[4]
                    opponent_score = data[5]
                    if player_score == 3:
                        game_over = "You won!"
                    elif opponent_score == 3:
                        game_over = "You lost!"
                    if player_score == 3 and opponent_score == 3:
                        game_over = "Draw!"
                    opponent_nick = data[8]
                    args = [player_dice, opponent_dice, player_score, opponent_score, nickname, opponent_nick, can_play, enemy_played, game_over]
                elif data[0] == "REROLL_OPPONENT" and data[1] == "OK":
                    opponent_dice = [int(x) for x in data[2].split(",")]
                    enemy_played = True
                    args = [player_dice, opponent_dice, player_score, opponent_score, nickname, opponent_nick, can_play, enemy_played, game_over]
                elif data[0] == "OPPONENT_DISCONNECTED":
                    opponent_nick = opponent_nick + " (Disconnected)"
                    args = [player_dice, opponent_dice, player_score, opponent_score, nickname, opponent_nick, can_play, enemy_played, game_over]
                elif data[0] == "OPPONENT_RECONNECTED":
                    opponent_nick = opponent_nick[:-15]
                    args = [player_dice, opponent_dice, player_score, opponent_score, nickname, opponent_nick, can_play, enemy_played, game_over]
                elif data[0] == "GAME_OVER" and data[1] == "OK":
                    if data[2] == "WIN":
                        game_over = "You won!"
                    elif data[2] == "LOSS":
                        game_over = "You lost!"
                    else:
                        game_over = "Draw!"
                    args = [player_dice, opponent_dice, player_score, opponent_score, nickname, opponent_nick, can_play, enemy_played, game_over]

    process_gui_input(gui_input, client_socket)
    current_scene(args)
