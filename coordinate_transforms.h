/*
Luis Fernandez - 26 March
This code contains the declaration of the dictionaries used to transform from alphabetical to numerical column notation,
as well as functions to convert from location in 8x8 board matrix to 1D array and viceversa. 
*/

#ifndef COORDINATETRANSFORMATIONS_H
#define COORDINATETRANSFORMATIONS_H

#include <iostream>
#include <map>
#include <algorithm>

namespace coordinates {
    const std::string alphabetical_string{"abcdefgh"};

    template <class c_type_1, class c_type_2> const std::map<c_type_2, c_type_1> 
    reverse_dictionary(const std::map<c_type_1, c_type_2>& map_to_reverse) {
        std::map<c_type_2, c_type_1> reversed_dict;
        for (const auto& value_pair : map_to_reverse) {
            reversed_dict[value_pair.second] = value_pair.first;
        }
        return reversed_dict;
    }

    namespace {
        const std::map<char, int> create_alphabetical_dictionary() {
            std::map<char, int> alphabetical_dict;
            for (size_t i{}; i < 8; i++) {
                alphabetical_dict[alphabetical_string[i]] = i + 1;
            }
            return alphabetical_dict;
        }
    }

    const std::map<char, int> alphabetical_dictionary{ create_alphabetical_dictionary() };
    const std::map<int, char> numerical_dictionary{ reverse_dictionary(alphabetical_dictionary) };

    const bool in_board_range(const int& location);
    int flatten_board_coordinates(const std::pair<char, int>& coordinates);
    std::pair<char, int> to_board_coordinates(const int& location);
    int get_column_number(const int& location);
}

#endif