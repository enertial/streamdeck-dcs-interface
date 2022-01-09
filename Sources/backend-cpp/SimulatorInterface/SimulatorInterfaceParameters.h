// Copyright 2020 Charles Tytler

#pragma once

// Default parameters set for Dcs-ExportScript backend defaults.
const std::string DEFAULT_LISTENER_PORT = "5010";   // Port number to receive simulator updates from.
const std::string DEFAULT_SEND_PORT = "7778";       // Port number which simulator commands will be sent to.
const std::string DEFAULT_IP_ADDRESS = "127.0.0.1"; // IP Address to communicate with simulator -- Default LocalHost.
const std::string DEFAULT_MULTICAST_ADDRESS = "239.255.50.10"; // Multicast Address group to join -- Empty will disable.
