/*
This folder defines classes, functions and constants which aid in the consolue visualisation of the game.
The two fonts that support Unicode symbols are NSimSun and MS_Gothic, and depending on which one is used some symbols
print differently. Therefore, a dictionary is created that takes in a pair of font class object a string indicating the
desired unicode symbol, and returns the corresponding symbol. This is used in the main.cpp file for output.
Symbols which do not change depending on font are also defined as constants.
*/

#ifndef CONSOLE_VISUALISATION_H
#define CONSOLE_VISUALISATION_H

#include <Windows.h>
#include <map>
#include <string>

enum class font {
    NSimSun,
    MS_Gothic,
};

enum class font_colour {
    green = 10,
    green_white_back = 250,
    red = 12,
    red_white_back = 252,
    white = 15,
    black_white_back = 240
};

namespace {
    const std::map<std::pair<font, std::string>, std::string> create_board_symbols_dictionary() {

        std::map<std::pair<font, std::string>, std::string> symbols_dictionary;

        symbols_dictionary.insert({ {font::NSimSun,"white_tile"},u8"█" });
        symbols_dictionary.insert({ {font::MS_Gothic, "white_tile"},u8"██" });

        symbols_dictionary.insert({ {font::NSimSun,"line"},u8"—" });
        symbols_dictionary.insert({ {font::MS_Gothic,"line"},u8"——" });

        symbols_dictionary.insert({ {font::NSimSun,"small_line"},u8"-" });
        symbols_dictionary.insert({ {font::MS_Gothic,"small_line"},u8"—" });

        symbols_dictionary.insert({ {font::NSimSun,"top_line"},u8"▔" });
        symbols_dictionary.insert({ {font::MS_Gothic,"top_line"},u8"▔▔" });

        symbols_dictionary.insert({ {font::NSimSun,"bottom_line"},u8"▁" });
        symbols_dictionary.insert({ {font::MS_Gothic,"bottom_line"},u8"▁▁" });

        return symbols_dictionary;
    }
}

const std::map<std::pair<font, std::string>, std::string> board_symbols{ create_board_symbols_dictionary() };

constexpr std::string_view top_left_corner{ u8"┌" };
constexpr std::string_view top_right_corner{ u8"┐" };
constexpr std::string_view bottom_left_corner{ u8"└" };
constexpr std::string_view bottom_right_corner{ u8"┘" };

void change_font_colour(const font_colour& colour);

#endif