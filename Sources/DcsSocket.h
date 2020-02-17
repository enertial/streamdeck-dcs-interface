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

    /**
     * @brief Reads the UDP buffer from DCS and adds received data to the queue of tokens.
     *
     * @param delimiter Character used to separate tokens from received buffer.
     * @param ignore_header_count Number of tokens to ignore at beginnin of each buffer.
     * @param tokens Vector of tokens that received content is pushed to.
     */
    void DcsReceive(const char *delimiter, const char *header_delimiter, std::queue<std::string> &tokens);

private:
    SOCKET socket_id_;             ///< Socket which is binded to the rx port.
    struct sockaddr_in dest_port_; ///< UDP port address which will be transmitted to.
};