/*
Luis Fernandez - 25 April
Implementation of the bishop piece. Its starting position is in column 'c' or 'f'.
It can move in diagonal lines. 
*/

#include "piece.h"
#include "bishop.h"
#include "coordinate_transforms.h"
#include "enum_attributes.h"
#include "allowed_moves_getter.h"
#include <list>
#include <array>

bishop::bishop() {
    //std::cout << "Bishop default constructor called." << std::endl;
}
bishop::bishop(const char& column, const piece_colour& colour_in) {
    //std::cout << "Bishop parametrised constructor called." << std::endl;
    try {
        int column_number{ coordinates::alphabetical_dictionary.at(column) };
    }
    catch (const std::out_of_range&) {
        throw std::out_of_range("Error: columns must go from 'a' to 'h'.");
    }
    if (column != 'c' && column != 'f') {
        throw std::out_of_range("Error: bishop column can only be initialised to 'c' or 'f'.");
    }
    if (colour_in != piece_colour::white && colour_in != piece_colour::black) {
        throw std::invalid_argument("Error: invalid colour.");
    }
    location.first = column;
    colour = colour_in;
    location.second = (colour == piece_colour::white) ? 1 : 8;
    symbol = piece_symbol::bishop;
}
bishop::~bishop() {
    //std::cout << "Destructor of bishop of colour " << colour_string_map.at(colour) << " called at position " << location.first << location.second << "." << std::endl;
}
std::list<int> bishop::get_allowed_moves(const std::array<board_occupation, 64>& board_matrix) const {
    std::list<int> allowed_moves;
    piece_colour colour_opposite{ opposite_colour.at(colour) };
    int current_location{ coordinates::flatten_board_coordinates(location) };

    //Use objects from the allowed_moves_getter files
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
std::string bishop::get_symbol_string(const bool& on_white_tile) const noexcept {
    if (on_white_tile) {
        return (colour == piece_colour::white) ? u8"♗" : u8"♝";
    }
    else {
        return (colour == piece_colour::white) ? u8"♝" : u8"♗";
    }
}