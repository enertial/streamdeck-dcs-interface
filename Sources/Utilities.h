// Copyright 2020 Charles Tytler

#pragma once

#include <stdlib.h>
#include <string>

/**
 * @brief Helper function to identify if a string represents an integer.
 *
 */
inline bool is_integer(const std::string &str) {
    // Check if all characters are base 10 digits (0-9).
    char *ptr_to_first_non_numeric_char;
    strtol(str.c_str(), &ptr_to_first_non_numeric_char, 10);
    return (*ptr_to_first_non_numeric_char == '\0');
}

/**
 * @brief Helper function to identify if a string represents a (decimal) number.
 *
 */
inline bool is_number(const std::string &str) {
    // Check if all characters can be converted to a floating point number.
    char *ptr_to_first_non_numeric_char;
    strtof(str.c_str(), &ptr_to_first_non_numeric_char);
    return (*ptr_to_first_non_numeric_char == '\0');
}