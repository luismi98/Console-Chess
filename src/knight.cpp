/*
Luis Fernandez - 25 April 2020
Implementation of the knight piece. Its starting position is in column 'b' or 'g'
It can move in L-shapes as will be described below.
*/

#include "piece.h"
#include "knight.h"
#include "coordinate_transforms.h"
#include "enum_attributes.h"
#include <list>
#include <array>

knight::knight() {
    //std::cout << "Knight default constructor called." << std::endl;
}

knight::knight(const char& column, const piece_colour& colour_in) {
    //std::cout << "Knight parametrised constructor called." << std::endl;
    try {
        int column_number{ coordinates::alphabetical_dictionary.at(column) };
    }
    catch (const std::out_of_range&) {
        throw std::out_of_range("Error: columns go from 'a' to 'h'.");
    }
    if (column != 'b' && column != 'g') {
        throw std::out_of_range("Error: knight column can only be initialised to 'b' or 'g'.");
    }
    if (colour_in != piece_colour::white && colour_in != piece_colour::black) {
        throw std::invalid_argument("Error: invalid colour.");
    }
    location.first = column;
    colour = colour_in;
    location.second = (colour == piece_colour::white) ? 1 : 8;
    symbol = piece_symbol::knight;
}

knight::~knight() {
    //std::cout << "Destructor of knight of colour " << colour_string_map.at(colour) << " called at position " << location.first << location.second << "." << std::endl;
}

std::list<int> knight::get_allowed_moves(const std::array<board_occupation, 64>& board_matrix) const {
    std::list<int> allowed_moves;
    piece_colour colour_opposite{ opposite_colour.at(colour) };
    int current_location{ coordinates::flatten_board_coordinates(location) };
    int column{ coordinates::alphabetical_dictionary.at(location.first)};
    int horizontal_step{ 1 };
    int vertical_step{ 8 };

    int vertical_change{};
    int horizontal_change{};
    int location{};
    //Construct new location by iterating over all the possible permutations of horizontal and vertical change
    //The knight can move to those locations that differ from the original location by 1 row and 2 columns, or viceversa
    for (int coeff_vertical{ 1 }, coeff_horizontal{ 2 }; coeff_vertical < 3 && coeff_horizontal > 0; coeff_vertical++, coeff_horizontal--) {
        for (int sign_vertical{ -1 }; sign_vertical < 2; sign_vertical += 2) {
            for (int sign_horizontal{ -1 }; sign_horizontal < 2; sign_horizontal += 2) {
                vertical_change = sign_vertical * coeff_vertical * vertical_step;
                horizontal_change = sign_horizontal * coeff_horizontal * horizontal_step;
                //Construct new location adding up horizontal and vertical change to old location
                location = current_location + vertical_change + horizontal_change;
                if (coordinates::in_board_range(location) && abs(column - coordinates::get_column_number(location)) <= 2) {
                    if (board_matrix.at(location - 1) == board_occupation::empty || static_cast<int>(board_matrix.at(location - 1)) == static_cast<int>(colour_opposite)) {
                        //Can move if board is empty or occupied by piece of opposite colour
                        allowed_moves.push_back(location);
                    }
                }
            }
        }
    }
    return allowed_moves;
}

std::string knight::get_symbol_string(const bool& on_white_tile) const noexcept {
    if (on_white_tile) {
        return (colour == piece_colour::white) ? u8"♘" : u8"♞";
    }
    else {
        return (colour == piece_colour::white) ? u8"♞" : u8"♘";
    }
}