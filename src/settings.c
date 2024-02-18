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

/*
//Regular
g_settings->redcolor = BRIGHT_RED;
g_settings->blackcolor = WHITE;
g_settings->deckcolor = BRIGHT_BLACK;
g_settings->bgcolor = BLACK;
g_settings->btnselectcolor = CYAN;
g_settings->btncolor = WHITE;
//Vapor
g_settings->redcolor = BRIGHT_MAGENTA;
g_settings->blackcolor = BRIGHT_CYAN;
g_settings->deckcolor = BRIGHT_BLACK;
g_settings->bgcolor = BLACK;
g_settings->btnselectcolor = CYAN;
g_settings->btncolor = WHITE;
*/
Settings *g_settings = NULL;

void init_settings(void) {
    if(g_settings) close_settings();
    g_settings = malloc(sizeof(Settings));
}

void close_settings(void) {
    if(!g_settings) return;
    free(g_settings);
}

void settings_menu(void) {
    int xo = (g_screenW / 2) - (SCREEN_WIDTH / 2);
    int yo = (g_screenH / 2) - (SCREEN_HEIGHT / 2);
    char ch = '\0';
    SList *menu = create_slist("-Color settings-");
    slist_push(&menu, "Press [q] to return");
    slist_push(&menu, "abcdefgq");
    slist_push(&menu, "Change red color");
    slist_push(&menu, "Change black color");
    slist_push(&menu, "Change deck color");
    slist_push(&menu, "Change card background color");
    slist_push(&menu, "Change button selected color");
    slist_push(&menu, "Change button text color");
    slist_push(&menu, "Return settings to default");
    while(ch != 'q') {
        scr_clear(); // Clear everything off the terminal screen
        pt_card_title((SCREEN_WIDTH / 2)-20+xo, yo, "Settings");
        fill_screen_blank(g_screenbuf); // Fill the screenbuf with blank characters (transparent)
        ch = draw_menu_nobox(menu, WHITE, BLACK);
        switch(ch) {
            case 'a': 
                g_settings->redcolor = color_picker_menu(g_settings->redcolor,
                        "Choose a new red color");
                break;
            case 'b':
                g_settings->blackcolor = color_picker_menu(g_settings->blackcolor,
                        "Choose a new black color");
                break;
            case 'c':
                g_settings->deckcolor = color_picker_menu(g_settings->deckcolor,
                        "Choose a new deck color");
                break;
            case 'd':
                g_settings->bgcolor = color_picker_menu(g_settings->bgcolor,
                        "Choose a new card background color");
                break;
            case 'e':
                g_settings->btnselectcolor = color_picker_menu(g_settings->btnselectcolor,
                        "Choose a new selected button color");
                break;
            case 'f': 
                g_settings->btncolor = color_picker_menu(g_settings->btncolor,
                        "Choose a new button text color");
                break;
            case 'g':
                g_settings->redcolor = BRIGHT_MAGENTA;
                g_settings->blackcolor = BRIGHT_CYAN;
                g_settings->deckcolor = BRIGHT_BLACK;
                g_settings->bgcolor = BLACK;
                g_settings->btnselectcolor = CYAN;
                g_settings->btncolor = WHITE;
                break;
            case 'q': break;
            default: break;
        }
    }
    clear_screen(g_screenbuf);
    destroy_slist(&menu);
}
