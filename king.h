/*
Luis Fernandez - 10 April 2020
Declaration of king piece. It overloads the pure virtual functions of the piece abstract base class.
It has a default and a parametrised constructor, as well as a destructor.
*/

#ifndef KING_H
#define KING_H

#include "piece.h"
#include "enum_attributes.h"
#include <list>
#include <array>

class king : public piece {
public:
    king();
    king(const char& column, const piece_colour& colour_in);
    ~king();
    std::list<int> get_allowed_moves(const std::array<board_occupation, 64>& board_matrix) const;
    std::string get_symbol_string(const bool& on_white_tile) const noexcept;
};

#endif