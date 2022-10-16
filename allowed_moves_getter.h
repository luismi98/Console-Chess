#ifndef ALLOWED_MOVES_GETTER_H
#define ALLOWED_MOVES_GETTER_H

/*
This template class takes a step as parameter and has a method which calculates all the allowed moves of a piece of 
"infinite range" (rook, bishop and queen) taking steps from the current location. 
It takes into account the occupation of the board.
*/

#include "enum_attributes.h"
#include <list>
#include <array>

template <int step> class allowed_moves_getter {
public:
    std::list<int> get_allowed_moves(const int& current_location, const std::array<board_occupation, 64>& board_matrix, 
                                                                  const piece_colour& colour_opposite) {
        std::list<int> allowed_moves;
        board_occupation occupation;
        int column{ coordinates::get_column_number(current_location) };
        int move_step{ step };
        int last_column{ column };
        while (0 < current_location + move_step && current_location + move_step <= 64) { //Check out of bounds
            if (abs(last_column - coordinates::get_column_number(current_location + move_step)) > 1) {
                break; //Break if it goes off the board on the left or right edges
            }
            occupation = board_matrix.at(current_location + move_step - 1);
            if (occupation == board_occupation::empty) { //Board is empty, so allow move
                allowed_moves.push_back(current_location + move_step);
            }
            else if (static_cast<int>(occupation) == static_cast<int>(colour_opposite)) {
                allowed_moves.push_back(current_location + move_step); //Allow move for potential capture of the piece of opposite colour
                break;
            }
            else { //Break if board location occupied by piece of same colour
                break;
            }
            last_column = coordinates::get_column_number(current_location + move_step);
            move_step += step;
        }
        return allowed_moves;
    }
};

#endif