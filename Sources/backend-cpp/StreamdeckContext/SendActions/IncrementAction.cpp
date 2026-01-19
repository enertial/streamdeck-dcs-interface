// Copyright 2021 Charles Tytler

#include "IncrementAction.h"

#include "Utilities/StringUtilities.h"

#include "ElgatoSD/EPLJSONUtils.h"

#include <sstream>
#include <cmath>

void IncrementAction::handleButtonPressedEvent(SimulatorInterface *simulator_interface,
                                               ESDConnectionManager *mConnectionManager,
                                               const json &inPayload)
{
    const auto settings = inPayload["settings"];
    const auto send_address = EPLJSONUtils::GetStringByName(settings, "send_address");

    // TODO: simplify increment monitor interface as this is currently the only user.
    increment_monitor_.update_settings(settings);
    increment_monitor_.update(simulator_interface);

    const auto send_command = determineSendValue(settings);
    if (send_command) {
        simulator_interface->send_command(send_address, send_command.value());
    }
}

std::string IncrementAction::getCurrentDisplayValue(SimulatorInterface *simulator_interface, const json &settings)
{
    const auto dcs_id_increment_monitor_str = EPLJSONUtils::GetStringByName(settings, "dcs_id_increment_monitor");
    
    if (is_integer(dcs_id_increment_monitor_str)) {
        const int dcs_id = std::stoi(dcs_id_increment_monitor_str);
        const std::optional<Decimal> maybe_value = simulator_interface->get_value_at_addr(dcs_id);
        
        if (maybe_value.has_value()) {
            const std::string raw_value = maybe_value.value().str();
            
            // Check if there's a value-to-text mapping
            const auto mapping_str = EPLJSONUtils::GetStringByName(settings, "encoder_value_text_mapping");
            
            if (!mapping_str.empty()) {
                // Parse the mapping format: "value1:text1;value2:text2;..."
                std::istringstream mapping_stream(mapping_str);
                std::string pair;
                
                while (std::getline(mapping_stream, pair, ';')) {
                    // Find the colon separator
                    size_t colon_pos = pair.find(':');
                    if (colon_pos != std::string::npos) {
                        std::string map_value = pair.substr(0, colon_pos);
                        std::string map_text = pair.substr(colon_pos + 1);
                        
                        // Remove any formatting remnants (|color:|size:) from old format
                        size_t pipe_pos = map_text.find('|');
                        if (pipe_pos != std::string::npos) {
                            map_text = map_text.substr(0, pipe_pos);
                        }
                        
                        // Trim whitespace
                        map_value.erase(0, map_value.find_first_not_of(" \t\n\r"));
                        map_value.erase(map_value.find_last_not_of(" \t\n\r") + 1);
                        map_text.erase(0, map_text.find_first_not_of(" \t\n\r"));
                        map_text.erase(map_text.find_last_not_of(" \t\n\r") + 1);
                        
                        // Compare values (with tolerance for floating point)
                        if (is_number(map_value)) {
                            Decimal map_decimal(map_value);
                            Decimal current_decimal(raw_value);
                            
                            // Check if values match (within a small tolerance)
                            Decimal diff = (map_decimal > current_decimal) ? (map_decimal - current_decimal) : (current_decimal - map_decimal);
                            if (std::stod(diff.str()) < 0.0001) {
                                return map_text;
                            }
                        }
                    }
                }
            }
            
            // No mapping found, return raw value
            return raw_value;
        }
    }
    
    return "";
}

