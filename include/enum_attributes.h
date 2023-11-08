/*
This header file defines enumerated classes to be used as attributes of the pieces (colour and symbol) and of the board (occupation).
There are also two constant maps, one to obtain the opposite piece_colour and another to obtain a string of the colour given a piece_colour object.
*/

#ifndef ENUM_ATTRIBUTES_H
#define ENUM_ATTRIBUTES_H

#include <map>
#include <iostream>

enum class piece_colour : int {
	white, //0
	black, //1
};

enum class piece_symbol : int {
	pawn, //0
	rook, //1
	knight, //2
	bishop, //3
	king, //4
	queen, //5
};

enum class board_occupation : int {
	empty = -1, //-1
	white_piece, //0
	black_piece, //1
};

const std::map<piece_colour, piece_colour> opposite_colour{ {piece_colour::white,piece_colour::black},
															{piece_colour::black, piece_colour::white} };
const std::map<piece_colour, std::string> colour_string_map{ {piece_colour::white,"white"},
															 {piece_colour::black,"black"} };

#endif
