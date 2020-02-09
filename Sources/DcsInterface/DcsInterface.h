// Copyright 2020 Charles Tytler

#pragma once

#include <string>
#include <winsock2.h>
namespace DcsInterface
{

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
     * @brief Reads the UDP buffer.
     * 
     * @param buffer      Character buffer to read into.
     * @param buffer_size Maximum size of character buffer.
     * @return int        Number of bytes read into buffer.
     */
    int dcs_receive(char *buffer, const size_t buffer_size);

private:
    SOCKET socket_id_;             ///< Socket which is binded to the rx port.
    struct sockaddr_in dest_port_; ///< UDP port address which will be transmitted to.
};

} // namespace DcsInterface