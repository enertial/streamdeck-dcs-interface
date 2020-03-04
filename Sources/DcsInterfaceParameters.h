// Copyright 2020 Charles Tytler

#pragma once

// Parameters used for DCS interface.
const std::string kDcsListenerPort = "1625";       // Port number to receive DCS updates from.
const std::string kDcsSendPort = "26027";          // Port number which DCS commands will be sent to.
const std::string kDcsSendIpAddress = "127.0.0.1"; // IP Address on which to send DCS commands -- Default LocalHost.
