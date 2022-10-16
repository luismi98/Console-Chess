
#include "board.h"
#include "piece.h"
#include "pawn.h"
#include "rook.h"
#include "knight.h"
#include "bishop.h"
#include "king.h"
#include "queen.h"
#include "coordinate_transforms.h"
#include "enum_attributes.h"
#include "console_visualisation.h"
#include <vector>
#include <list>
#include <array>
#include <map>
#include <exception>
#include <string>
#include <sstream>

namespace {
    template <class c_type> void fill_pieces_column(std::vector<std::unique_ptr<piece>>& pieces_ingame, const size_t& column) {
        if (column < 1 || column > 8) {
            throw std::out_of_range("Error: index of column whose pieces to fill vector with must be in range from 1 to 8.");
        }
        pieces_ingame[column - 1] = std::make_unique<c_type>(coordinates::alphabetical_string[column - 1], piece_colour::white);
        pieces_ingame[column + 7] = std::make_unique<pawn>(coordinates::alphabetical_string[column - 1], piece_colour::white);
        pieces_ingame[column + 15] = std::make_unique<pawn>(coordinates::alphabetical_string[column - 1], piece_colour::black);
        pieces_ingame[column + 23] = std::make_unique<c_type>(coordinates::alphabetical_string[column - 1], piece_colour::black);
    }
}

