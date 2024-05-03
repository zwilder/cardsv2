/*
* Cards
* Copyright (C) Zach Wilder 2024
* 
* This file is a part of Cards
*
* Cards is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* Cards is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with Cards.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <cards.h>

int main(int argc, char **argv) {
    init_genrand(time(NULL)); // Seed the prng
    term_init(); // Initialize the terminal
    init_screenbuf(); // Initialize the global screen buffer
    clear_screen(g_screenbuf); // Clear the screenbuf
    draw_screen(g_screenbuf); // Draw the screenbuf
    init_settings(); // Create global settings
    load_settings(); // Load settings/high scores

    main_menu(); // Open the main menu

    save_settings(); // Save settings/high scores
    close_settings(); // Free global settings
    close_screenbuf(); // Close the global screen buffer
    term_close(); // Reset the terminal
    return 0;
}
