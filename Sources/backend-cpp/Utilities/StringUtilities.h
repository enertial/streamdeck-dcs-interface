// Copyright 2020 Charles Tytler

#pragma once

#include <optional>
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
 * @brief Splits a string into a pair given a delimiter character.
 *    Example split_pair("foo,bar",",") => pair<"foo","bar">
 *
 * @param str [in] String containing two substrings separated by a delimiter.
 * @param key_value_delim [in] Delimiter separating two substrings.
 * @return Optional: Pair if it was found, nullopt if delimiter split was unsuccessful.
 */
std::optional<std::pair<std::string, std::string>> split_pair(const std::string &str, const char delim);

/**
 * @brief Get the next key and value pair from a delimited stringstream.
 *    Example stringstream using token_delim(,) and key_value_delim(:):
 *      "key:value,key:value,key:value"
 *
 * @param ss [in,out]          String stream.
 * @param token_delim [in]     Delimiter separating key-value pairs.
 * @param key_value_delim [in] Delimiter separating key from value.
 * @return Optional: key-value pair if it was found, nullopt if no remaining key-value pairs in stringstream.
 */
std::optional<std::pair<std::string, std::string>>
pop_key_and_value(std::stringstream &ss, const char token_delim, const char key_value_delim);
