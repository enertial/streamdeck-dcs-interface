// Copyright 2020 Charles Tytler

#include "StringUtilities.h"

#include <stdlib.h>

bool is_integer(const std::string &str)
{
    if (!str.empty()) {
        // Add special handling for stripping of trailing spaces
        const std::string str_trailing_spaces_stripped = str.substr(0, str.find_last_not_of(" ") + 1);
        if (!str_trailing_spaces_stripped.empty()) {
            // Check if all characters are base 10 digits (0-9).
            char *ptr_to_first_non_numeric_char;
            strtol(str_trailing_spaces_stripped.c_str(), &ptr_to_first_non_numeric_char, 10);
            return (*ptr_to_first_non_numeric_char == '\0');
        }
    }
    return false;
}

bool is_number(const std::string &str)
{
    if (!str.empty()) {
        // Add special handling for stripping of trailing spaces
        const std::string str_trailing_spaces_stripped = str.substr(0, str.find_last_not_of(" ") + 1);
        if (!str_trailing_spaces_stripped.empty()) {
            // Check if all characters can be converted to a floating point number.
            char *ptr_to_first_non_numeric_char;
            strtof(str_trailing_spaces_stripped.c_str(), &ptr_to_first_non_numeric_char);
            return (*ptr_to_first_non_numeric_char == '\0');
        }
    }
    return false;
}

std::optional<std::pair<std::string, std::string>> split_pair(const std::string &str, const char delim)
{
    std::pair<std::string, std::string> string_pair;
    const auto delim_loc = str.find(delim);
    if (delim_loc != std::string::npos && delim_loc > 0) {
        string_pair.first = str.substr(0, delim_loc);
        string_pair.second = str.substr(delim_loc + 1, str.size());
        return string_pair;
    }
    return std::nullopt;
}

std::optional<std::pair<std::string, std::string>>
pop_key_and_value(std::stringstream &ss, const char token_delim, const char key_value_delim)
{
    // Iterate through tokens received from single message.
    std::string token;
    if (std::getline(ss, token, token_delim)) {
        return split_pair(token, key_value_delim);
    }
    return std::nullopt;
}
