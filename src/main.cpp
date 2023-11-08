/*
Luis Fernandez - 30 April 2020
The operative system used is Windows. The code was written for C++17.

This code correctly implements: 
    -The allowed moves for all the chess pieces
    -Piece captures
    -Check, checkmate and stalemate
    -History of moves and a cemetery
The user is allowed to:
    -Get all the allowed moves for the pieces of a certain colour
    -Select a piece (which gets highlighted in the board) and:
        -Get its allowed moves
        -Move it
        -Change piece selection
*/

#include "piece.h"
#include "pawn.h"
#include "rook.h"
#include "knight.h"
#include "bishop.h"
#include "king.h"
#include "queen.h"
#include "board.h"
#include "coordinate_transforms.h"
#include "enum_attributes.h"
#include "console_visualisation.h"
#include <iostream>
#include <utility>
#include <vector>
#include <list>
#include <array>
#include <map>
#include <functional>
#include <exception>

using board_coordinates_t = std::pair<char, int>;

int main()
{
    change_font_colour(font_colour::white);
    std::cout << "Welcome to this console CHESS game. This program uses Unicode symbols for better visualisation. "
        << "Please change the console font to NSimSun or MS Gothic before playing. This can be done by clicking on the top left corner of the console "
        << "window. For best results, please choose MS Gothic and then maximise the console window for the duration of the game."
        << "After changing the font, please indicate which one you chose - enter N for NSimSun or G for MS Gothic: ";
    font font_choice;
    std::string font_choice_string;
    bool valid_font_choice{ false };
    while (!valid_font_choice) {
        std::getline(std::cin, font_choice_string);
        if (font_choice_string.size() > 1 || font_choice_string.size() == 0) {
            std::cout << "Invalid choice. Please enter [N]SimSun or MS [G]othic: ";
        }
        else if (font_choice_string == "N" || font_choice_string == "n") {
            font_choice = font::NSimSun;
            std::cout << "Thank you, you chose NSimSun." << std::endl;
            valid_font_choice = true;
        }
        else if (font_choice_string == "G" || font_choice_string == "g") {
            font_choice = font::MS_Gothic;
            std::cout << "Thank you, you chose MS Gothic." << std::endl;
            valid_font_choice = true;
        }
        else {
            std::cout << "Invalid choice, please enter N or G: ";
        }
    }
    std::unique_ptr<board> gameboard;
    try {
        gameboard = std::make_unique<board>();
    }
    catch (const std::exception& board_construct_err) {
        std::cerr << board_construct_err.what() << std::endl;
        return 1; //End program if could not construct board
    }
    piece_colour colour_turn{ piece_colour::white };
    std::list<std::string> all_allowed_moves;
      
    bool game_over{ false };
    bool check{ false };
    bool last_move_check{ false };
    int last_move{};
    while (!game_over) { //Loop over turns
        system("cls");
        gameboard->show(font_choice, colour_turn, check);
        try {
            all_allowed_moves = gameboard->get_all_pieces_allowed_moves(colour_turn, last_move_check, last_move);
        }
        catch (const std::exception& all_moves_err) {
            std::cerr << all_moves_err.what() << std::endl;
            return 1;
        }
        if (all_allowed_moves.size() == 0) {
            game_over = true;
            if (check) {
                change_font_colour(font_colour::red);
                std::cout << "CHECKMATE! ";
                change_font_colour(font_colour::white);
                std::cout << "Team of colour "
                    << colour_string_map.at(opposite_colour.at(colour_turn));
                change_font_colour(font_colour::green);
                std::cout << " WINS";
                change_font_colour(font_colour::white);
                std::cout << "!" << std::endl;
            }
            else {
                std::cout << "Stalemate! Team of colour " << colour_string_map.at(colour_turn) << " cannot move! The game ends in a draw!" << std::endl;
            }
            continue;
        }
        if (check) {
            std::cout << "King of colour " << colour_string_map.at(colour_turn) << " is in ";
            change_font_colour(font_colour::red);
            std::cout << "CHECK";
            change_font_colour(font_colour::white);
            std::cout << "!" << std::endl;
        }
        bool valid_input{ false };
        bool got_all_allowed_moves{ false };
        std::cout << "It is " << colour_string_map.at(colour_turn) << "'s turn! Please enter A to get all the allowed moves "
            << "or enter the board coordinates (e.g. a2) of a piece: ";
        while (!valid_input) {
            std::string input_string;
            std::getline(std::cin, input_string);
            if (input_string.size() > 2 || input_string.size() == 0) {
                std::cerr << "Invalid input. Please try again: ";
                continue;
            }
            if (!got_all_allowed_moves && input_string.size() == 1) {
                if (std::toupper(input_string[0]) == 'A') {
                    system("cls"); //clear the console 
                    gameboard->show(font_choice, colour_turn, check);
                    if (check) {
                        std::cout << "King of colour " << colour_string_map.at(colour_turn) << " is in ";
                        change_font_colour(font_colour::red);
                        std::cout << "CHECK";
                        change_font_colour(font_colour::white);
                        std::cout << "!" << std::endl;
                    }
                    std::cout << "These are all the allowed moves for the " << colour_string_map.at(colour_turn) << " pieces: " << std::endl;
                    for (const auto& move : all_allowed_moves) {
                        std::cout << move << '\t';
                    }
                    got_all_allowed_moves = true; //all the allowed moves were obtained so the only option now is to select a piece
                    std::cout << std::endl << "Please now enter the board coordinates of a piece: ";
                }
                else {
                    std::cerr << "Invalid letter input. To get all the allowed moves, please enter the letter A: ";
                }
                continue;
            }
            if (got_all_allowed_moves && (input_string == "A" || input_string == "a")) {
                std::cerr << "You already asked for all the allowed moves! Please enter the board coordinates of a piece: ";
                continue;
            }
            if (input_string.size() == 1 || isdigit(input_string[0]) || !isdigit(input_string[1])) {
                std::cerr << "Please enter valid board coordinates (character followed by integer): ";
                continue;
            }
            //Reached here, we know the input was a character followed by a number (i.e. board coordinates)
            char old_column{ static_cast<char>(std::tolower(input_string[0])) }; //static cast since std::tolower returns an int
            int old_row{ static_cast<int>(input_string[1]) - 48 }; //In ASCII, digits start at 48
            board_coordinates_t old_piece_coords{ old_column,old_row };
            board_occupation gameboard_occupation_old_location;
            int old_location_int{};
            try {
                old_location_int = coordinates::flatten_board_coordinates(old_piece_coords);
                gameboard_occupation_old_location = gameboard->get_element(old_location_int);
            }
            catch (const std::out_of_range& coords_err) {
                std::cerr << coords_err.what() << " Please try again: ";
                continue;
            }
            if (gameboard_occupation_old_location == board_occupation::empty) {
                std::cerr << "There is no piece at those coordinates! Please try again: ";
                continue;
            }
            if (static_cast<int>(gameboard_occupation_old_location) != static_cast<int>(colour_turn)) {
                std::cerr << "It is " << colour_string_map.at(colour_turn) << "'s turn! Please enter the board coordinates of a piece of the right colour: ";
                continue;
            }
            std::list<int> allowed_moves;
            try {
                allowed_moves = gameboard->get_piece_allowed_moves(old_piece_coords, last_move_check, last_move);
            }
            catch (const std::exception& piece_moves_err) {
                std::cerr << piece_moves_err.what() << std::endl;
                return 1;
            }
            if (allowed_moves.size() == 0) {
                std::cout << "The chosen piece cannot be moved! Please select a different one: ";
                continue;
            }
            valid_input = true;
            system("cls");
            gameboard->show(font_choice, colour_turn, old_piece_coords, check);
            if (check) {
                std::cout << "King of colour " << colour_string_map.at(colour_turn) << " is in ";
                change_font_colour(font_colour::red);
                std::cout << "CHECK";
                change_font_colour(font_colour::white);
                std::cout << "!" << std::endl;
            }
            bool valid_choice{ false };
            bool got_allowed_moves{ false };
            std::cout << "You selected the piece at " << old_piece_coords.first << old_piece_coords.second << 
                ". Enter new board coordinates to move the selected piece, G to get its allowed moves, or C to change piece: ";
            while (!valid_choice) {
                std::string choice_string;
                std::getline(std::cin, choice_string);
                if (choice_string.size() > 2 || choice_string.size() == 0) {
                    std::cerr << "Invalid input. Please try again: ";
                    continue;
                }
                if (choice_string.size() == 1) {
                    if (!got_allowed_moves && std::toupper(choice_string[0]) == 'G') {
                        system("cls");
                        gameboard->show(font_choice, colour_turn, old_piece_coords, check);
                        if (check) {
                            std::cout << "King of colour " << colour_string_map.at(colour_turn) << " is in ";
                            change_font_colour(font_colour::red);
                            std::cout << "CHECK";
                            change_font_colour(font_colour::white);
                            std::cout << "!" << std::endl;
                        }
                        std::list<board_coordinates_t> allowed_board_coords;
                        //Loop over allowed moves transforming them from int to board coordinates
                        std::transform(allowed_moves.begin(), allowed_moves.end(), std::back_inserter(allowed_board_coords),
                            [](const int& location) -> board_coordinates_t {
                                return coordinates::to_board_coordinates(location);
                            });
                        allowed_board_coords.sort();
                        std::cout << "These are the allowed coordinates the piece at " << old_piece_coords.first << old_piece_coords.second << " can move to: " << std::endl;
                        for (const auto& coordinates : allowed_board_coords) {
                            std::cout << coordinates.first << coordinates.second << '\t';
                        }
                        got_allowed_moves = true;
                        std::cout << std::endl << "Please now enter new board coordinates to move the piece, or C to change piece: ";
                    }
                    else if (got_allowed_moves && std::toupper(choice_string[0]) == 'G') {
                        std::cout << "You already asked for the allowed moves! Please enter the new board coordinates, or C to change piece: ";
                    }
                    else if (std::toupper(choice_string[0]) == 'C') {
                        system("cls"); //clear the console 
                        gameboard->show(font_choice, colour_turn, check);
                        if (check) {
                            std::cout << "King of colour " << colour_string_map.at(colour_turn) << " is in ";
                            change_font_colour(font_colour::red);
                            std::cout << "CHECK";
                            change_font_colour(font_colour::white);
                            std::cout << "!" << std::endl;
                        }
                        std::cout << "Enter the board coordinates of the new piece you would like to select, or A to get all the piece's allowed moves: ";
                        got_all_allowed_moves = false;
                        valid_choice = true;
                        valid_input = false; //to go back to start of previous while loop and select a new piece
                    }
                    else {
                        std::cerr << "Invalid letter input. To change piece, enter C" <<
                            (got_allowed_moves ? ": " : " or to get the allowed moves, enter G: ");
                    }
                    continue;
                }
                if (isdigit(choice_string[0]) || !isdigit(choice_string[1])) {
                    std::cerr << "Please enter valid board coordinates (character followed by integer): ";
                    continue;
                }
                char new_column{ static_cast<char>(std::tolower(choice_string[0])) };
                int new_row{ static_cast<int>(choice_string[1]) - 48 }; //In ASCII, digits start at 48
                board_coordinates_t new_piece_coords{ new_column,new_row };
                int new_location_int;
                try {
                    new_location_int = coordinates::flatten_board_coordinates(new_piece_coords);
                }
                catch (const std::out_of_range& new_coords_err) {
                    std::cerr << new_coords_err.what() << " Please try again: ";
                    continue;
                }
                //Check if new location is an allowed move
                auto it_moves = std::find_if(allowed_moves.begin(), allowed_moves.end(),
                    [&new_location_int](const int& move) {
                        return move == new_location_int;
                    });
                if (it_moves == allowed_moves.end()) {
                    std::cerr << "Could not set to specified location, move was not allowed. Please try again: ";
                    continue;
                }
                board_occupation gameboard_occupation_new_location;
                try {
                    gameboard_occupation_new_location = gameboard->get_element(new_location_int);
                    if (static_cast<int>(gameboard_occupation_new_location) == static_cast<int>(opposite_colour.at(colour_turn))) {
                        piece_symbol captured_piece_symbol{ gameboard->capture_piece(new_piece_coords, colour_turn) };
                        if (captured_piece_symbol == piece_symbol::king) {
                            game_over = true;
                            valid_input = true;
                            valid_choice = true;
                        }
                    }
                    gameboard->set_piece_location(old_piece_coords, new_piece_coords);
                    gameboard->set_element(old_location_int) = board_occupation::empty;
                    gameboard->set_element(new_location_int) = gameboard_occupation_old_location;
                    gameboard->store_move(new_piece_coords);
                }
                catch (const std::invalid_argument& not_allowed_move_err) {
                    std::cerr << not_allowed_move_err.what() << " Please choose another location: ";
                    continue;
                }
                catch (const piece_colour&) {
                    std::cerr << "Error: the colour of the piece to capture was the same as the colour turn!" << std::endl;
                    return 1;
                }
                catch (const std::exception& setting_location_err) {
                    std::cerr << setting_location_err.what() << " Exiting.." << std::endl;
                    return 1;
                }
                valid_choice = true;
                if (!game_over) {
                    try {
                        check = gameboard->check_for_check(new_piece_coords, last_move_check);
                        last_move = coordinates::flatten_board_coordinates(new_piece_coords);
                    }
                    catch (const std::exception & err) {
                        std::cerr << err.what() << std::endl;
                        return 1;
                    }
                }
            }
        }
        colour_turn = opposite_colour.at(colour_turn);
    }
    std::cout << "Thanks for playing!" << std::endl;
    return 0;
}
