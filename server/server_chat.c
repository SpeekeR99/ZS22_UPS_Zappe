#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

int main(int argc, char *argv[]) {
    int server_socket;
    int client_socket, fd;
    int return_value;
    char buf[256];
    int len_addr;
    int a2read;
    struct sockaddr_in my_addr, peer_addr;
    fd_set client_socks, tests;
    char usernames[100][100];

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    int param = 1;
    return_value = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char *) &param, sizeof(int));

    if (return_value == -1)
        printf("setsockopt ERR\n");

    memset(&my_addr, 0, sizeof(struct sockaddr_in));

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(10000);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    return_value = bind(server_socket, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in));
    if (!return_value)
        printf("Bind - OK\n");
    else {
        printf("Bind - ERR\n");
        return -1;
    }

    return_value = listen(server_socket, 5);
    if (!return_value) {
        printf("Listen - OK\n");
    } else {
        printf("Listen - ER\n");
    }

    FD_ZERO(&client_socks);
    FD_SET(server_socket, &client_socks);

    for (;;) {
        tests = client_socks;
        for (int i = 0; i < 100; i++)
            buf[i] = 0;

        return_value = select(FD_SETSIZE, &tests, NULL, NULL, NULL);
        if (return_value < 0) {
            printf("Select ERR\n");
            return -1;
        }

        for (fd = 3; fd < FD_SETSIZE; fd++) {
            if (FD_ISSET(fd, &tests)) {
                if (fd == server_socket) {
                    client_socket = accept(server_socket, (struct sockaddr *) &peer_addr, &len_addr);
                    FD_SET(client_socket, &client_socks);
                    printf("Pripojen novy klient a pridan do sady socketu\n");
                } else {
                    ioctl(fd, FIONREAD, &a2read);
                    if (a2read > 0) {
                        return_value = recv(fd, buf, sizeof(buf), 0);
                        printf("Prijato od %d: %s\n", fd, buf);
                        for (int i = 3; i < FD_SETSIZE; i++) {
                            if (FD_ISSET(i, &client_socks)) {
                                if (i != server_socket && i != fd) {
                                    printf("Odeslano klientovi %d: %s\n", i, buf);
                                    send(i, buf, return_value, 0);
                                }
                            }
                        }
//                        send(fd, buf, sizeof(buf), 0);
//                        printf("%s: ", usernames[fd - 3]);
//                        recv(fd, &buf, a2read, 0);
//                        printf("%s\n",buf);
                    } else {
                        close(fd);
                        FD_CLR(fd, &client_socks);
                        printf("Klient se odpojil a byl odebran ze sady socketu\n");
                    }
                }
            }
        }
    }

    return 0;
}