board::board() try : pieces_ingame(32) { //memory reservation
    //std::cout << "Board default constructor called." << std::endl;
    fill_pieces_column<rook>(pieces_ingame, 1);
    fill_pieces_column<knight>(pieces_ingame, 2);
    fill_pieces_column<bishop>(pieces_ingame, 3);
    fill_pieces_column<queen>(pieces_ingame, 4);
    fill_pieces_column<king>(pieces_ingame, 5);
    fill_pieces_column<bishop>(pieces_ingame, 6);
    fill_pieces_column<knight>(pieces_ingame, 7);
    fill_pieces_column<rook>(pieces_ingame, 8);

    for (size_t i{}; i < 64; i++) {
        if (i < 16) { //From 0 to 15 -> 16 for the white pieces
            board_matrix[i] = board_occupation::white_piece;
            piece_location_to_index_dictionary.insert({ coordinates::to_board_coordinates(i + 1), i });
        }
        else if (i > 47) { //From 48 to 63 -> 16 for the black pieces
            board_matrix[i] = board_occupation::black_piece;
            piece_location_to_index_dictionary.insert({ coordinates::to_board_coordinates(i + 1), i - 32 }); //48 - 32 = 16
        }
        else {
            board_matrix[i] = board_occupation::empty;
        }
    }

    for (const auto& pair : piece_location_to_index_dictionary) {
        pieces_map.insert({ pair.first, std::move(pieces_ingame.at(pair.second)) });
    }
    pieces_ingame.clear();
    piece_location_to_index_dictionary.clear();
}
catch (const std::bad_alloc&) {
    std::cerr << "Memory error constructing board." << std::endl;
    throw;
}
catch (const std::exception&) {
    std::cerr << "Exception caught on board constructor." << std::endl;
    throw;
}
board::~board() {
    //std::cout << "Destructor of board called." << std::endl;
    pieces_map.clear();
}
board_occupation board::get_element(const int& element) const {
    if (element < 1 || element > 64) {
        throw std::out_of_range("Error: element of board matrix to get must be in range from 1 to 64.");
    }
    return board_matrix.at(element - 1);
}
board_occupation& board::set_element(const int& element) {
    if (element < 1 || element > 64) {
        throw std::out_of_range("Error: element of board matrix to set must be in range from 1 to 64.");
    }
    return board_matrix.at(element - 1);
}
std::list<std::string> board::get_all_pieces_allowed_moves(const piece_colour& colour_turn, const bool& last_move_check, const int& last_move) const {
    std::list<std::pair<char, int>> all_current_locations;
    std::list<int> all_allowed_future_locations;
    std::list<int> single_piece_allowed_future_locations;

    for (const auto& piece_pair : pieces_map) { //Loop over all pieces
        if (piece_pair.second->get_colour() == colour_turn) {
            single_piece_allowed_future_locations = get_piece_allowed_moves(piece_pair.second->get_location(), last_move_check, last_move);
            for (const int& future_location : single_piece_allowed_future_locations) { //Loop over the individual piece's moves
                //Store the current location for as many allowed moves available.
                //This is so below we can loop over both current and future locations simultaneously
                all_current_locations.push_back(piece_pair.second->get_location());
            }
            all_allowed_future_locations.splice(all_allowed_future_locations.end(), single_piece_allowed_future_locations);
        }
    }
    if (all_current_locations.size() != all_allowed_future_locations.size()) {
        throw std::length_error("Fatal error: lists of current and future locations did not match in size.");
    }

    std::list<std::string> all_allowed_moves_string;
    std::pair<char, int> future_board_coord;
    std::ostringstream move_stringstream;
    auto it_current_loc = all_current_locations.begin();
    auto it_future_loc = all_allowed_future_locations.begin();

    //Use a ostringstream to store all the allowed moves in a string of format "a2->b3".
    for (; it_current_loc != all_current_locations.end() && it_future_loc != all_allowed_future_locations.end(); ++it_current_loc, ++it_future_loc) {
        future_board_coord = coordinates::to_board_coordinates(*it_future_loc);
        move_stringstream << (*it_current_loc).first << (*it_current_loc).second << u8"→" << future_board_coord.first << future_board_coord.second;
        all_allowed_moves_string.push_back(move_stringstream.str());
        move_stringstream.str(""); //clear stringstream
    }
    all_allowed_moves_string.sort();
    return all_allowed_moves_string;
}
bool board::check_for_check(const std::pair<char, int>& last_move_opposite_team, bool& last_move_check) {
    if (pieces_map.count(last_move_opposite_team) == 0) {
        throw std::out_of_range("The specified piece of opposite colour that last moved does not exist!");
    }
    piece_colour colour_turn{ opposite_colour.at(pieces_map.at(last_move_opposite_team)->get_colour()) }; //current colour turn

    std::pair<char, int> king_coords;
    for (const auto& piece_pair : pieces_map) { //Get coordinates of king
        if (piece_pair.second->get_symbol() == piece_symbol::king && piece_pair.second->get_colour() == colour_turn) {
            king_coords = piece_pair.second->get_location();
            break;
        }
    }
    std::list<int> allowed_moves_last_piece{ pieces_map.at(last_move_opposite_team)->get_allowed_moves(board_matrix) };
    for (const int& move_last_piece : allowed_moves_last_piece) { //Check if piece of opposite team that last moved is now attacking the king
        if (move_last_piece == coordinates::flatten_board_coordinates(king_coords)) {
            last_move_check = true; //Set variable, taken by reference, to true
            return true;
        }
    }
    std::list<int> inf_allowed_moves; //allowed moves of pieces of infinite range (rook, bishop, queen)
    std::pair<char, int> inf_piece_loc;
    piece_symbol inf_piece_symbol;
    bool check_moves{ false };
    for (const auto& piece_pair : pieces_map) {
        if (piece_pair.second->get_colour() == colour_turn) {
            continue; //only check the pieces of opposite colour to the king in the current colour turn
        }
        inf_piece_loc = piece_pair.second->get_location();
        inf_piece_symbol = piece_pair.second->get_symbol();
        if (inf_piece_loc == last_move_opposite_team) { //we have already checked the moves of the piece of opposite team that last moved
            continue;
        }
        else if (inf_piece_symbol == piece_symbol::bishop || inf_piece_symbol == piece_symbol::queen) {
            if (abs(inf_piece_loc.first - king_coords.first) == abs(inf_piece_loc.second - king_coords.second)) {
                check_moves = true; //check moves of bishop (or queen) since they are in a position of potential check to the king
            }
        }
        else if (inf_piece_symbol == piece_symbol::rook || inf_piece_symbol == piece_symbol::queen) {
            if (inf_piece_loc.first == king_coords.first || inf_piece_loc.second == king_coords.second) {
                check_moves = true; //check moves of rook (or queen) since they are in a position of potential check to the king
            }
        }
        else {
            continue;
        }
        if (check_moves) {
            inf_allowed_moves = piece_pair.second->get_allowed_moves(board_matrix);
            for (const int& move : inf_allowed_moves) {
                if (coordinates::to_board_coordinates(move) == king_coords) {
                    return true;
                }
            }
        }
    }
    return false;
}
std::list<int> board::get_piece_allowed_moves(const std::pair<char, int>& piece_board_coords, const bool& last_move_check, const int& last_move) const {
    if (pieces_map.count(piece_board_coords) == 0) {
        throw std::out_of_range("Error: no piece to get allowed moves was found at the specified board coordinates.");
    }
    std::list<int> allowed_moves{ pieces_map.at(piece_board_coords)->get_allowed_moves(board_matrix) };
    piece_colour colour_turn{ pieces_map.at(piece_board_coords)->get_colour() };
    board_occupation current_occupation{ get_element(coordinates::flatten_board_coordinates(piece_board_coords)) };

    std::pair<char, int> original_king_coords;
    for (const auto& piece_pair : pieces_map) {
        if (piece_pair.second->get_colour() == colour_turn && piece_pair.second->get_symbol() == piece_symbol::king) {
            original_king_coords = piece_pair.second->get_location();
        }
    }

    std::array<board_occupation, 64> matrix_copy;
    std::list<int> allowed_moves_reduced;
    std::list<int> allowed_moves_to_check;
    piece_symbol piece_symbol;
    std::pair<char, int> piece_coords;
    std::pair<char, int> king_coords;
    for (const int& move : allowed_moves) {
        matrix_copy = board_matrix;
        matrix_copy.at(coordinates::flatten_board_coordinates(piece_board_coords) - 1) = board_occupation::empty;

        if (piece_board_coords == original_king_coords) {
            king_coords = coordinates::to_board_coordinates(move);
        }
        else {
            king_coords = original_king_coords;
        }
        matrix_copy.at(move - 1) = current_occupation;

        bool check_moves{ false };
        bool found_move{ false };
        bool move_is_allowed{ true };
        for (const auto& piece_pair : pieces_map) {
            piece_coords = piece_pair.second->get_location();
            //The move involves capturing this piece so don't check its moves
            if (coordinates::flatten_board_coordinates(piece_coords) == move) {
                continue;
            }
            if (found_move) {
                break;
            }
            if (piece_pair.second->get_colour() == colour_turn) {
                continue;
            }
            piece_symbol = piece_pair.second->get_symbol();
            check_moves = false;
            //if the piece of opposite colour that last moved had put the king in check, cross-check its next moves
            if (last_move_check && coordinates::flatten_board_coordinates(piece_coords) == last_move) {
                check_moves = true;
            }
            else if (piece_symbol == piece_symbol::bishop || piece_symbol == piece_symbol::queen) {
                if (abs(piece_coords.first - king_coords.first) == abs(piece_coords.second - king_coords.second)) {
                    check_moves = true; //check moves of bishop (or queen) since they are in a position of potential check to the king
                }
            }
            else if (piece_symbol == piece_symbol::rook || piece_symbol == piece_symbol::queen) {
                if (piece_coords.first == king_coords.first || piece_coords.second == king_coords.second) {
                    check_moves = true; //check moves of rook (or queen) since they are in a position of potential check to the king
                }
            }
            else {
                continue;
            }
            if (check_moves) {
                allowed_moves_to_check = piece_pair.second->get_allowed_moves(matrix_copy);
                for (const int& move_to_check : allowed_moves_to_check) {
                    if (coordinates::to_board_coordinates(move_to_check) == king_coords) {
                        move_is_allowed = false;
                        found_move = true;
                        break;
                    }
                }
            }
        }
        if (move_is_allowed) {
            allowed_moves_reduced.push_back(move);
        }
    }
    return allowed_moves_reduced;
}
void board::set_piece_location(const std::pair<char, int>& old_piece_coords, const std::pair<char, int>& new_piece_coords) {
    if (pieces_map.count(old_piece_coords) == 0){
        throw std::out_of_range("Error: no piece to set location was found at the specified board coordinates.");
    }
    pieces_map.at(old_piece_coords)->set_to_location(coordinates::flatten_board_coordinates(new_piece_coords), board_matrix);
    pieces_map.insert({ new_piece_coords, std::move(pieces_map.at(old_piece_coords)) }); //create entry for the piece at its new location
    pieces_map.erase(old_piece_coords); //delete dictionary entry at old location
}

