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

void shutdown_server(char ***usernames) {
    for (int i = 0; i < FD_SETSIZE; i++) {
        if ((*usernames)[i] != NULL) {
            free((*usernames)[i]);
            (*usernames)[i] = NULL;
        }
    }
    free(*usernames);
    printf("Ukoncuji server\n");
    exit(0);
}

void set_username(int fd, char *buffer, fd_set *client_socks, int server_socket, char ***usernames) {
    char *username = strtok(buffer, "|");
    username = strtok(NULL, "|");
    (*usernames)[fd] = (char *) malloc(strlen(username) + 1);
    strcpy((*usernames)[fd], username);
    printf("Uzivatel %s se pripojil/a\n", username);
    for (int i = 3; i < FD_SETSIZE; i++) {
        if (FD_ISSET(i, client_socks)) {
            if (i != server_socket && i != fd) {
                char *msg = " se pripojil/a\n";
                char *actual_msg = malloc(strlen(msg) + strlen(username) + 1);
                strcpy(actual_msg, username);
                strcat(actual_msg, msg);
                send(i, actual_msg, strlen(actual_msg), 0);
                free(actual_msg);
            }
        }
    }
}

void list_connected_users(int fd, char *buffer, fd_set *client_socks, int server_socket, char ***usernames) {
    char *msg = "Seznam pripojenych uzivatelu:\n";
    send(fd, msg, strlen(msg), 0);
    for (int i = 3; i < FD_SETSIZE; i++) {
        if (FD_ISSET(i, client_socks)) {
            if (i != server_socket) {
                char *actual_msg = malloc(strlen((*usernames)[i]) + 3);
                strcpy(actual_msg, (*usernames)[i]);
                strcat(actual_msg, "\n");
                send(fd, actual_msg, strlen(actual_msg), 0);
                free(actual_msg);
            }
        }
    }
}

void broadcast_message(int fd, char *buffer, fd_set *client_socks, int server_socket, char ***usernames) {
    char *msg = strtok(buffer, "|");
    msg = strtok(NULL, "|");
    for (int i = 3; i < FD_SETSIZE; i++) {
        if (FD_ISSET(i, client_socks)) {
            if (i != server_socket && i != fd) {
                char *actual_msg = malloc(strlen(msg) + strlen((*usernames)[fd]) + 3);
                strcpy(actual_msg, (*usernames)[fd]);
                strcat(actual_msg, ": ");
                strcat(actual_msg, msg);
                strcat(actual_msg, "\n");
                printf("Odeslano uzivateli %s: %s\n", (*usernames)[i], actual_msg);
                send(i, actual_msg, strlen(actual_msg), 0);
                free(actual_msg);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Pouziti: %s <port>\n", argv[0]);
        return 1;
    }

    int server_socket, client_socket, fd, len_addr, a2read;
    char buffer[BUFFER_LEN], **usernames = calloc(FD_SETSIZE, sizeof(char *));
    struct sockaddr_in my_addr, peer_addr;
    fd_set client_socks, readfds;

    char *known_server_commands[] = {
            "exit\n"
    };
    void (*server_cmd_functions[])(char ***) = {
            shutdown_server
    };
    char *known_client_commands[] = {
            "USERNAME",
            "LIST",
            "MSG"
    };
    void (*client_cmd_functions[])(int, char *, fd_set *, int, char ***) = {
            set_username,
            list_connected_users,
            broadcast_message,
    };

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    int param = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char *) &param, sizeof(int)) == -1) {
        printf("setsockopt ERR\n");
        return EXIT_FAILURE;
    }

    memset(&my_addr, 0, sizeof(struct sockaddr_in));

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(atoi(argv[1]));
    my_addr.sin_addr.s_addr = INADDR_ANY;

    if (!bind(server_socket, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in))) printf("bind - OK\n");
    else {
        printf("bind - ERR\n");
        return EXIT_FAILURE;
    }

    if (!listen(server_socket, 5)) printf("listen - OK\n");
    else printf("listen - ER\n");

    FD_ZERO(&client_socks);
    FD_SET(server_socket, &client_socks);
    FD_SET(STDIN_FILENO, &client_socks);

    for (;;) {
        readfds = client_socks;
        for (int i = 0; i < BUFFER_LEN; i++)
            buffer[i] = 0;

        if (select(FD_SETSIZE, &readfds, NULL, NULL, NULL) < 0) {
            printf("Select ERR\n");
            return EXIT_FAILURE;
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            read(STDIN_FILENO, buffer, BUFFER_LEN);
            for (int i = 0; i < sizeof(known_server_commands) / sizeof(char *); i++) {
                if (!strcmp(buffer, known_server_commands[i])) {
                    server_cmd_functions[i](&usernames);
                    break;
                }
            }
        }
        else {
            for (fd = 3; fd < FD_SETSIZE; fd++) {
                if (FD_ISSET(fd, &readfds)) {
                    if (fd == server_socket) {
                        client_socket = accept(server_socket, (struct sockaddr *) &peer_addr, &len_addr);
                        FD_SET(client_socket, &client_socks);
                        printf("Pripojen novy klient: %d\n", client_socket);
                    } else {
                        ioctl(fd, FIONREAD, &a2read);
                        if (a2read > 0) {
                            recv(fd, buffer, a2read, 0);
                            printf("Prijato od uzivatele %d: %s\n", fd, buffer);
                            for (int i = 0; i < sizeof(known_client_commands) / sizeof(char *); i++) {
                                if (strstr(buffer, known_client_commands[i])) {
                                    client_cmd_functions[i](fd, buffer, &client_socks, server_socket, &usernames);
                                }
                            }
                        } else {
                            for (int i = 3; i < FD_SETSIZE; i++) {
                                if (FD_ISSET(i, &client_socks)) {
                                    if (i != server_socket && i != fd) {
                                        char *msg = " se odpojil/a\n";
                                        char *actual_msg = malloc(strlen(msg) + strlen(usernames[fd]) + 1);
                                        strcpy(actual_msg, usernames[fd]);
                                        strcat(actual_msg, msg);
                                        send(i, actual_msg, strlen(actual_msg), 0);
                                        free(actual_msg);
                                    }
                                }
                            }
                            close(fd);
                            FD_CLR(fd, &client_socks);
                            printf("Uzivatel %s se odpojil/a\n", usernames[fd]);
                        }
                    }
                }
            }
        }
    }

    return 0;
}