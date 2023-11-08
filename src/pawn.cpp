/*
Luis Fernandez - 11 April 2020
This file contains the implementation of the pawn class. Its starting position may be in any column.
It can move forward (by 2 positions if in the starting position, otherwise 1) or diagonally by 1 if it can capture a piece.
*/

#include "piece.h"
#include "board.h"
#include "pawn.h"
#include "coordinate_transforms.h"
#include "enum_attributes.h"
#include <list>
#include <array>

pawn::pawn() {
    //std::cout << "Pawn default constructor called." << std::endl;
}

pawn::pawn(const char& column, const piece_colour& colour_in) {
    //std::cout << "Pawn parametrised constructor called." << std::endl;
    try {
        int column_number{ coordinates::alphabetical_dictionary.at(column) };
    }
    catch (const std::out_of_range&) {
        throw std::out_of_range("Error: columns go from 'a' to 'h'.");
    }
    if (colour_in != piece_colour::white && colour_in != piece_colour::black) {
        throw std::invalid_argument("Error: invalid colour.");
    }
    location.first = column;
    colour = colour_in;
    location.second = (colour == piece_colour::white) ? 2 : 7; //Only depends on colour
    symbol = piece_symbol::pawn;
}

pawn::~pawn() {
    //std::cout << "Destructor of pawn of colour " << colour_string_map.at(colour) << " called at position " << location.first << location.second << "." << std::endl;
}

std::list<int> pawn::get_allowed_moves(const std::array<board_occupation, 64>& board_matrix) const {
    std::list<int> allowed_moves;
    int current_location{ coordinates::flatten_board_coordinates(location) };
    const std::map<piece_colour, int> step_forward{ {piece_colour::white,8},{piece_colour::black,-8} };

    int forward{ current_location + step_forward.at(colour) };
    if (coordinates::in_board_range(forward) && board_matrix.at(forward - 1) == board_occupation::empty) {
        allowed_moves.push_back(forward);
    }

    //If in starting position, pawn can move two forward
    bool starting_position{ (colour == piece_colour::white) ? location.second == 2 : location.second == 7 };
    if (starting_position) {
        int forward_two{ current_location + 2 * step_forward.at(colour) };
        if (coordinates::in_board_range(forward_two) && board_matrix.at(forward_two - 1) == board_occupation::empty) {
            allowed_moves.push_back(forward_two);
        }
    }

    //Possibility of moving diagonally to capture a piece of opposite colour
    board_occupation occupation_right_diagonal{ board_matrix.at(forward + 1 - 1) };
    board_occupation occupation_left_diagonal{ board_matrix.at(forward - 1 - 1) };
    if (coordinates::in_board_range(forward + 1) && static_cast<int>(occupation_right_diagonal) == static_cast<int>(opposite_colour.at(colour))) {
        allowed_moves.push_back(forward + 1);
    }
    if (coordinates::in_board_range(forward - 1) && static_cast<int>(occupation_left_diagonal) == static_cast<int>(opposite_colour.at(colour))) {
        allowed_moves.push_back(forward - 1);
    }
    return allowed_moves;
}

std::string pawn::get_symbol_string(const bool& on_white_tile) const noexcept {
    if (on_white_tile) {
        return (colour == piece_colour::white) ? u8"♙" : u8"♟";
    }
    else {
        return (colour == piece_colour::white) ? u8"♟" : u8"♙";
    }
}