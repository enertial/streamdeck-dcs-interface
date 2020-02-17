// Copyright 2020 Charles Tytler

#pragma comment(lib, "Ws2_32.lib")

#include "pch.h"

#include "DcsSocket.h"

#include <WS2tcpip.h>

constexpr int MAX_UDP_MSG_SIZE = 1024; // Maximum UDP buffer size to read.

DcsSocket::DcsSocket(const int rx_port, const int tx_port)
{
    // Initialize Windows Sockets DLL to version 2.2.
    WSADATA wsaData;
    const auto err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0)
    {
        const std::string error_msg = "Could not startup Windows socket library -- WSA Error: " + std::to_string(WSAGetLastError());
        throw std::runtime_error(error_msg);
    }

    // Define local receive port.
    sockaddr_in local_port;
    memset(&local_port, 0, sizeof(local_port));
    local_port.sin_family = AF_INET;
    local_port.sin_addr.s_addr = INADDR_ANY;
    local_port.sin_port = htons(rx_port);

    // Bind local socket to receive port.
    socket_id_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    const auto result = bind(socket_id_, reinterpret_cast<const sockaddr *>(&local_port), sizeof(local_port));

    if (result == SOCKET_ERROR)
    {
        const std::string error_msg = "Could not bind UDP address to socket -- WSA Error: " + std::to_string(WSAGetLastError());
        closesocket(socket_id_);
        WSACleanup();
        throw std::runtime_error(error_msg);
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

void DcsSocket::DcsReceive(const char *token_delimiter, const char *header_delimiter, std::queue<std::string> &tokens)
{
    // Sender address - dummy variable as it is unused outside recvfrom.
    sockaddr_in sender_addr;
    int sender_addr_size = sizeof(sender_addr);

    // Receive next UDP message.
    char msg[MAX_UDP_MSG_SIZE] = {};
    (void)recvfrom(socket_id_, msg, MAX_UDP_MSG_SIZE, 0, (SOCKADDR *)&sender_addr, &sender_addr_size);

    // First remove header (up to header_delimiter) from received msg.
    char *extracted_token;
    char *dummy_token_storage = NULL;
    extracted_token = strtok_s(msg, header_delimiter, &dummy_token_storage);

    if (extracted_token != NULL)
    {
        // Parse message and push to tokens.
        extracted_token = strtok_s(NULL, token_delimiter, &dummy_token_storage);
        while (extracted_token != NULL)
        {
            tokens.push(extracted_token);
            extracted_token = strtok_s(NULL, token_delimiter, &dummy_token_storage);
        }
    }
}