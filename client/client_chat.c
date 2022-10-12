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
    int len_addr;
    struct sockaddr_in my_addr;
    int a2read;

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

//    send(client_socket, username, strlen(username), 0);

    for (;;) {
        for (int i = 0; i < 100; i++)
            buf[i] = 0;
        printf("%s: ", username);
        scanf("%s", buf);
        if (!strcmp(buf, "exit"))
            break;
        send(client_socket, buf, strlen(buf), 0);
        ioctl(client_socket, FIONREAD, &a2read);
        if (a2read > 0) {
            recv(client_socket, buf, 100, 0);
            printf("Server: %s\n", buf);
        }
    }

    return 0;
}