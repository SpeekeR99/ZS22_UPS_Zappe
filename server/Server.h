#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <vector>

constexpr size_t BUFFER_LEN = 1024;

class Server {
private:
    int server_socket;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    fd_set client_socks{};
    fd_set read_fds{};
    char char_buffer[BUFFER_LEN];
    std::string string_buffer;

public:
    explicit Server(int port);
    ~Server();

    void run();
};
