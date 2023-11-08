/*
This source file includes the implementation of the function to change the colour of the font. 
It uses SetConsoleTextAttribute() from Windows.h
*/

#include "console_visualisation.h"

void change_font_colour(const font_colour& colour) {
    int colour_code{ static_cast<int>(colour) };
    //SetConsoleTextAttribute(hConsole, colour_code);
}