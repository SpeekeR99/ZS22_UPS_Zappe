import socket
import select
import time
from gui.gui_scenes import *

LOG_FILE = "client.log"


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


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python {} <ip> <port> <nickname>".format(sys.argv[0]))
        sys.exit(1)

    ip = sys.argv[1]
    port = int(sys.argv[2])
    nickname = sys.argv[3]

    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((ip, port))
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
        from gui.gui_scenes import current_scene
        readReady, writeReady, exceptionReady = select.select(read_fds, write_fds, exception_fds, 0.01)

        for fd in readReady:
            if fd is socket_fd:
                data = client_socket.recv(1024)
                if len(data) == 0:
                    print("Server je nedostupny")
                    time.sleep(1)
                print(data.decode())
                break

        current_scene()
