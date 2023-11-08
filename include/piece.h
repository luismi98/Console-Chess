//Luis Fernandez - 30 March 2020
//This code contains the declaration of the abstract base class piece, interface for all pieces.
//The virtual member functions are those to return the allowed moves and the piece symbol as a string

#ifndef PIECE_H
#define PIECE_H

#include "enum_attributes.h"
#include <utility>
#include <list>
#include <array>

class board;

class piece {
protected:
    inline static int number_of_pieces{};
    std::pair<char, int> location{};
    piece_colour colour{};
    piece_symbol symbol{};
public:
    piece();
    virtual ~piece();
    std::pair<char, int> get_location() const noexcept;
    virtual void set_to_location(const int& new_location, const std::array<board_occupation, 64>& board_matrix);
    piece_colour get_colour() const noexcept;
    piece_symbol get_symbol() const noexcept;
    virtual std::list<int> get_allowed_moves(const std::array<board_occupation, 64>& board_matrix) const = 0;
    virtual std::string get_symbol_string(const bool& on_white_tile) const noexcept = 0;
};

#endif