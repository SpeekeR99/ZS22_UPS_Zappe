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
    std::string buffer;

    void clear_char_buffer();
    void read_message(int fd);
    void send_message(int fd, const std::string &message);
    void recv_message(int fd, int a2read);

public:
    explicit Server(int port);
    ~Server();

    void run();
};
