//Luis Fernandez - 26 March 2020
//This code contains the implementation of functions used to convert from location in 8x8 board matrix to 1D array and viceversa.

#include "coordinate_transforms.h"
#include <iostream>
#include <map>

namespace coordinates {
    const bool in_board_range(const int& location) {
        return (0 < location && location <= 64);
    }
    //Converts from board coordinates (char-int pair) to location along 1D array.
    //Convert from location along 1D array to board coordinates (char-int pair)

    int flatten_board_coordinates(const std::pair<char, int>& board_coordinates) {
        
        int matrix_column{};
        try {
            matrix_column = alphabetical_dictionary.at(board_coordinates.first);
        }
        catch (const std::out_of_range&) {
            throw std::out_of_range("Error: column must be a letter between 'a' and 'h'.");
        }
        int matrix_row{ board_coordinates.second };
        if (matrix_row < 1 || matrix_row > 8) {
            throw std::out_of_range("Error: row must be a number between 1 and 8.");
        }
        return 8 * (matrix_row - 1) + matrix_column; //location from 1 to 64
    }
    std::pair<char, int> to_board_coordinates(const int& location) {
        if (location < 1 || location > 64) {
            throw std::out_of_range("Error: location along 1D array must be in range from 1 to 64.");
        }
        std::pair<char, int> board_coordinates;
        int int_division{ location / 8 };
        int remainder{ location % 8 };
        int row{ remainder ? int_division + 1 : int_division };
        int column{ remainder ? remainder : 8 };
        board_coordinates.first = numerical_dictionary.at(column);
        board_coordinates.second = row;
        return board_coordinates;
    }

    int get_column_number(const int& location) {
        //Returns the column of a given board location
        if (location < 1 || location > 64) {
            throw std::out_of_range("Error: location to get column must be a number between 1 and 64.");
        }
        int remainder{ location % 8 };
        int column{ remainder ? remainder : 8 };
        return column;
    }
}