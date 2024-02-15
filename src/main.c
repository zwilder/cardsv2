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

Settings *g_settings;
int main(int argc, char **argv) {
    term_init(); // Initialize the terminal
    init_genrand(time(NULL)); // Seed the pnrg
    g_settings = malloc(sizeof(Settings));
    /*
    //Regular
    g_settings->redcolor = BRIGHT_RED;
    g_settings->blackcolor = WHITE;
    g_settings->deckcolor = BRIGHT_BLACK;
    g_settings->bgcolor = BLACK;
    g_settings->btnselectcolor = CYAN;
    g_settings->btncolor = WHITE;
    */
    //Vapor
    g_settings->redcolor = BRIGHT_MAGENTA;
    g_settings->blackcolor = BRIGHT_CYAN;
    g_settings->deckcolor = BRIGHT_BLACK;
    g_settings->bgcolor = BLACK;
    g_settings->btnselectcolor = CYAN;
    g_settings->btncolor = WHITE;
    load_game();

    scr_clear();
    pt_card_title((g_screenW / 2) - 12, 
            (g_screenH / 2) - 2,
            "Cards!");
    scr_pt_clr((g_screenW / 2) - 40,
            (g_screenH / 2) + 12,
            BRIGHT_BLACK, BLACK,
            "\u00A9 2024 - Zach Wilder");
    scr_reset();
    kb_get_bl_char();
    klondike_init();

    free(g_settings);
    term_close(); // Reset the terminal
    return 0;
}
