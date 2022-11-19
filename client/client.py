import socket
import select
import time
import datetime
from gui.gui_scenes import *

# Path to log file
LOG_FILE = "client.log"
# Args for GUI scenes
args = []
# Current scene
current_scene = main_menu
# Game_id of current game
game_id = -1
# Available games to join
listed_games = []
# Current page of list of games
curr_list_page = 0
# Opponent nickname
opponent_nick = ""
# Player dice
player_dice = []
# Opponent dice
opponent_dice = []
# Player score
player_score = "0"
# Opponent score
opponent_score = "0"
# Player turn
player_turn = False
# Opponent turn
opponent_turn = False
# Accepted end of round
accepted_end_of_round = False
# Game over text
game_over = ""


def log(message):
    """
    Logs message to stdout and log file
    :param message: Message to log
    """
    ct = datetime.datetime.now()
    print("[{}]: {}".format(ct, message))
    with open(LOG_FILE, "a") as f:
        f.write("[{}]: {}".format(ct, message))


def send_and_log(client_sock, data):
    """
    Sends data to server and logs it to stdout and log file
    :param client_sock: Socket to send data through
    :param data: Data to send
    """
    log("Sending to server: {}".format(data))
    client_sock.send(data.encode())


def recv_and_log(client_sock):
    """
    Receives data from server and logs it to stdout and log file
    :param client_sock: Client socket to receive data from
    :return: Data received from server
    """
    data = client_sock.recv(1024)
    log("Received from server: {}".format(data.decode()))
    return data.decode().strip()


def handshake_and_login():
    """
    Function handshakes and logs in to server
    """
    global accepted_end_of_round, game_over
    # Handshake with server
    send_and_log(client_socket, "HELLO")
    data = recv_and_log(client_socket)
    if data != "HELLO":
        log("ERROR: Server didn't respond with HELLO")
        sys.exit(1)

    # Send nickname to server
    send_and_log(client_socket, "LOGIN|{}".format(nickname))
    data = recv_and_log(client_socket)
    if data != "LOGIN|OK":
        log("ERROR: Server didn't respond with LOGIN|OK")

        # Try to reconnect if the nickname is already taken
        if data == "LOGIN|ERR|Nickname is already taken":
            send_and_log(client_socket, "RECONNECT|{}".format(nickname))
            data = recv_and_log(client_socket)
            accepted_end_of_round = False
            game_over = ""
            if data != "RECONNECT|OK":
                log("ERROR: Server didn't respond with RECONNECT|OK")
                sys.exit(1)

        else:
            sys.exit(1)


def logout(client_sock, gui_input):
    """
    Function to handle LOGOUT command
    Sends "LOGOUT" to server and closes socket
    :param client_sock: Client socket to send data through
    :param gui_input: GUI input
    """
    send_and_log(client_sock, gui_input)
    recv_and_log(client_sock)
    pygame.quit()
    sys.exit(0)


def create_game(client_sock, gui_input):
    """
    Function to handle CREATE_GAME command
    Sends "CREATE_GAME" to server and receives game_id
    :param client_sock: Client socket to send data through
    :param gui_input: GUI input
    """
    global current_scene, args, game_id
    send_and_log(client_sock, gui_input)
    data = recv_and_log(client_sock).split("|")
    if data[0] == "CREATE_GAME" and data[1] == "OK":
        game_id = int(data[2])
        current_scene = waiting_for_opponent
    args = []


def list_games(client_sock, gui_input):
    """
    Function to handle LIST_GAMES command
    Sends "LIST_GAMES" to server and receives list of games
    :param client_sock: Client socket to send data through
    :param gui_input: GUI input
    """
    global current_scene, args, listed_games
    send_and_log(client_sock, gui_input)
    data = recv_and_log(client_sock).split("|")
    if data[0] == "LIST_GAMES" and data[1] == "OK":
        listed_games = data[2:]
        current_scene = list_of_games
    args = [listed_games, curr_list_page]


