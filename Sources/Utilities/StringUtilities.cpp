// Copyright 2020 Charles Tytler

#include "pch.h"

#include <stdlib.h>
#include "StringUtilities.h"

bool is_integer(const std::string& str)
{
    if (!str.empty()) {
        // Add special handling for stripping of trailing spaces
        const std::string str_trailing_spaces_stripped = str.substr(0, str.find_last_not_of(" ") + 1);
        if (!str_trailing_spaces_stripped.empty()) {
            // Check if all characters are base 10 digits (0-9).
            char* ptr_to_first_non_numeric_char;
            strtol(str_trailing_spaces_stripped.c_str(), &ptr_to_first_non_numeric_char, 10);
            return (*ptr_to_first_non_numeric_char == '\0');
        }
    }
    return false;
}

bool is_number(const std::string& str)
{
    if (!str.empty()) {
        // Add special handling for stripping of trailing spaces
        const std::string str_trailing_spaces_stripped = str.substr(0, str.find_last_not_of(" ") + 1);
        if (!str_trailing_spaces_stripped.empty()) {
            // Check if all characters can be converted to a floating point number.
            char* ptr_to_first_non_numeric_char;
            strtof(str_trailing_spaces_stripped.c_str(), &ptr_to_first_non_numeric_char);
            return (*ptr_to_first_non_numeric_char == '\0');
        }
    }
    return false;
}

bool pop_key_and_value(std::stringstream& ss,
                       const char token_delim,
                       const char key_value_delim,
                       std::pair<std::string, std::string>& key_and_value)
{
    // Iterate through tokens received from single message.
    std::string token;
    if (std::getline(ss, token, token_delim)) {
        // Parse token string of the form:
        //   "key<key_value_delim>value"
        const auto key_value_delim_loc = token.find(key_value_delim);
        if (key_value_delim_loc != std::string::npos && key_value_delim_loc > 0) {
            key_and_value.first = token.substr(0, key_value_delim_loc);
            key_and_value.second = token.substr(key_value_delim_loc + 1, token.size());
            return true;
        }
    }
    return false;
}
