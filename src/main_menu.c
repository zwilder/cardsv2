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

void main_menu(void) {
    int xo = (g_screenW / 2) - (SCREEN_WIDTH / 2);
    int yo = (g_screenH / 2) - (SCREEN_HEIGHT / 2);
    char ch = '\0';
    while(ch != 'q') {
        scr_clear(); // Clear everything off the terminal screen
        fill_screen_blank(g_screenbuf); // Fill the screenbuf with blank characters (transparent)
        pt_card_title((SCREEN_WIDTH/2)-12+xo,1+yo,"Cards!");
        scr_pt_clr((SCREEN_WIDTH / 2)-9+xo,(SCREEN_HEIGHT - 1)+yo,
                BRIGHT_BLACK,BLACK, "\u00A9 Zach Wilder 2024");
        // 40-20+66 = 86
        draw_str((SCREEN_WIDTH/2)-20,8,
                "[k] - Klondike        [p] - Penguin");
        draw_str((SCREEN_WIDTH/2)-20,9,
                "[f] - Freecell        [c] - Cribbage");
        draw_str((SCREEN_WIDTH/2)-20,11,
                "[h] - High Scores     [o] - Card Settings");
        draw_str((SCREEN_WIDTH/2)-20,12,
                "[?] - Help            [q] - Quit");
        draw_screen(g_screenbuf);
        ch = kb_get_bl_char();
        switch(ch) {
            case 'k':
                if(!klondike_init()) ch = 'q';
                break;
            case 'p':
                if(!penguin_init()) ch = 'q';
                break;
            case 'h':
                high_scores();
                break;
            case 'o':
                settings_menu();
                break;
            case '?':
                //help_menu();
                break;
            case 'q':
                break;
            default: break;
        }
    }
    clear_screen(g_screenbuf);
    //destroy_slist(&menu);
}
