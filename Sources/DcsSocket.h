// Copyright 2020 Charles Tytler

#pragma once

#include <string>
#include <queue>
#include <winsock2.h>

class DcsSocket
{
public:
    // Binds a UDP socket to the rx port and also intializes the destination address using the tx port.
    /**
     * @brief Construct a new Dcs Socket object bound to the rx port and initializes the destination address using the tx port.
     *
     * @param rx_port UDP receive port.
     * @param tx_port UDP transmit port.
     */
    DcsSocket(const int rx_port, const int tx_port);

    /**
     * @brief Destroy the Dcs Socket object
     *
     */
    ~DcsSocket();

    // Disable copy and move constructors.
    DcsSocket(const DcsSocket &) = delete;
    DcsSocket(DcsSocket &&) = delete;
    DcsSocket &operator=(const DcsSocket &) = delete;
    DcsSocket &operator=(DcsSocket &&) = delete;

    /**
     * @brief Reads the UDP buffer from DCS and adds received data to the queue of tokens.
     *
     * @param token_delimiter Character used to separate tokens from received buffer.
     * @param header_delimiter Character used to separate header from message contents in buffer.
     * @return Vector of string tokens parsed from received message.
     */
    std::vector<std::string> DcsReceive(const char *token_delimiter, const char *header_delimiter);

private:
    SOCKET socket_id_;      // Socket which is binded to the rx port.
    sockaddr_in dest_port_; // UDP port address which will be transmitted to.
};