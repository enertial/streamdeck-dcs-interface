// Copyright 2020 Charles Tytler

#pragma comment(lib, "Ws2_32.lib")

#include "DcsInterface.h"

#include <iostream>
#include <WS2tcpip.h>

constexpr int MAX_SIZE = 1024; // Maximum UDP buffer size to read.
namespace DcsInterface
{

DcsSocket::DcsSocket(const int rx_port, const int tx_port)
{
    // Initialize Windows Sockets DLL to version 2.2.
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        fprintf(stderr, "Could not startup Windows socket library -- WSA Error: %d\n", WSAGetLastError());
    }

    // Define local receive port.
    sockaddr_in local_port;
    memset(&local_port, 0, sizeof(local_port));
    local_port.sin_family = AF_INET;
    local_port.sin_addr.s_addr = INADDR_ANY;
    local_port.sin_port = htons(rx_port);

    // Bind local socket to receive port.
    socket_id_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (bind(socket_id_, (const struct sockaddr *)&local_port, sizeof(local_port)) == SOCKET_ERROR)
    {
        fprintf(stderr, "Could not bind UDP address to socket -- WSA Error: %d\n", WSAGetLastError());
        closesocket(socket_id_);
        WSACleanup();
    }

    // Define destination port for transmit.
    memset(&dest_port_, 0, sizeof(dest_port_));
    dest_port_.sin_family = AF_INET;
    dest_port_.sin_addr.s_addr = INADDR_ANY;
    dest_port_.sin_port = htons(tx_port);
}

DcsSocket::~DcsSocket()
{
    // Delete opened socket.
    closesocket(socket_id_);
    WSACleanup();
}

int DcsSocket::dcs_receive(const char *delimiter, const char *header_delimiter, std::queue<std::string> &tokens)
{
    // Sender address - dummy variable as it is unused outside recvfrom.
    sockaddr_in dummy_sender_addr;
    int dummy_sender_addr_size = sizeof(dummy_sender_addr);

    int bytes_received;
    char msg[MAX_SIZE] = {};
    bytes_received = recvfrom(socket_id_, msg, MAX_SIZE, 0, (SOCKADDR *)&dummy_sender_addr, &dummy_sender_addr_size);
    std::cout << "MSG: " << msg << std::endl;
    char *new_token;
    char *next_token = NULL;
    new_token = strtok_s(msg, header_delimiter, &next_token);
    int count = 0;
    if (new_token != NULL)
    {
        new_token = strtok_s(NULL, delimiter, &next_token);
    }
    while (new_token != NULL)
    {
        tokens.push(new_token);
        //std::cout << "Token: " << new_token << std::endl;
        new_token = strtok_s(NULL, delimiter, &next_token);
    }
    return 0;
}
} // namespace DcsInterface

/*
FOR TESTING:
*/
int main()
{
    int rx_port = 1625;
    int tx_port = 26027;
    DcsInterface::DcsSocket dcs_socket(rx_port, tx_port);

    std::cout << "start" << std::endl;
    // Read 10 messages from UDP port.
    std::queue<std::string> recvd_msgs;
    for (int i = 0; i < 50; i++)
    {
        dcs_socket.dcs_receive(":", "*", recvd_msgs);
        while (!recvd_msgs.empty())
        {
            std::cout << recvd_msgs.front() << std::endl;
            recvd_msgs.pop();
        }
    }

    return 0;
}
