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
    term_init(); // Initialize the terminal
    init_screenbuf(); // Initialize the global screen buffer
    init_genrand(time(NULL)); // Seed the pnrg
    init_settings(); // Create global settings
    load_game(); // Load settings/high scores

    clear_screen(g_screenbuf); // Clear the screenbuf
    draw_screen(g_screenbuf); // Draw the screen buf
    scr_clear();
    pt_card_title((g_screenW / 2) - 12, 
            (g_screenH / 2) - 2,
            "Cards!");
    scr_pt_clr((g_screenW / 2) - 40,
            (g_screenH / 2) + 11,
            BRIGHT_BLACK, BLACK,
            "\u00A9 2024 - Zach Wilder");
    scr_reset();
    kb_get_bl_char();
    klondike_init();
    
    save_game(); // Save settings/high scores
    close_settings(); // Free global settings
    close_screenbuf(); // Close the global screen buffer
    term_close(); // Reset the terminal
    return 0;
}