def join_game(client_sock, gui_input):
    """
    Function to handle JOIN_GAME command
    Sends "JOIN_GAME|<game id>" to server and receives opponent nickname
    :param client_sock: Client socket to send data through
    :param gui_input: GUI input
    """
    global current_scene, args, game_id, opponent_nick
    game_id = int(gui_input.split("|")[1])
    send_and_log(client_sock, gui_input)
    data = recv_and_log(client_sock).split("|")
    if data[0] == "JOIN_GAME" and data[1] == "OK":
        opponent_nick = data[2]
        current_scene = play
        args = []
    # Game might be already full or not exist at all, update list of games
    elif data[0] == "JOIN_GAME" and data[1] == "ERR":
        list_games(client_sock, "LIST_GAMES")


def leave_game(client_sock, gui_input):
    """
    Function to handle LEAVE_GAME command
    Sends "LEAVE_GAME" to server
    :param client_sock: Client socket to send data through
    :param gui_input: GUI input
    """
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
    """
    Function to handle REROLL command
    Send "REROLL|<flag1>,<flag2>,<flag3>,<flag4>,<flag5>" to server and receives new dice
    :param client_sock: Client socket to send data through
    :param gui_input: GUI input
    """
    global args, player_dice, player_turn
    player_turn = False
    send_and_log(client_sock, gui_input)
    data = recv_and_log(client_sock).split("|")
    if data[0] == "REROLL" and data[1] == "OK":
        player_dice = [int(x) for x in data[2].split(",")]
        args = [player_dice, opponent_dice, player_score, opponent_score, nickname, opponent_nick, player_turn,
                opponent_turn, game_over]


def accept_end_of_round(client_sock, gui_input):
    """
    Function to handle ACCEPT_END_OF_ROUND command
    Sends "ACCEPT_END_OF_ROUND" to server
    :param client_sock: Client socket to send data through
    :param gui_input: GUI input
    """
    global accepted_end_of_round
    if accepted_end_of_round:
        return
    send_and_log(client_sock, gui_input)
    recv_and_log(client_sock).split("|")
    accepted_end_of_round = True


def process_gui_input(gui_input, client_sock):
    """
    Function to process GUI input
    Calls corresponding function based on input thanks to map of functions
    :param gui_input: GUI input
    :param client_sock: Client socket to send data through
    """
    global current_scene, args, curr_list_page
    # Do nothing if input is empty
    if gui_input == "":
        return
    # Go back to main menu from the LIST OF GAMES scene
    elif gui_input == "BACK_TO_MAIN_MENU":
        current_scene = main_menu
        args = []
        return
    # Go to next page of games list
    elif gui_input == "NEXT_PAGE":
        if (curr_list_page + 1) * 5 < len(listed_games):
            curr_list_page += 1
        # If next page is more than number of games / 5, go to first page
        else:
            curr_list_page = 0
        args = [listed_games, curr_list_page]
        return
    # Go to previous page of games list
    elif gui_input == "PREVIOUS_PAGE":
        if curr_list_page > 0:
            curr_list_page -= 1
        # If previous page is less than 0, go to last page
        else:
            curr_list_page = len(listed_games) // 5
        args = [listed_games, curr_list_page]
        return
    # Call function from map of functions
    else:
        FUNCTION_MAP[gui_input.split("|")[0]](client_sock, gui_input)


# Map of functions to call based on input from GUI
FUNCTION_MAP = {
    "LOGOUT": logout,
    "CREATE_GAME": create_game,
    "LIST_GAMES": list_games,
    "JOIN_GAME": join_game,
    "LEAVE_GAME": leave_game,
    "REROLL": reroll,
    "ACCEPT_END_OF_ROUND": accept_end_of_round,
}

# Check if all arguments are provided
if len(sys.argv) != 4:
    print("Usage: python {} <ip> <port> <nickname>".format(sys.argv[0]))
    sys.exit(1)

# Get arguments
ip = sys.argv[1]
port = int(sys.argv[2])
nickname = sys.argv[3]

# Check nickname length
if len(nickname) > 20:
    print("Nickname cannot be longer than 20 characters")
    sys.exit(1)

# Create client socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# Try to connect to server
try:
    client_socket.connect((ip, port))
