#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_LEN 1024

int main(int argc, char *argv[]) {
    int client_socket, len_addr, a2read;
    char buffer_send[BUFFER_LEN], buffer_recv[BUFFER_LEN];
    struct sockaddr_in my_addr, remote_addr;
    fd_set main_sock;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&my_addr, 0, sizeof(struct sockaddr_in));
    memset(&remote_addr, 0, sizeof(struct sockaddr_in));

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(0);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(client_socket, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in));

    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(2001);
    remote_addr.sin_addr.s_addr = inet_addr("35.184.178.82");

    if (!connect(client_socket, (struct sockaddr *) &remote_addr, sizeof(struct sockaddr_in)))
        printf("Uspesne pripojeno k serveru echo\n");
    else {
        printf("Pripojeni k serveru echo se nezdarilo\n");
        return EXIT_FAILURE;
    }

    FD_ZERO(&main_sock);
    FD_SET(client_socket, &main_sock);

    recv(client_socket, buffer_recv, BUFFER_LEN, 0); // throw away first message from the server
    for (int i = 0; i < BUFFER_LEN; i++)
        buffer_recv[i] = 0;

    printf("Zadejte cokoliv, server zopakuje:\n");
    fgets(buffer_send, BUFFER_LEN, stdin);

    send(client_socket, buffer_send, BUFFER_LEN, 0);
    printf("Poslano: %s", buffer_send);
    recv(client_socket, buffer_recv, BUFFER_LEN, 0);
    printf("Prijato: %s", buffer_recv);

    if (!strcmp(buffer_send, buffer_recv))
        printf("OK, shoduji se\n");
    else
        printf("ERROR, neshoduji se\n");

    close(client_socket);

    return EXIT_SUCCESS;
}