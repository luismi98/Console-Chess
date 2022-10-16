/*
Luis Fernandez - 30 April
This file contains the declaration of the board class. It contains the pieces. The member functions are designed so that
the member functions of the pieces can be accessed from this class.
*/

#ifndef BOARD_H
#define BOARD_H

#include "enum_attributes.h"
#include "console_visualisation.h"
#include <vector>
#include <list>
#include <array>
#include <string>
#include <map>

class piece; //Forward declaration of the piece class to use it in the board's member data

class board {
private:
    std::vector<std::unique_ptr<piece>> pieces_ingame;
    std::map<std::pair<char, int>, std::unique_ptr<piece>> pieces_map;
    std::vector<std::unique_ptr<piece>> cemetery;
    std::array<board_occupation, 64> board_matrix{};
    std::vector<std::string> moves_history{};
    std::map<std::pair<char, int>, int> piece_location_to_index_dictionary;
public:
    board();
    ~board();

    //get and set elements of the board matrix
    board_occupation get_element(const int& element) const;
    board_occupation& set_element(const int& element);

    //set and get the moves of the pieces
    std::list<std::string> get_all_pieces_allowed_moves(const piece_colour& colour_turn, const bool& last_move_check, const int& last_move) const;
    std::list<int> get_piece_allowed_moves(const std::pair<char, int>& piece_board_coords, const bool& last_move_check, const int& last_move) const;
    void set_piece_location(const std::pair<char, int>& piece_board_coords, const std::pair<char, int>& new_coordinates);

    bool check_for_check(const std::pair<char, int>& last_move_opposite_team, bool& last_move_check); //check if king is in check
    piece_symbol capture_piece(const std::pair<char, int>& new_piece_coords, const piece_colour& colour_turn); //send piece to cemetery
    void store_move(const std::pair<char, int>& new_board_coords); //store move in move_history

    //Two overloads to show the board, the second one highlighting a selected piece
    void show(const font& chosen_font, const piece_colour& colour_turn, const bool& check) const;
    void show(const font& chosen_font, const piece_colour& colour_turn, const std::pair<char, int>& piece_coords, const bool& check) const;
};

#endif