/*
Luis Fernandez - 20 April 2020
This file contains the implementation of the king class. Its starting position is in column 'e'.
The char column parameter in the constructor was set so that the template function in the board constructor 
to fill the vector of pieces can be used.
The king can move by one tile in all directions.
*/

#include "piece.h"
#include "king.h"
#include "board.h"
#include "coordinate_transforms.h"
#include "enum_attributes.h"
#include <list>
#include <array>

king::king() {
    //std::cout << "King default constructor called." << std::endl;
}
king::king(const char& column, const piece_colour& colour_in) {
    //std::cout << "King parametrised constructor called." << std::endl;
    try {
        int column_number{ coordinates::alphabetical_dictionary.at(column) };
    }
    catch (const std::out_of_range&) {
        throw std::out_of_range("Error: columns go from 'a' to 'h'.");
    }
    if (column != 'e') {
        throw std::out_of_range("Error: king column can only be initialised to 'e'.");
    }
    if (colour_in != piece_colour::white && colour_in != piece_colour::black) {
        throw std::invalid_argument("Error: invalid colour.");
    }
    colour = colour_in;
    location.first = column;
    location.second = (colour == piece_colour::white) ? 1 : 8;
    symbol = piece_symbol::king;
}
king::~king() {
    //std::cout << "Destructor of king of colour " << colour_string_map.at(colour) << " called at position " << location.first << location.second << "." << std::endl;
}
std::list<int> king::get_allowed_moves(const std::array<board_occupation, 64>& board_matrix) const {
    std::list<int> allowed_moves;
    piece_colour colour_opposite{ opposite_colour.at(colour) };
    int current_location{ coordinates::flatten_board_coordinates(location) };
    int horizontal_step{ 1 };
    int vertical_step{ 8 };
    int vertical_change{};
    int horizontal_change{};
    int location{};
    //Loop over all possible permutations of change in horizontal and vertical direction
    for (int factor_vertical{ -1 }; factor_vertical < 2; factor_vertical += 1) {
        for (int factor_horizontal{ -1 }; factor_horizontal < 2; factor_horizontal += 1) {
            vertical_change = factor_vertical * vertical_step;
            horizontal_change = factor_horizontal * horizontal_step;
            //Calculate new location to be tested by adding up the horizontal and vertical changes to the old location
            location = current_location + vertical_change + horizontal_change;
            if (coordinates::in_board_range(location)) {
                if (board_matrix.at(location - 1) == board_occupation::empty || static_cast<int>(board_matrix.at(location - 1)) == static_cast<int>(colour_opposite)) {
                    //Can move if board is empty or occupied by piece of opposite colour
                    allowed_moves.push_back(location);
                }
            }
        }
    }
    return allowed_moves;
}
std::string king::get_symbol_string(const bool& on_white_tile) const noexcept {
    if (on_white_tile) {
        return (colour == piece_colour::white) ? u8"♔" : u8"♚";
    }
    else {
        return (colour == piece_colour::white) ? u8"♚" : u8"♔";
    }
}