std::string IncrementAction::getCurrentImagePath(SimulatorInterface *simulator_interface, const json &settings)
{
    const auto dcs_id_increment_monitor_str = EPLJSONUtils::GetStringByName(settings, "dcs_id_increment_monitor");
    
    if (is_integer(dcs_id_increment_monitor_str)) {
        const int dcs_id = std::stoi(dcs_id_increment_monitor_str);
        const std::optional<Decimal> maybe_value = simulator_interface->get_value_at_addr(dcs_id);
        
        if (maybe_value.has_value()) {
            const std::string raw_value = maybe_value.value().str();
            
            // Check if there's a value-to-image mapping
            const auto mapping_str = EPLJSONUtils::GetStringByName(settings, "encoder_value_text_mapping");
            
            if (!mapping_str.empty()) {
                // Parse the mapping format: "value1:text1;value2:IMG:imagepath;..."
                std::istringstream mapping_stream(mapping_str);
                std::string pair;
                
                while (std::getline(mapping_stream, pair, ';')) {
                    // Find the colon separator
                    size_t colon_pos = pair.find(':');
                    if (colon_pos != std::string::npos) {
                        std::string map_value = pair.substr(0, colon_pos);
                        std::string map_content = pair.substr(colon_pos + 1);
                        
                        // Trim whitespace
                        map_value.erase(0, map_value.find_first_not_of(" \t\n\r"));
                        map_value.erase(map_value.find_last_not_of(" \t\n\r") + 1);
                        map_content.erase(0, map_content.find_first_not_of(" \t\n\r"));
                        map_content.erase(map_content.find_last_not_of(" \t\n\r") + 1);
                        
                        // Check if this is an image mapping (format: IMG:path)
                        if (map_content.substr(0, 4) == "IMG:") {
                            std::string image_path = map_content.substr(4);
                            
                            // Compare values (with tolerance for floating point)
                            if (is_number(map_value)) {
                                Decimal map_decimal(map_value);
                                Decimal current_decimal(raw_value);
                                
                                // Check if values match (within a small tolerance)
                                Decimal diff = (map_decimal > current_decimal) ? (map_decimal - current_decimal) : (current_decimal - map_decimal);
                                if (std::stod(diff.str()) < 0.0001) {
                                    return image_path;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    return "";
}

void IncrementAction::handleButtonReleasedEvent(SimulatorInterface *simulator_interface,
                                                ESDConnectionManager *mConnectionManager,
                                                const json &inPayload)
{
    // Nothing sent to DCS on release.
}

std::optional<std::string> IncrementAction::determineSendValue(const json &settings)
{
    const auto increment_cmd_value_str = EPLJSONUtils::GetStringByName(settings, "increment_value");
    const auto increment_min_str = EPLJSONUtils::GetStringByName(settings, "increment_min");
    const auto increment_max_str = EPLJSONUtils::GetStringByName(settings, "increment_max");
    const bool cycling_is_allowed = EPLJSONUtils::GetBoolByName(settings, "increment_cycle_allowed_check");

    if (is_number(increment_cmd_value_str) && is_number(increment_min_str) && is_number(increment_max_str)) {
        const auto value = increment_monitor_.get_increment_after_command(Decimal(increment_cmd_value_str),
                                                                          Decimal(increment_min_str),
                                                                          Decimal(increment_max_str),
                                                                          cycling_is_allowed);
        return value.str();
    }
    return std::nullopt;
}

void IncrementAction::handleEncoderRotation(SimulatorInterface *simulator_interface,
                                           ESDConnectionManager *mConnectionManager,
                                           const json &inPayload,
                                           int ticks)
{
    const auto settings = inPayload["settings"];
    const auto send_address = EPLJSONUtils::GetStringByName(settings, "send_address");
    const auto increment_cw_str = EPLJSONUtils::GetStringByName(settings, "increment_cw");
    const auto increment_ccw_str = EPLJSONUtils::GetStringByName(settings, "increment_ccw");
    const auto increment_min_str = EPLJSONUtils::GetStringByName(settings, "increment_min");
    const auto increment_max_str = EPLJSONUtils::GetStringByName(settings, "increment_max");
    const bool cycling_is_allowed = EPLJSONUtils::GetBoolByName(settings, "increment_cycle_allowed_check");

    // Debug logging
    mConnectionManager->LogMessage("[Encoder Rotation] send_address: " + send_address);
    mConnectionManager->LogMessage("[Encoder Rotation] increment_cw: " + increment_cw_str);
    mConnectionManager->LogMessage("[Encoder Rotation] increment_ccw: " + increment_ccw_str);
    mConnectionManager->LogMessage("[Encoder Rotation] min: " + increment_min_str + " max: " + increment_max_str);
    mConnectionManager->LogMessage("[Encoder Rotation] ticks: " + std::to_string(ticks));

    // Update increment monitor with current game state
    increment_monitor_.update_settings(settings);
    increment_monitor_.update(simulator_interface);

    // Choose the appropriate increment value based on rotation direction
    std::string increment_value_str;
    if (ticks > 0) {
        // Clockwise rotation
        increment_value_str = increment_cw_str;
        mConnectionManager->LogMessage("[Encoder Rotation] Direction: CW, using increment_cw");
    } else if (ticks < 0) {
        // Counter-clockwise rotation
        increment_value_str = increment_ccw_str;
        mConnectionManager->LogMessage("[Encoder Rotation] Direction: CCW, using increment_ccw");
    } else {
        // No rotation
        mConnectionManager->LogMessage("[Encoder Rotation] No rotation (ticks = 0), ignoring");
        return;
    }

    if (is_number(increment_value_str) && is_number(increment_min_str) && is_number(increment_max_str)) {
        const Decimal increment_value(increment_value_str);
        
        // Use absolute value of ticks since direction is already handled by choosing CW or CCW value
        const Decimal delta_cmd = increment_value * Decimal(std::to_string(std::abs(ticks)));
        
        const auto value = increment_monitor_.get_increment_after_command(delta_cmd,
                                                                          Decimal(increment_min_str),
                                                                          Decimal(increment_max_str),
                                                                          cycling_is_allowed);
        mConnectionManager->LogMessage("[Encoder Rotation] Sending value: " + value.str() + " to address: " + send_address);
        simulator_interface->send_command(send_address, value.str());
    } else {
        mConnectionManager->LogMessage("[Encoder Rotation] Invalid settings - not all values are numbers");
    }
}

void IncrementAction::handleEncoderPress(SimulatorInterface *simulator_interface,
                                        ESDConnectionManager *mConnectionManager,
                                        const json &inPayload)
{
    const auto settings = inPayload["settings"];
    const auto send_address = EPLJSONUtils::GetStringByName(settings, "send_address");
    
    // Debug logging
    mConnectionManager->LogMessage("[Encoder Press] send_address: " + send_address);
    
    // Verify send_address is not empty
    if (send_address.empty()) {
        mConnectionManager->LogMessage("[Encoder Press] send_address is empty - cannot send command");
        return;
    }
    
    // Check if there's a fixed value setting for encoder press (e.g., "encoder_press_value")
    const auto encoder_press_value_str = EPLJSONUtils::GetStringByName(settings, "encoder_press_value");
    
    mConnectionManager->LogMessage("[Encoder Press] encoder_press_value: " + encoder_press_value_str);
    
    if (!encoder_press_value_str.empty()) {
        // Send the fixed value configured for encoder press
        mConnectionManager->LogMessage("[Encoder Press] Sending fixed value: " + encoder_press_value_str);
        simulator_interface->send_command(send_address, encoder_press_value_str);
    } else {
        // Fallback: if no fixed value is set, use increment_min as default (reset to minimum)
        const auto increment_min_str = EPLJSONUtils::GetStringByName(settings, "increment_min");
        if (!increment_min_str.empty()) {
            mConnectionManager->LogMessage("[Encoder Press] Sending min value: " + increment_min_str);
            simulator_interface->send_command(send_address, increment_min_str);
        } else {
            mConnectionManager->LogMessage("[Encoder Press] No value to send - both encoder_press_value and increment_min are empty");
        }
    }
}
