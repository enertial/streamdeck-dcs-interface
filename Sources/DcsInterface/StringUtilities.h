// Copyright 2020 Charles Tytler

#pragma once

#include <sstream>
#include <stdlib.h>
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

/**
 * @brief Provides a type for decimal values which can be converted to/from string representation and supports
 * summation and comparison while maintaining precision.
 *
 */
class Decimal {
  public:
    Decimal();
    Decimal(std::string number);
    Decimal(int significant_digits, int exponent);

    /**
     * @brief Returns a string representation of the decimal value.
     *
     */
    std::string str();

    /**
     * @brief Overloaded summation operators allow add and subtract while maintaining precision of highest-exponent
     * Decimal.
     *
     */
    friend Decimal operator+(const Decimal &lhs, const Decimal &rhs);
    friend Decimal operator-(const Decimal &lhs, const Decimal &rhs);
    inline Decimal &operator+=(const Decimal &rhs) {
        *this = *this + rhs;
        return *this;
    }
    inline Decimal &operator-=(const Decimal &rhs) {
        *this = *this - rhs;
        return *this;
    }

    friend bool operator<(const Decimal &lhs, const Decimal &rhs);
    friend inline bool operator>(const Decimal &lhs, const Decimal &rhs) { return rhs < lhs; }
    friend inline bool operator<=(const Decimal &lhs, const Decimal &rhs) { return !(lhs > rhs); }
    friend inline bool operator>=(const Decimal &lhs, const Decimal &rhs) { return !(lhs < rhs); }

    friend bool operator==(const Decimal &lhs, const Decimal &rhs);
    friend inline bool operator!=(const Decimal &lhs, const Decimal &rhs) { return !(lhs == rhs); }

  private:
    int significant_digits_; // The significant digits of the decimal value.
    int exponent_;           // Exponent such that the numeric value = significant_digits * 10 ^ (exponent).

    /**
     * @brief Converts a string to a Decimal represtentation (significant_digits and exponent).
     *
     * @param number String representing a numeric value.
     */
    void string_to_decimal(std::string number);

    /**
     * @brief Get significant digit representation with a higher exponent value.
     *        Note: it is assumed that current exponent_ <= higher_exponent.
     *
     * @param higher_exponent An exponent value (higher or equal to current exponent_).
     * @return Equivalent significant digits representation when using a higher exponent value.
     */
    int get_as_higher_exponent(const int higher_exponent) const;
};