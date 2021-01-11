// Copyright 2020 Charles Tytler

#pragma once

// Parameters used for DCS interface.
const std::string kDefaultDcsListenerPort = "1725"; // Port number to receive DCS updates from.
const std::string kDefaultsendPort = "26027";       // Port number which DCS commands will be sent to.
const std::string kDefaultDcsIpAddress =
    "127.0.0.1"; // IP Address on which to communicate with DCS -- Default LocalHost.
