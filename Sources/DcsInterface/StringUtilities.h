// Copyright 2020 Charles Tytler

#pragma once

#include <sstream>
#include <string>

/**
 * @brief Helper function to identify if a string represents an integer.
 *
 */
bool is_integer(const std::string &str);

/**
 * @brief Helper function to identify if a string represents a (decimal) number.
 *
 */
bool is_number(const std::string &str);

/**
 * @brief Get the next key and value pair from a delimited stringstream.
 *    Example stringstream using token_delim(,) and key_value_delim(:):
 *      "key:value,key:value,key:value"
 *
 * @param ss [in,out]          String stream.
 * @param token_delim [in]     Delimiter separating key-value pairs.
 * @param key_value_delim [in] Delimiter separating key from value.
 * @param key_and_value [out]  Pair of key and value strings.
 * @return True if a key-value pair was found, False if no remaining key-value pairs in stringstream.
 */
bool pop_key_and_value(std::stringstream &ss,
                       const char token_delim,
                       const char key_value_delim,
                       std::pair<std::string, std::string> &key_and_value);
