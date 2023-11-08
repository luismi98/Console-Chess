/*
Luis Fernandez - 20 April 2020
This file contains the implementation of the queen class. Its starting position is in column 'd'.
The char column parameter in the constructor was set so that the template function in the board constructor
to fill the vector of pieces can be used.
It combines the allowed moves of the rook and bishop.
*/

#include "piece.h"
#include "queen.h"
#include "coordinate_transforms.h"
#include "enum_attributes.h"
#include "allowed_moves_getter.h"
#include <list>
#include <array>

queen::queen() {
    //std::cout << "Queen default constructor called." << std::endl;
}

queen::queen(const char& column, const piece_colour& colour_in) {
    //std::cout << "Queen parametrised constructor called." << std::endl;
    try {
        int column_number{ coordinates::alphabetical_dictionary.at(column) };
    }
    catch (const std::out_of_range&) {
        throw std::out_of_range("Error: columns go from 'a' to 'h'.");
    }
    if (column != 'd') {
        throw std::out_of_range("Error: queen column can only be initialised to 'd'.");
    }
    if (colour_in != piece_colour::white && colour_in != piece_colour::black) {
        throw std::invalid_argument("Error: invalid colour.");
    }
    colour = colour_in;
    location.first = column;
    location.second = (colour == piece_colour::white) ? 1 : 8;
    symbol = piece_symbol::queen;
}

queen::~queen() {
    //std::cout << "Destructor of queen of colour " << colour_string_map.at(colour) << " called at position " << location.first << location.second << "." << std::endl;
}

std::list<int> queen::get_allowed_moves(const std::array<board_occupation, 64>& board_matrix) const {
    std::list<int> allowed_moves;
    piece_colour colour_opposite{ opposite_colour.at(colour) };
    int current_location{ coordinates::flatten_board_coordinates(location) };

    //Use objects from the allowed_moves_getter files
    const int horizontal_step{ 1 };
    allowed_moves_getter<horizontal_step> right;
    allowed_moves_getter<-horizontal_step> left;
    allowed_moves.splice(allowed_moves.end(), right.get_allowed_moves(current_location, board_matrix, colour_opposite));
    allowed_moves.splice(allowed_moves.end(), left.get_allowed_moves(current_location, board_matrix, colour_opposite));

    const int vertical_step{ 8 };
    allowed_moves_getter<vertical_step> down;
    allowed_moves_getter<-vertical_step> up;
    allowed_moves.splice(allowed_moves.end(), down.get_allowed_moves(current_location, board_matrix, colour_opposite));
    allowed_moves.splice(allowed_moves.end(), up.get_allowed_moves(current_location, board_matrix, colour_opposite));

    const int major_diagonal_step{ 9 };
    allowed_moves_getter<major_diagonal_step> down_right;
    allowed_moves_getter<-major_diagonal_step> up_left;
    allowed_moves.splice(allowed_moves.end(), down_right.get_allowed_moves(current_location, board_matrix, colour_opposite));
    allowed_moves.splice(allowed_moves.end(), up_left.get_allowed_moves(current_location, board_matrix, colour_opposite));

    const int minor_diagonal_step{ 7 };
    allowed_moves_getter<minor_diagonal_step> down_left;
    allowed_moves_getter<-minor_diagonal_step> up_right;
    allowed_moves.splice(allowed_moves.end(), down_left.get_allowed_moves(current_location, board_matrix, colour_opposite));
    allowed_moves.splice(allowed_moves.end(), up_right.get_allowed_moves(current_location, board_matrix, colour_opposite));

    return allowed_moves;
}

std::string queen::get_symbol_string(const bool& on_white_tile) const noexcept {
    if (on_white_tile) {
        return (colour == piece_colour::white) ? u8"♕" : u8"♛";
    }
    else {
        return (colour == piece_colour::white) ? u8"♛" : u8"♕";
    }
}