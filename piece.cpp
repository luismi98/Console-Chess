/*
Luis Fernandez - 30 March
This code contains the implementation of the abstract base class piece, interface for all pieces.
*/

#include "piece.h"
#include "board.h"
#include "coordinate_transforms.h"
#include "enum_attributes.h"
#include <algorithm>
#include <list>
#include <array>

piece::piece() {
    //std::cout << "Piece default constructor called." << std::endl;
    number_of_pieces++;
}
piece::~piece() {
    number_of_pieces--;
    //std::cout << "Destructor of base class piece at location " << location.first << location.second 
    //    << " called. Number of pieces: " << number_of_pieces << "." << std::endl;
}
std::pair<char, int> piece::get_location() const noexcept {
    return location;
}
void piece::set_to_location(const int& new_location, const std::array<board_occupation, 64>& board_matrix) {
    if (new_location < 1 || new_location > 64) {
        throw std::out_of_range("Error: new location must be a number between 1 and 64.");
    }
    std::list<int> allowed_moves{ get_allowed_moves(board_matrix) };
    auto it_moves = std::find_if(allowed_moves.begin(), allowed_moves.end(), //Check whether move is allowed
        [&new_location](const int& move) {
            return move == new_location;
        });
    if (it_moves == allowed_moves.end()) { //Move not allowed if iterator reached the end of the container
        throw std::invalid_argument("Error: could not set to specified location, move was not allowed.");
    }
    location = coordinates::to_board_coordinates(new_location); //Convert int to board coodinates
}
piece_colour piece::get_colour() const noexcept {
    return colour;
}
piece_symbol piece::get_symbol() const noexcept {
    return symbol;
}