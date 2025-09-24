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

#include <sys/time.h>
#include <cards.h>

Solitaire *g_klondike = NULL;

bool klondike_init(void) {
    /*
     * Initialize a solitaire game of Klondike, and return a bool indicating if
     * the user wants to return to the main menu, or quit the game entirely.
     */
    int i = 0;
    bool ret_to_main = false;
    if(g_klondike) klondike_cleanup();

    // Allocate memory for decks/buttons
    g_klondike = create_solitaire(KL_NUM_DECKS);

    // Register events/update/draw
    g_klondike->events = &klondike_events;
    g_klondike->update = &klondike_update;
    g_klondike->draw = &klondike_draw;

    // Put the buttons in the right spot
    g_klondike->btns[KL_STOCK]->x = 3;
    g_klondike->btns[KL_STOCK]->active = true;
    g_klondike->btns[KL_WASTE]->x = 8;
    
    for(i = 0; i < 7; i++) {
        g_klondike->btns[KL_TAB_B + i]->x = 22 + (5*i);
        g_klondike->btns[KL_TAB_B + i]->active = true;
    }
    
    for(i = 0; i < 4; i++) {
        g_klondike->btns[KL_FND_H + i]->x = 61 + (5*i);
        g_klondike->btns[KL_FND_H + i]->active = true;
        g_klondike->btns[KL_FND_H + i]->ch = '1' + i;
    }

    // Put 52 cards in the stock, and shuffle it
    fill_deck(g_klondike->decks[KL_STOCK]);
    shuffle_deck(g_klondike->decks[KL_STOCK]);

    // Deal the cards, draw the cards, enter the loop
    klondike_deal();
    klondike_update();
    klondike_draw();
    klondike_loop();

    // If the game is going to be restarted, do it here
    if(check_flag(g_klondike->flags,GFL_RESTART)) {
        ret_to_main = klondike_init();
    } else {
        ret_to_main = check_flag(g_klondike->flags, GFL_QTOMAIN);
    }

    // Cleanup
    klondike_cleanup();
    return ret_to_main;
}

void klondike_cleanup(void) {
    if(!g_klondike) return;
    destroy_solitaire(g_klondike);
    g_klondike = NULL;
}

void klondike_deal(void) {
    int i = 0, j = 0;
    Card *tmp = NULL;
    for(i = 0; i < 7; i++) {
        j = i + 1;
        // Draw cards from the deck to put on the tableau
        draw_cards(g_klondike->decks[KL_STOCK],g_klondike->decks[KL_TAB_B + i], j);

        // Turn the top card (last card) faceup
        tmp = g_klondike->decks[KL_TAB_B + i]->cards;
        while(tmp->next) {
            tmp = tmp->next;
        }
        engage_flag(&(tmp->flags), CD_UP);
    }
}

void klondike_loop(void) {
    // Main loop
    solitaire_loop(g_klondike);
    // Score check here... If score is new high score, save it
    if(g_klondike->score > g_settings->klondike_hs) {
        g_settings->klondike_hs = g_klondike->score;
    }
    if(g_klondike->score) {
        g_settings->klondike_last = g_klondike->score;
        add_new_score_chrono(g_settings->klondike_scores,
                g_klondike->score);
    }
    save_settings();
}

void klondike_events(void) {
    bool redraw = false;
    char ch = kb_get_char();
    switch(ch) {
        case 'A':
        case 'a': toggle_button(g_klondike->btns[KL_WASTE]); 
                  redraw = true; 
                  break;
        case 'B':
        case 'b': toggle_button(g_klondike->btns[KL_TAB_B]);
                  redraw = true;
                  break;
        case 'C':
        case 'c': toggle_button(g_klondike->btns[KL_TAB_C]);
                  redraw = true;
                  break;
        case 'D':
        case 'd': toggle_button(g_klondike->btns[KL_TAB_D]);
                  redraw = true;
                  break;
        case 'E':
        case 'e': toggle_button(g_klondike->btns[KL_TAB_E]);
                  redraw = true;
                  break;
        case 'F':
        case 'f': toggle_button(g_klondike->btns[KL_TAB_F]);
                  redraw = true;
                  break;
        case 'G':
        case 'g': toggle_button(g_klondike->btns[KL_TAB_G]);
                  redraw = true;
                  break;
        case 'H':
        case 'h': toggle_button(g_klondike->btns[KL_TAB_H]);
                  redraw = true;
                  break;
        case '1': toggle_button(g_klondike->btns[KL_FND_H]);
                  redraw = true;
                  break;
        case '2': toggle_button(g_klondike->btns[KL_FND_D]);
                  redraw = true;
                  break;
        case '3': toggle_button(g_klondike->btns[KL_FND_C]);
                  redraw = true;
                  break;
        case '4': toggle_button(g_klondike->btns[KL_FND_S]);
                  redraw = true;
                  break;
        case 'M':
        case 'm': toggle_button(g_klondike->btns[KL_STOCK]);
                  redraw = true;
                  break;
        case 27: 
        case 'q':
                  solitaire_pause(g_klondike);
                  redraw = true; 
                  break;
        default: break;
    }
    if(redraw) {
        g_klondike->flags |= GFL_DRAW;
    }
}

void klondike_pause(void) {
    int xo = (g_screenW / 2) - (SCREEN_WIDTH / 2);
    int yo = (g_screenH / 2) - (SCREEN_HEIGHT / 2);
    char ch = '\0';

    // Create the slist for the menu
    SList *menu = create_slist("-Game Paused-");
    slist_push(&menu, "Press any other key to resume");
    slist_push(&menu, "cnmq");
    slist_push(&menu, "Change card color settings");
    slist_push(&menu, "Start new game");
    slist_push(&menu, "Quit to main menu");
    slist_push(&menu, "Quit game");

    // Show the menu
    scr_clear(); // Clear everything off the terminal screen
    pt_card_title((SCREEN_WIDTH / 2)-10+xo, yo+1, "Pause");
    fill_screen_blank(g_screenbuf); // Fill the screenbuf with blank characters (transparent)
    ch = draw_menu_nobox(menu, WHITE, BLACK);

    // Process the input
    switch(ch) {
        case 'c':
            settings_menu();
            break;
        case 'n':
            g_klondike->flags |= GFL_RESTART;
            break;
        case 'm':
            g_klondike->flags |= GFL_QTOMAIN;
            g_klondike->flags &= ~GFL_RUNNING;
            break;
        case 'q':
            g_klondike->flags &= ~GFL_RUNNING;
            break;
        default: break;
    }

    // Cleanup
    clear_screen(g_screenbuf);
    destroy_slist(&menu);
}
