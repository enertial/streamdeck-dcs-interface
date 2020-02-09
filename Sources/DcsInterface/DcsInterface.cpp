// Copyright 2020 Charles Tytler

#pragma comment(lib, "Ws2_32.lib")

#include "DcsInterface.h"

#include <iostream>
#include <WS2tcpip.h>

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

int DcsSocket::dcs_receive(char *msg, const size_t max_size)
{
    int bytes_received = recv(socket_id_, msg, (int)max_size, 0);
    return bytes_received;
}
} // namespace DcsInterface

/*
FOR TESTING:

int main()
{
    int rx_port = 1625;
    int tx_port = 26027;
    DcsInterface::DcsSocket dcs_socket(rx_port, tx_port);

    // Read 10 messages from UDP port.
    for (int i = 0; i < 10; i++)
    {
        char recvbuf[1024] = {};
        dcs_socket.dcs_receive(recvbuf, 1024);
        std::cout << recvbuf << std::endl;
    }

    return 0;
}
*/
