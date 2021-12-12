// Copyright 2021 Charles Tytler, modified from "ExportStreamParser":
/*
    Copyright 2015 Craig Courtney
    This file is part of DcsBios-Firmware.
    DcsBios-Firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    DcsBios-Firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with DcsBios-Firmware.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <unordered_map>

class DcsBiosStreamParser
{
  public:
    DcsBiosStreamParser();

    /**
     * @brief Processes a single byte from the export stream at a time, populating data by address.
     * @param Single byte of DCS BIOS export stream.
     * @param Map to populate data by address when full address contents are received.
     */
    void processByte(uint8_t c, std::unordered_map<unsigned int, unsigned int> &data_by_address);

  private:
    // State machine states for parsing protocol.
    enum class DcsBiosState { WAIT_FOR_SYNC, ADDRESS_LOW, ADDRESS_HIGH, COUNT_LOW, COUNT_HIGH, DATA_LOW, DATA_HIGH };
    DcsBiosState _state;
    unsigned int _address;
    unsigned int _count;
    unsigned int _data;
    unsigned char _sync_byte_count;
};
