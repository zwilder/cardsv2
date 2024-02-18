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

void main_menu(void);

int main(int argc, char **argv) {
    init_genrand(time(NULL)); // Seed the pnrg
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

void main_menu(void) {
    int xo = (g_screenW / 2) - (SCREEN_WIDTH / 2);
    int yo = (g_screenH / 2) - (SCREEN_HEIGHT / 2);
    char ch = '\0';
    SList *menu = create_slist("-Cards-");
    slist_push(&menu, " ");
    slist_push(&menu, "khoq");
    slist_push(&menu, "New game of Klondike");
    slist_push(&menu, "High scores");
    slist_push(&menu, "Card color settings");
    slist_push(&menu, "Quit");

    while(ch != 'q') {
        scr_clear(); // Clear everything off the terminal screen
        pt_card_title((SCREEN_WIDTH / 2)-16+xo, yo, "Cards!");
        scr_pt_clr((SCREEN_WIDTH / 2)-9+xo,(SCREEN_HEIGHT - 1)+yo,
                BRIGHT_BLACK,BLACK, "\u00A9 Zach Wilder 2024");
        fill_screen_blank(g_screenbuf); // Fill the screenbuf with blank characters (transparent)
        ch = draw_menu_nobox(menu, WHITE, BLACK);
        switch(ch) {
            case 'k':
                klondike_init();
                // For now, quit when leaving klondike.
                // I may, in the future, have init return a bool or something -
                // like if I add a "return to main menu" screen in the Klondike
                // menu
                ch = 'q';
                break;
            case 'h':
                //high_scores();
                break;
            case 'o':
                settings_menu();
                break;
            case 'q':
                break;
            default: break;
        }
    }
    clear_screen(g_screenbuf);
    destroy_slist(&menu);
}