piece_symbol board::capture_piece(const std::pair<char, int>& new_piece_coords, const piece_colour& colour_turn) {
    if (pieces_map.count(new_piece_coords) == 0) {
        throw std::out_of_range("Error: no piece to capture was found at the new board coordinates.");
    }
    if (pieces_map.at(new_piece_coords)->get_colour() == colour_turn) { //piece to capture is of the same colour as the colour turn!
        throw pieces_map.at(new_piece_coords)->get_colour();
    }
    cemetery.push_back(std::move(pieces_map.at(new_piece_coords))); //give ownership of captured piece to cemetery
    pieces_map.erase(new_piece_coords);
    return cemetery.back()->get_symbol();
}
void board::store_move(const std::pair<char, int>& new_piece_coords) {
    if (pieces_map.count(new_piece_coords) == 0) {
        throw std::out_of_range("Error: no moved piece to store move was found at the specified board coordinates.");
    }
    std::string piece_symbol_string{ pieces_map.at(new_piece_coords)->get_symbol_string(false) };

    std::ostringstream move_stringstream; //Use ostringstream to concatenate move onto a string looking like "♗ a2"
    move_stringstream << piece_symbol_string << " " << new_piece_coords.first << new_piece_coords.second;
    moves_history.push_back(move_stringstream.str());
    move_stringstream.str(""); //clear ostringstream
}
void board::show(const font& chosen_font, const piece_colour& colour_turn, const bool& check) const {
    std::cout << "   ";
    for (size_t letter_index{}; letter_index < 8; letter_index++) {
        std::cout << coordinates::alphabetical_string[letter_index] << " ";
    }
    std::cout << "\t\tMoves history" << "\t\t\tCemetery" << std::endl << " ";

    std::cout << top_left_corner;
    for (size_t i{}; i < 8; i++) {
        std::cout << board_symbols.at({ chosen_font,"line" }); //Use dictionary from "console_visualisation.h"
    }
    std::cout << board_symbols.at({ chosen_font,"small_line" }) << top_right_corner;
    std::cout << std::endl;

    size_t moves_history_index{ 1 }; //define this size_t variable to use as a counter of the moves performed
    for (size_t row{ 8 }; row > 0; row--) {
        std::cout << row << u8"││";
        for (size_t column{ 1 }; column < 9; column++) {
            bool on_white_tile{ (row + column) % 2 != 0 }; //check if piece is on a white or black tile
            auto dictionary_it{ pieces_map.find({ coordinates::numerical_dictionary.at(column),row }) };
            bool print_piece{ false };
            if (dictionary_it != pieces_map.end()){ //There is a piece in the location
                //check if found the checked king
                if (check && dictionary_it->second->get_symbol() == piece_symbol::king && dictionary_it->second->get_colour() == colour_turn) {
                    change_font_colour(on_white_tile ? font_colour::red_white_back : font_colour::red);
                    std::cout << dictionary_it->second->get_symbol_string(on_white_tile) << " ";
                    change_font_colour(font_colour::white);
                }
                else {
                    if (on_white_tile) {
                        change_font_colour(font_colour::black_white_back);
                        std::cout << dictionary_it->second->get_symbol_string(on_white_tile) << (column == 8 && chosen_font == font::NSimSun ? u8"│" : " ");
                        change_font_colour(font_colour::white);
                    }
                    else {
                        std::cout << dictionary_it->second->get_symbol_string(on_white_tile) << (column == 8 && chosen_font == font::NSimSun ? u8"│" : " ");
                    }
                }
                print_piece = true; //A piece was found and printed
            }
            if(!print_piece) { //The tile was empty
                if (on_white_tile) {
                    std::cout << board_symbols.at({ chosen_font,"white_tile" });
                }
                else {
                    if (row == 8) {
                        std::cout << board_symbols.at({ chosen_font,"top_line" });
                    }
                    else if (row == 1) {
                        std::cout << board_symbols.at({ chosen_font,"bottom_line" });
                    }
                    else {
                        std::cout << " " << (column == 8 && chosen_font == font::NSimSun ? u8"│" : " ");
                    }
                }
            }
        }
        std::cout << (chosen_font == font::MS_Gothic ? u8"▏▏" : u8"│");
        int moves_per_line{};
        while (moves_history_index <= moves_history.size()) { //print as many as 4 allowed moves in this line
            std::cout << '\t' << moves_history_index << "." << moves_history.at(moves_history_index - 1);
            moves_history_index++;
            moves_per_line++;
            if (moves_per_line == 4) {
                break;
            }
        }
        while (5 - moves_per_line > 0) { //print some space to print the cemetery next to the allowed moves
            std::cout << "\t";
            moves_per_line++;
        }
        std::cout << " ";
        for (const auto& captured_piece : cemetery) {
            //Print the captured pieces in the same row as their initial row in the board
            //This is 7 and 2 for the black and white pawns respectively, and 8 and 1 for the other type of pieces of black and white colour respectively
            if (row == 8 && captured_piece->get_symbol() != piece_symbol::pawn && captured_piece->get_colour() == piece_colour::black) {
                std::cout << captured_piece->get_symbol_string(false) << " ";
            }
            else if (row == 7 && captured_piece->get_symbol() == piece_symbol::pawn && captured_piece->get_colour() == piece_colour::black) {
                std::cout << captured_piece->get_symbol_string(false) << " ";
            }
            else if (row == 2 && captured_piece->get_symbol() == piece_symbol::pawn && captured_piece->get_colour() == piece_colour::white) {
                std::cout << captured_piece->get_symbol_string(false) << " ";
            }
            else if (row == 1 && captured_piece->get_symbol() != piece_symbol::pawn && captured_piece->get_colour() == piece_colour::white) {
                std::cout << captured_piece->get_symbol_string(false) << " ";
            }
        }
        std::cout << std::endl;
    }

    std::cout << " " << bottom_left_corner;
    for (size_t i{}; i < 8; i++) {
        std::cout << board_symbols.at({ chosen_font,"line" });
    }
    std::cout << board_symbols.at({ chosen_font,"small_line" }) << bottom_right_corner;
    std::cout << std::endl;

    std::cout << "   ";
    for (size_t letter_index{}; letter_index < 8; letter_index++) {
        std::cout << coordinates::alphabetical_string[letter_index] << " ";
    }
    std::cout << std::endl << std::endl;
}
void board::show(const font& chosen_font, const piece_colour& colour_turn, const std::pair<char, int>& selected_piece_coords, const bool& check) const {
    //Most of the code in this function is identical to the other overload above, so the appropriate comments can be found there.
    //The difference is that this function takes as argument the board coordinates of a selected piece to highlight it

    std::cout << "   ";
    for (size_t letter_index{}; letter_index < 8; letter_index++) {
        std::cout << coordinates::alphabetical_string[letter_index] << " ";
    }
    std::cout << "\t\tMoves history" << "\t\t\tCemetery" << std::endl << " ";

    std::cout << top_left_corner;
    for (size_t i{}; i < 8; i++) {
        std::cout << board_symbols.at({ chosen_font,"line" });
    }
    std::cout << board_symbols.at({ chosen_font,"small_line" }) << top_right_corner;
    std::cout << std::endl;

    int piece_index{};
    size_t moves_history_index{ 1 };
    for (size_t row{ 8 }; row > 0; row--) {
        std::cout << row << u8"││";
        for (size_t column{ 1 }; column < 9; column++) {
            bool on_white_tile{ (row + column) % 2 != 0 };
            auto dictionary_it{ pieces_map.find({ coordinates::numerical_dictionary.at(column),row }) };
            bool print_piece{ false };
            if (dictionary_it != pieces_map.end()) {
                if (dictionary_it->second->get_location() == selected_piece_coords) { //The selected piece was found
                    //The piece will be highlighted by printing it in green
                    change_font_colour(on_white_tile ? font_colour::green_white_back : font_colour::green);
                    std::cout << dictionary_it->second->get_symbol_string(on_white_tile) << " ";
                    change_font_colour(font_colour::white);
                }
                //check if found the checked king
                else if (check && dictionary_it->second->get_symbol() == piece_symbol::king && dictionary_it->second->get_colour() == colour_turn) {
                    change_font_colour(on_white_tile ? font_colour::red_white_back : font_colour::red);
                    std::cout << dictionary_it->second->get_symbol_string(on_white_tile) << " ";
                    change_font_colour(font_colour::white);
                }
                else { //There is a piece but it's not the selected one
                    if (on_white_tile) {
                        change_font_colour(font_colour::black_white_back);
                        std::cout << dictionary_it->second->get_symbol_string(on_white_tile) << (column == 8 && chosen_font == font::NSimSun ? u8"│" : " ");
                        change_font_colour(font_colour::white);
                    }
                    else {
                        std::cout << dictionary_it->second->get_symbol_string(on_white_tile) << (column == 8 && chosen_font == font::NSimSun ? u8"│" : " ");
                    }
                }
                print_piece = true;
            }
            if (!print_piece) {
                if (on_white_tile) {
                    std::cout << board_symbols.at({ chosen_font,"white_tile" });
                }
                else {
                    if (row == 8) {
                        std::cout << board_symbols.at({ chosen_font,"top_line" });
                    }
                    else if (row == 1) {
                        std::cout << board_symbols.at({ chosen_font,"bottom_line" });
                    }
                    else {
                        std::cout << " " << (column == 8 && chosen_font == font::NSimSun ? u8"│" : " ");
                    }
                }
            }
        }
        std::cout << (chosen_font == font::MS_Gothic ? u8"▏▏" : u8"│");
        int moves_per_line{};
        while (moves_history_index <= moves_history.size()) {
            std::cout << '\t' << moves_history_index << "." << moves_history.at(moves_history_index - 1);
            moves_history_index++;
            moves_per_line++;
            if (moves_per_line == 4) {
                break;
            }
        }
        while (5 - moves_per_line > 0) {
            std::cout << "\t";
            moves_per_line++;
        }
        std::cout << " ";
        for (const auto& captured_piece : cemetery) {
            if (row == 8 && captured_piece->get_symbol() != piece_symbol::pawn && captured_piece->get_colour() == piece_colour::black) {
                std::cout << captured_piece->get_symbol_string(false) << " ";
            }
            else if (row == 7 && captured_piece->get_symbol() == piece_symbol::pawn && captured_piece->get_colour() == piece_colour::black) {
                std::cout << captured_piece->get_symbol_string(false) << " ";
            }
            else if (row == 2 && captured_piece->get_symbol() == piece_symbol::pawn && captured_piece->get_colour() == piece_colour::white) {
                std::cout << captured_piece->get_symbol_string(false) << " ";
            }
            else if (row == 1 && captured_piece->get_symbol() != piece_symbol::pawn && captured_piece->get_colour() == piece_colour::white) {
                std::cout << captured_piece->get_symbol_string(false) << " ";
            }
        }
        std::cout << std::endl;
    }

    std::cout << " " << bottom_left_corner;
    for (size_t i{}; i < 8; i++) {
        std::cout << board_symbols.at({ chosen_font,"line" });
    }
    std::cout << board_symbols.at({ chosen_font,"small_line" }) << bottom_right_corner;
    std::cout << std::endl;

    std::cout << "   ";
    for (size_t letter_index{}; letter_index < 8; letter_index++) {
        std::cout << coordinates::alphabetical_string[letter_index] << " ";
    }
    std::cout << std::endl << std::endl;
}