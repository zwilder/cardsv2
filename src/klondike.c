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

void klondike_init(void) {
    int i = 0;
    if(g_klondike) klondike_cleanup();

    // Allocate memory for decks/buttons
    g_klondike = create_solitaire(KL_NUM_DECKS);

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
    }

    // Put 52 cards in the stock, and shuffle it
    fill_deck(g_klondike->decks[KL_STOCK]);
    shuffle_deck(g_klondike->decks[KL_STOCK]);

    // Deal the cards, draw the cards, enter the loop
    klondike_deal();
    klondike_update();
    klondike_draw();
    klondike_loop();

    // Cleanup
    klondike_cleanup();
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

long current_ms(void) {
    // Helper function to return the current time in ms
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return ((tp.tv_sec * 1000) + (tp.tv_usec / 1000));
}

void klondike_loop(void) {
    /* Overdesigned main loop? Probably. This is your standard,
     * events-update-render loop with a nice check to keep it at a steady
     * 30fps (otherwise it will render as fast as it can draw, and since this
     * isn't a very processor intensive program at all - it can look blinky) 
     * Update: this doesn't stop it looking blinky. Fairly certain the way I'm
     * using escape codes to clear/refresh the screen is causing the blinky.
     * Current fix is only drawing the screen when the game does something that
     * would make the display change.*/
    long prev = current_ms();
    long lag = 0, current = 0, elapsed = 0;
    long msperframe = 33; // 16ms = ~60fps, 33ms = ~30fps
    g_klondike->flags |= GFL_RUNNING;
    while(check_flag(g_klondike->flags, GFL_RUNNING)) {
        current = current_ms();
        elapsed = current - prev;
        prev = current;
        lag += elapsed;
        
        klondike_events();
        klondike_update();
        if(check_flag(g_klondike->flags,GFL_DRAW)) {
            klondike_draw();
        }
        if(check_flag(g_klondike->flags,GFL_RESTART)) {
            g_klondike->flags &= ~GFL_RUNNING;
        }
        while(lag >= msperframe) {
            lag -= msperframe;
        }
    }
    // Score check here... If score is new high score, save it
    if(g_klondike->score > g_settings->klondike_hs) {
        g_settings->klondike_hs = g_klondike->score;
        save_settings();
    }
    // If the game is going to be restarted, do it here
    if(check_flag(g_klondike->flags,GFL_RESTART)) {
        klondike_init();
    }
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
        case 'I':
        case 'i': toggle_button(g_klondike->btns[KL_FND_H]);
                  redraw = true;
                  break;
        case 'J':
        case 'j': toggle_button(g_klondike->btns[KL_FND_D]);
                  redraw = true;
                  break;
        case 'K':
        case 'k': toggle_button(g_klondike->btns[KL_FND_C]);
                  redraw = true;
                  break;
        case 'L':
        case 'l': toggle_button(g_klondike->btns[KL_FND_S]);
                  redraw = true;
                  break;
        case 'M':
        case 'm': toggle_button(g_klondike->btns[KL_STOCK]);
                  redraw = true;
                  break;
        case 27: 
        case 'q':
                  klondike_pause(); 
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
    slist_push(&menu, "cnq");
    slist_push(&menu, "Change card color settings");
    //slist_push(&menu, "Save game and quit");
    //slist_push(&menu, "Load saved game");
    slist_push(&menu, "Start new game");
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
        case 's':
            //save_klondike();
            g_klondike->flags &= ~GFL_RUNNING;
            break;
        case 'l':
            //load_klondike();
            break;
        case 'n':
            g_klondike->flags |= GFL_RESTART;
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
