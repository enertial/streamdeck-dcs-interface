// Copyright 2020 Charles Tytler

#pragma once

#include <sstream>
#include <winsock2.h>

class UdpSocket
{
  public:
    // Binds a UDP socket to the rx port and also initializes the destination address using the tx port.
    /**
     * @brief Construct a new Dcs Socket object bound to the rx port and initializes the destination address using the
     * tx port if provided, or determines tx_port address dynamically if not.
     *
     * @param ip_address UDP receive/transmit IP address.
     * @param rx_port UDP receive port.
     * @param tx_port UDP transmit port, defaults to dynamic (use recvfrom address) if not provided.
     * @param multicast_addr Address to join multicast group on, not set if no address provided.
     */
    UdpSocket(const std::string &ip_address,
              const std::string &rx_port,
              const std::string &tx_port = "dynamic",
              const std::string &multicast_addr = "");

    /**
     * @brief Destroy the Dcs Socket object
     *
     */
    ~UdpSocket();

    // Disable copy and move constructors.
    UdpSocket(const UdpSocket &) = delete;
    UdpSocket(UdpSocket &&) = delete;
    UdpSocket &operator=(const UdpSocket &) = delete;
    UdpSocket &operator=(UdpSocket &&) = delete;

    /**
     * @brief Reads the UDP buffer populating the data into the provided byte buffer.
     * @return Byte buffer of received data.
     */
    int receive_bytes(char *buffer, const int buffer_size);

    /**
     * @brief Reads the UDP buffer returning data in a string stream.
     * @return String stream of received messages.
     */
    std::stringstream receive_stream();

    /**
     * @brief Sends a UDP message of the provided byte buffer to the destination port.
     * @return Number of bytes sent.
     */
    int send_bytes(const char *byte_buffer, const int buffer_size);

    /**
     * @brief Sends a UDP message to the destination port.
     * @return Number of bytes sent.
     */
    int send_string(const std::string &message);

  private:
    SOCKET socket_id_;      // Socket which is binded to the rx port.
    sockaddr dest_addr_;    // UDP address info for port which will be transmitted to.
    int dest_addr_len_ = 0; // Size of dest address.
};
