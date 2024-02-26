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
    g_settings->redcolor = BRIGHT_MAGENTA;
    g_settings->blackcolor = BRIGHT_CYAN;
    g_settings->deckcolor = BRIGHT_BLACK;
    g_settings->deckdesign = 0;
    g_settings->bgcolor = BLACK;
    g_settings->btnselectcolor = CYAN;
    g_settings->btncolor = WHITE;
    g_settings->klondike_hs = 0;
    g_settings->klondike_last = 0;
    g_settings->klondike_wins = 0;
    g_settings->penguin_hs = 0;
    g_settings->penguin_last = 0;
    g_settings->penguin_wins = 0;
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
    slist_push(&menu, "abcdehq");
    slist_push(&menu, "Change red color");
    slist_push(&menu, "Change black color");
    slist_push(&menu, "Change deck color");
    slist_push(&menu, "Change deck back design");
    slist_push(&menu, "Change card background color");
    slist_push(&menu, "Return settings to default");
    while(ch != 'q') {
        scr_clear(); // Clear everything off the terminal screen
        pt_card_title((SCREEN_WIDTH / 2)-16+xo, yo+1, "Settings");
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
                deck_design_menu();
                break;
            case 'e':
                g_settings->bgcolor = color_picker_menu(g_settings->bgcolor,
                        "Choose a new card background color");
                break;
                /*
            case 'f':
                g_settings->btnselectcolor = color_picker_menu(g_settings->btnselectcolor,
                        "Choose a new selected button color");
                break;
            case 'g': 
                g_settings->btncolor = color_picker_menu(g_settings->btncolor,
                        "Choose a new button text color");
                break;
                */
            case 'h':
                g_settings->redcolor = BRIGHT_MAGENTA;
                g_settings->blackcolor = BRIGHT_CYAN;
                g_settings->deckcolor = BRIGHT_BLACK;
                g_settings->deckdesign = 0;
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

void deck_design_menu(void) {
    int xo = (g_screenW / 2) - (SCREEN_WIDTH / 2);
    int yo = (g_screenH / 2) - (SCREEN_HEIGHT / 2);
    char ch = '\0';
    int i = 0, x = 0, y = 0;
    int ogback = g_settings->deckdesign;
    // Clear the screen
    scr_clear(); 
    fill_screen_blank(g_screenbuf);

    // Draw the title
    pt_card_title((SCREEN_WIDTH / 2)-16+xo, yo+1, "Settings");

    // Draw the options
    /*    ╔══╗
     *[a] ║▚▞║
     *    ║▞▚║ 
     *    ╚══╝
     */
    for(i = 0; i < 5; i++) {
        x = 17 + (i*9);
        y = 8;
        g_settings->deckdesign = i; // hacky, but it works
        pt_card_back(x+4+xo, y+yo);
        scr_pt_clr(x+xo,y+yo+1, WHITE, BLACK, "[%c]", ('a'+i));
    }
    y = 13;
    x = 17;
    g_settings->deckdesign = 5;
    pt_card_back(x+4+xo, y+yo);
    scr_pt_clr(x+xo,y+yo+1, WHITE, BLACK, "[%c]", ('a'+5));

    // Draw the instructions
    scr_pt_clr(xo,SCREEN_HEIGHT+yo-2,WHITE,BLACK,
            "Select a new deck design - press any other key to return");

    // Draw the screen
    draw_screen(g_screenbuf);

    // Wait for a keypress
    ch = kb_get_bl_char();

    // Change the design
    switch(ch) {
        case 'a': g_settings->deckdesign = 0; break;
        case 'b': g_settings->deckdesign = 1; break;
        case 'c': g_settings->deckdesign = 2; break;
        case 'd': g_settings->deckdesign = 3; break;
        case 'e': g_settings->deckdesign = 4; break;
        case 'f': g_settings->deckdesign = 5; break;
        default: g_settings->deckdesign = ogback;
    }
}
