#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

int main(int argc, char *argv[]) {
    char *username;
    int client_socket;
    int return_value;
    char buf[100];
    int filled = 0;
    int len_addr;
    struct sockaddr_in my_addr;
    int a2read;
    fd_set main_sock, tests_in, tests_out;

    if (argc != 2)
        printf("Usage: %s <username>", argv[0]);
    username = argv[1];

    printf("Napiste \"exit\" pro ukonceni chatu\n");

    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&my_addr, 0, sizeof(struct sockaddr_in));

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(10000);
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    return_value = connect(client_socket, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in));
    if (!return_value)
        printf("Uspesne pripojeno k serveru\n");
    else {
        printf("Pripojeni k serveru se nezdarilo\n");
        return -1;
    }

    FD_ZERO(&main_sock);
    FD_SET(client_socket, &main_sock);
    FD_SET(STDIN_FILENO, &main_sock);

    char msg[100] = "USERNAME|";
    strcat(msg, username);
    send(client_socket, msg, strlen(msg), 0);

    for (;;) {
        tests_in = main_sock;
        for (int i = 0; i < 100; i++)
            buf[i] = 0;

        return_value = select(FD_SETSIZE, &tests_in, NULL, NULL, NULL);
        if (return_value < 0) {
            printf("Select ERR\n");
            return -1;
        }
        if (FD_ISSET(client_socket, &tests_in)) {
            ioctl(client_socket, FIONREAD, &a2read);
            if (a2read == 0) {
                printf("Server je nedostupny\n");
                break;
            }
            recv(client_socket, buf, a2read, 0);
            printf("%s", buf);
        }
        else if (FD_ISSET(STDIN_FILENO, &tests_in)) {
            read(STDIN_FILENO, buf, 100);
            if (strcmp(buf, "exit\n") == 0) {
                break;
            }
            send(client_socket, buf, strlen(buf), 0);
        }
    }
    printf("Ukoncuji chat\n");
    return 0;
}