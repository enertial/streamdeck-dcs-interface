// Copyright 2020 Charles Tytler

#pragma once

#include <sstream>
#include <winsock2.h>

class DcsSocket {
  public:
    // Binds a UDP socket to the rx port and also initializes the destination address using the tx port.
    /**
     * @brief Construct a new Dcs Socket object bound to the rx port and initializes the destination address using the
     * tx port if provided, or determines tx_port address dynamically if not.
     *
     * @param tx_ip_address UDP transmit IP address.
     * @param rx_port UDP receive port.
     * @param tx_port UDP transmit port, defaults to dynamic (use recvfrom address) if not provided.
     */
    DcsSocket(const std::string &ip_address, const std::string &rx_port, const std::string &tx_port = "dynamic");

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
     * @return String stream of received message from DCS.
     */
    std::stringstream DcsReceive();

    /**
     * @brief Sends a UDP message to the destination port.
     *
     * @param message String to send.
     */
    void DcsSend(const std::string &message);

  private:
    SOCKET socket_id_;      // Socket which is binded to the rx port.
    sockaddr dest_addr_;    // UDP address info for port which will be transmitted to.
    int dest_addr_len_ = 0; // Size of dest address.
};