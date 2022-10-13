#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

#define BUFFER_LEN 1024

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Pouzit: %s <ip> <port> <username>\n", argv[0]);
        return EXIT_FAILURE;
    }
    printf("Napiste \"help\" pro napovedu\n");
    printf("Napiste \"exit\" pro ukonceni chatu\n");

    int client_socket, len_addr, a2read;
    char buffer[BUFFER_LEN], *username = argv[3];
    struct sockaddr_in my_addr;
    fd_set main_sock, read_fds;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&my_addr, 0, sizeof(struct sockaddr_in));

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(atoi(argv[2]));
    my_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if (!connect(client_socket, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in)))
        printf("Uspesne pripojeno k serveru\n");
    else {
        printf("Pripojeni k serveru se nezdarilo\n");
        return EXIT_FAILURE;
    }

    FD_ZERO(&main_sock);
    FD_SET(client_socket, &main_sock);
    FD_SET(STDIN_FILENO, &main_sock);

    char msg[BUFFER_LEN] = "USERNAME|";
    strcat(msg, username);
    send(client_socket, msg, strlen(msg), 0);

    for (;;) {
        read_fds = main_sock;
        for (int i = 0; i < BUFFER_LEN; i++)
            buffer[i] = 0;

        if (select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) < 0) {
            printf("Select ERR\n");
            return EXIT_FAILURE;
        }
        if (FD_ISSET(client_socket, &read_fds)) {
            ioctl(client_socket, FIONREAD, &a2read);
            if (a2read == 0) {
                printf("Server je nedostupny\n");
                break;
            }
            recv(client_socket, buffer, a2read, 0);
            printf("%s", buffer);
        }
        else if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            read(STDIN_FILENO, buffer, BUFFER_LEN);
            if (!strcmp(buffer, "exit\n")) {
                break;
            }
            if (!strcmp(buffer, "help\n")) {
                printf("Dostupne prikazy:\n");
                printf("\"help\" - zobrazi napovedu\n");
                printf("\"LIST\" - zobrazi seznam pripojenych uzivatelu\n");
                printf("\"MSG|<zprava>\" - posle zpravu vsem pripojenym uzivatelum\n");
                printf("\"exit\" - ukonci chat\n");
                continue;
            }
            send(client_socket, buffer, strlen(buffer), 0);
        }
    }
    printf("Ukoncuji chat\n");
    return EXIT_SUCCESS;
}