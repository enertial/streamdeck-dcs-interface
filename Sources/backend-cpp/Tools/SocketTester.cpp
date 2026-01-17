// Copyright 2021 Charles Tytler

#include "Utilities/UdpSocket.cpp"
#include <bitset>
#include <csignal>
#include <fstream>
#include <iomanip>
#include <iostream>

/**
 *  This tool is currently set up to connect to DCS-BIOS and print out received data in Hex format.
 */

void signalInteruptHandler(int sig)
{
    std::cout << "Stopped." << std::endl;
    std::exit(sig);
}

int main()
{
    signal(SIGINT, signalInteruptHandler);

    UdpSocket socket("127.0.0.1", "5010", "7778", "239.255.50.10");
    std::ofstream log_file;
    log_file.open("./socket_dump.log");

    std::cout << "Logging data to ./socket_dump.log  |  Press CTRL+C to stop." << std::endl;

    for (;;) {

        constexpr int MAX_UDP_MSG_SIZE = 1024; // Maximum UDP buffer size to read.
        char msg[MAX_UDP_MSG_SIZE] = {0};

        const int num_bytes = socket.receive_bytes(msg, MAX_UDP_MSG_SIZE);

        if (num_bytes != SOCKET_ERROR) {
            log_file << std::dec << "Bytes recv: " << num_bytes << std::endl;
            for (size_t byte = 0; byte < num_bytes; byte++) {
                // Set format to "0x00"
                log_file << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex;
                log_file << static_cast<uint16_t>((unsigned char)(msg[byte])) << " ";
            }
            log_file << std::endl;
        }
    }

    return 0;
}
