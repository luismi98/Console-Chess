/*
Luis Fernandez - 10 April 2020
Declaration of knight piece. It overloads the pure virtual functions of the piece abstract base class.
It has a default and a parametrised constructor, as well as a destructor.
*/

#ifndef KNIGHT_H
#define KNIGHT_H

#include "piece.h"
#include "enum_attributes.h"
#include <list>
#include <array>

class knight : public piece {
public:
    knight();
    knight(const char& column, const piece_colour& colour_in);
    ~knight();
    std::list<int> get_allowed_moves(const std::array<board_occupation, 64>& board_matrix) const;
    std::string get_symbol_string(const bool& on_white_tile) const noexcept;
};

#endif