except socket.error as e:
    log("ERROR: Could not connect to server: {}".format(e))
    sys.exit(1)
log("Successfully connected to server at {}:{}".format(ip, port))

# Handshake and login
handshake_and_login()

# Socket file descriptor
socket_fd = client_socket.fileno()

# Create file descriptor sets for select
read_fds = [socket_fd]
write_fds = []
exception_fds = []

while True:
    # Get the GUI Input from the GUI
    from gui.gui_scenes import gui_input

    # Try to receive message from server or timeout after 0.01 seconds
    readReady, writeReady, exceptionReady = select.select(read_fds, write_fds, exception_fds, 0.01)

    # If there is data to read from server
    for fd in readReady:
        if fd is socket_fd:
            data = recv_and_log(client_socket)
            # If server disconnected, exit
            if len(data) == 0:
                current_scene = disconnected
                new_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                try:
                    new_socket.connect((ip, port))
                    client_socket.shutdown(socket.SHUT_RDWR)
                    client_socket.close()
                    client_socket = new_socket
                    log("Successfully reconnected to server at {}:{}".format(ip, port))
                    handshake_and_login()
                    socket_fd = client_socket.fileno()
                    read_fds = [socket_fd]
                    current_scene = main_menu
                except socket.error as e:
                    log("ERROR: Server is unavailable")
                    new_socket.close()
                    time.sleep(0.1)

            # If server sent a message, process it
            else:
                data = data.split("|")
                # If server sent a message about JOIN_GAME it means that opponent joined the game and the game can start
                if data[0] == "JOIN_GAME" and data[1] == "OK":
                    opponent_nick = data[2]
                    current_scene = play
                    args = []

                # If server sent a message about GAME_STATUS process it and update the GUI accordingly
                elif data[0] == "GAME_STATUS" and data[1] == "OK":
                    current_scene = play
                    player_dice = [int(x) for x in data[2].split(",")]
                    opponent_dice = [int(x) for x in data[3].split(",")]
                    if data[6] == "1":
                        player_turn = True
                    else:
                        player_turn = False
                    if data[7] == "1":
                        opponent_turn = True
                    else:
                        opponent_turn = False
                    if player_turn and opponent_turn:
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
                    args = [player_dice, opponent_dice, player_score, opponent_score, nickname, opponent_nick,
                            player_turn, opponent_turn, game_over]

                # If server sent a message about REROLL_OPPONENT process it and update the GUI accordingly
                elif data[0] == "REROLL_OPPONENT" and data[1] == "OK":
                    opponent_dice = [int(x) for x in data[2].split(",")]
                    opponent_turn = False
                    args = [player_dice, opponent_dice, player_score, opponent_score, nickname, opponent_nick,
                            player_turn, opponent_turn, game_over]

                # If server sent a message about OPPONENT_DISCONNECTED process it and update the GUI accordingly
                elif data[0] == "OPPONENT_DISCONNECTED":
                    opponent_nick = opponent_nick + " (Disconnected)"
                    args = [player_dice, opponent_dice, player_score, opponent_score, nickname, opponent_nick,
                            player_turn, opponent_turn, game_over]

                # If server sent a message about OPPONENT_RECONNECTED process it and update the GUI accordingly
                elif data[0] == "OPPONENT_RECONNECTED":
                    opponent_nick = opponent_nick[:-len(" (Disconnected)")]
                    args = [player_dice, opponent_dice, player_score, opponent_score, nickname, opponent_nick,
                            player_turn, opponent_turn, game_over]

                # If server sent a message about GAME_OVER process it and update the GUI accordingly
                elif data[0] == "GAME_OVER" and data[1] == "OK":
                    if data[2] == "WIN":
                        game_over = "You won!"
                    elif data[2] == "LOSS":
                        game_over = "You lost!"
                    else:
                        game_over = "Draw!"
                    args = [player_dice, opponent_dice, player_score, opponent_score, nickname, opponent_nick,
                            player_turn, opponent_turn, game_over]

    # Process GUI Input
    process_gui_input(gui_input, client_socket)
    # Update the GUI
    current_scene(args)
