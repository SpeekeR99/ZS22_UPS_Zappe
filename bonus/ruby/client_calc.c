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

void add_operators(char **buffer, int a, int b) {
    char a_str[10];
    char b_str[10];
    sprintf(a_str, "%d", a);
    sprintf(b_str, "%d", b);
    strcat(*buffer, a_str);
    strcat(*buffer, "|");
    strcat(*buffer, b_str);
    strcat(*buffer, "\n");
}

int add(char **buffer, int a, int b) {
    strcpy(*buffer, "plus|");
    add_operators(buffer, a, b);
    return a + b;
}

int sub(char **buffer, int a, int b) {
    strcpy(*buffer, "minus|");
    add_operators(buffer, a, b);
    return a - b;
}

int mul(char **buffer, int a, int b) {
    strcpy(*buffer, "multiply|");
    add_operators(buffer, a, b);
    return a * b;
}

int divide(char **buffer, int a, int b) {
    strcpy(*buffer, "division|");
    add_operators(buffer, a, b);
    return a / b;
}

int main(int argc, char *argv[]) {
    int client_socket, len_addr, a2read, operand1, operand2, result_local, result_server, should_be_ok = 0;
    char *buffer, *token, operator, known_operators[4] = {'+', '-', '*', '/'};
    struct sockaddr_in my_addr, remote_addr;
    fd_set main_sock;
    int (*operations[4])(char **, int, int) = {add, sub, mul, divide};
    buffer = malloc(BUFFER_LEN);

    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&my_addr, 0, sizeof(struct sockaddr_in));
    memset(&remote_addr, 0, sizeof(struct sockaddr_in));

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(0);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(client_socket, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in));

    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(2000);
    remote_addr.sin_addr.s_addr = inet_addr("35.184.178.82");

    if (!connect(client_socket, (struct sockaddr *) &remote_addr, sizeof(struct sockaddr_in)))
        printf("Uspesne pripojeno k serveru kalkulacka\n");
    else {
        printf("Pripojeni k serveru kalkulacka se nezdarilo\n");
        return EXIT_FAILURE;
    }

    FD_ZERO(&main_sock);
    FD_SET(client_socket, &main_sock);

    recv(client_socket, buffer, BUFFER_LEN, 0); // throw away first two messages from the server
    recv(client_socket, buffer, BUFFER_LEN, 0);
    for (int i = 0; i < BUFFER_LEN; i++)
        buffer[i] = 0;

    printf("Zadejte priklad jako: <operand> <operator> <operand> \n");
    printf("(<operand> = integer | <operator> = + - * / )\n");
    scanf("%d %c %d", &operand1, &operator, &operand2);

    printf("Zadano: %d %c %d\n", operand1, operator, operand2);

    for (int i = 0; i < 4; i++) {
        if (operator == known_operators[i]) {
            result_local = operations[i](&buffer, operand1, operand2);
            should_be_ok = 1;
            break;
        }
    }

    send(client_socket, buffer, strlen(buffer), 0);

    for (int i = 0; i < BUFFER_LEN; i++)
        buffer[i] = 0;

    recv(client_socket, buffer, BUFFER_LEN, 0);

    if (should_be_ok) {
        token = strtok(buffer, "|");
        if (!strcmp(token, "OK")) {
            token = strtok(NULL, "|");
            result_server = atoi(token);
            if (result_local == result_server)
                printf("Vysledek ze serveru je spravny: %d\n", result_server);
            else
                printf("Vysledek ze serveru je spatny: %d (server) vs %d (local)\n", result_server, result_local);
        }
        else
            printf("Vysledek by mel byt OK, ale neni\n");
    }
    else {
        token = strtok(buffer, "|");
        if (!strcmp(token, "ERROR"))
            printf("Vysledek ze serveru je spravny: ERROR\n");
        else
            printf("Vysledek by mel byt ERROR, ale neni\n");
    }

    close(client_socket);
    free(buffer);

    return EXIT_SUCCESS;
}