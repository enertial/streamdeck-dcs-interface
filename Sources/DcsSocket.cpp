// Copyright 2020 Charles Tytler

#pragma comment(lib, "Ws2_32.lib")

#include "pch.h"

#include "DcsSocket.h"

#include <WS2tcpip.h>

DcsSocket::DcsSocket(const std::string &rx_port, const std::string &tx_port, const std::string &ip_address)
{
    // Initialize Windows Sockets DLL to version 2.2.
    WSADATA wsaData;
    const auto err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0)
    {
        const std::string error_msg = "Could not startup Windows socket library -- WSA Error: " + std::to_string(WSAGetLastError());
        throw std::runtime_error(error_msg);
    }

    // Define socket address info settings for UDP protocol.
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    // Define local receive port.
    addrinfo *local_port;
    getaddrinfo(ip_address.c_str(), rx_port.c_str(), &hints, &local_port);

    // Bind local socket to receive port.
    socket_id_ = socket(local_port->ai_family, local_port->ai_socktype, local_port->ai_protocol);
    const auto result = bind(socket_id_, local_port->ai_addr, static_cast<int>(local_port->ai_addrlen));
    freeaddrinfo(local_port);

    if (result == SOCKET_ERROR)
    {
        const std::string error_msg = "Could not bind UDP address to socket -- WSA Error: " + std::to_string(WSAGetLastError());
        closesocket(socket_id_);
        WSACleanup();
        throw std::runtime_error(error_msg);
    }

    // Define send destination port.
    getaddrinfo(ip_address.c_str(), tx_port.c_str(), &hints, &dest_port_);
}

DcsSocket::~DcsSocket()
{
    // Delete opened socket.
    freeaddrinfo(dest_port_);
    closesocket(socket_id_);
    WSACleanup();
}

std::stringstream DcsSocket::DcsReceive()
{
    // Sender address - dummy variable as it is unused outside recvfrom.
    sockaddr_in sender_addr;
    int sender_addr_size = sizeof(sender_addr);

    // Receive next UDP message.
    constexpr int MAX_UDP_MSG_SIZE = 1024; // Maximum UDP buffer size to read.
    char msg[MAX_UDP_MSG_SIZE] = {};
    (void)recvfrom(socket_id_, msg, MAX_UDP_MSG_SIZE, 0, (SOCKADDR *)&sender_addr, &sender_addr_size);

    std::stringstream ss;
    ss << msg;
    return ss;
}

void DcsSocket::DcsSend(const std::string &message)
{
    (void)sendto(socket_id_,
                 message.c_str(),
                 static_cast<int>(message.length()),
                 0,
                 dest_port_->ai_addr,
                 static_cast<int>(dest_port_->ai_addrlen));
}
