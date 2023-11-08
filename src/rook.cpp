/*
Luis Fernandez - 25 April 2020
This file contains the implementation of the rook class. Its starting position is in column 'a' or 'h'.
It can move along straight lines along rows and columns.
*/

#include "piece.h"
#include "board.h"
#include "rook.h"
#include "coordinate_transforms.h"
#include "enum_attributes.h"
#include "allowed_moves_getter.h"
#include <list>
#include <array>

rook::rook() {
    //std::cout << "Rook default constructor called." << std::endl;
}
rook::rook(const char& column, const piece_colour& colour_in) {
    //std::cout << "Rook parametrised constructor called." << std::endl;
    try {
        int column_number{ coordinates::alphabetical_dictionary.at(column) };
    }
    catch (const std::out_of_range&) {
        throw std::out_of_range("Error: columns go from 'a' to 'h'.");
    }
    if (column != 'a' && column != 'h') {
        throw std::out_of_range("Error: rook column can only be initialised to 'a' or 'h'.");
    }
    if (colour_in != piece_colour::white && colour_in != piece_colour::black) {
        throw std::invalid_argument("Error: invalid rook colour.");
    }
    location.first = column;
    colour = colour_in;
    location.second = (colour == piece_colour::white) ? 1 : 8; //Starting row only depends in colour of piece
    symbol = piece_symbol::rook;
}
rook::~rook() {
    //std::cout << "Destructor of rook of colour " << colour_string_map.at(colour) << " called at position " << location.first << location.second << "." << std::endl;
}
std::list<int> rook::get_allowed_moves(const std::array<board_occupation, 64>& board_matrix) const {
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
    allowed_moves_getter<vertical_step> up;
    allowed_moves_getter<-vertical_step> down;
    allowed_moves.splice(allowed_moves.end(), up.get_allowed_moves(current_location, board_matrix, colour_opposite));
    allowed_moves.splice(allowed_moves.end(), down.get_allowed_moves(current_location, board_matrix, colour_opposite));

    return allowed_moves;
}
std::string rook::get_symbol_string(const bool& on_white_tile) const noexcept {
    if (on_white_tile) {
        return (colour == piece_colour::white) ? u8"♖" : u8"♜";
    }
    else {
        return (colour == piece_colour::white) ? u8"♜" : u8"♖";
    }
}