/*
Luis Fernandez - 10 April
Declaration of rook piece. It overloads the pure virtual functions of the piece abstract base class.
It has a default and a parametrised constructor, as well as a destructor.
*/

#ifndef ROOK_H
#define ROOK_H

#include "piece.h"
#include "enum_attributes.h"
#include <list>
#include <array>

class rook : public piece {
public:
    rook();
    rook(const char& column, const piece_colour& colour_in);
    ~rook();
    std::list<int> get_allowed_moves(const std::array<board_occupation, 64>& board_matrix) const;
    std::string get_symbol_string(const bool& on_white_tile) const noexcept;
};

#endif