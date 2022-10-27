import sys
import socket
import select

if len(sys.argv) != 4:
    print("Pouziti: python {} <ip> <port> <username>".format(sys.argv[0]))
    sys.exit()
print("Napiste \"help\" pro napovedu")
print("Napiste \"exit\" pro ukonceni chatu")

host = sys.argv[1]
port = int(sys.argv[2])

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((host, port))
print("Uspesne pripojeno k serveru")

socket_fd = client_socket.fileno()
input_fd = sys.stdin.fileno()

read_fds = [socket_fd, input_fd]
write_fds = []
exception_fds = []

client_socket.send("USERNAME|{}".format(sys.argv[3]).encode())

while True:
    readReady, writeReady, exceptionReady = select.select(read_fds, write_fds, exception_fds)

    for fd in readReady:
        if fd is socket_fd:
            data = client_socket.recv(1024)
            if len(data) == 0:
                print("Server je nedostupny")
                print("Ukoncuji chat")
                client_socket.close()
                sys.exit()
            print(data.decode())
            break
        if fd is input_fd:
            data = input()
            if data == "exit":
                print("Ukoncuji chat")
                client_socket.close()
                sys.exit()
            elif data == "help":
                print("Dostupne prikazy:")
                print("\"help\" - zobrazi napovedu")
                print("\"LIST\" - zobrazi seznam pripojenych uzivatelu")
                print("\"MSG|<zprava>\" - posle zpravu vsem pripojenym uzivatelum")
                print("\"exit\" - ukonci chat")
            client_socket.send(data.encode())
            break
