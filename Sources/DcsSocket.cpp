// Copyright 2020 Charles Tytler

#pragma comment(lib, "Ws2_32.lib")

#include "pch.h"

#include "DcsSocket.h"

#include <iostream> //TODO: delete after testing
#include <WS2tcpip.h>

constexpr int MAX_SIZE = 1024; // Maximum UDP buffer size to read.

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

void DcsSocket::DcsReceive(const char *delimiter, const char *header_delimiter, std::queue<std::string> &tokens)
{
    // Sender address - dummy variable as it is unused outside recvfrom.
    sockaddr_in dummy_sender_addr;
    int dummy_sender_addr_size = sizeof(dummy_sender_addr);

    // Receive next UDP message.
    char msg[MAX_SIZE] = {};
    recvfrom(socket_id_, msg, MAX_SIZE, 0, (SOCKADDR *)&dummy_sender_addr, &dummy_sender_addr_size);

    // First remove header (up to header_delimiter) from received msg.
    char *new_token;
    char *next_token = NULL;
    new_token = strtok_s(msg, header_delimiter, &next_token);

    // Parse message and push to tokens.
    if (new_token != NULL)
    {
        new_token = strtok_s(NULL, delimiter, &next_token);
    }
    while (new_token != NULL)
    {
        tokens.push(new_token);
        new_token = strtok_s(NULL, delimiter, &next_token);
    }
}