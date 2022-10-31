#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

constexpr size_t BUFFER_LEN = 1024;

class Server {
private:
    int server_socket;
    char char_buffer[BUFFER_LEN]{};

public:
    explicit Server(int port);
    ~Server();

    void run();
};
