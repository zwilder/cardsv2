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

Solitaire *g_penguin = NULL;

bool penguin_init(void) {
    /*
     * Initialize a solitaire game of Penguin, and return a bool indicating if
     * the user wants to return to the main menu, or quit the game entirely.
     */
    bool ret_to_main = false;
    int i = 0, j = 0;

    if(g_penguin) penguin_cleanup();
    // Allocate memory for decks/buttons
    g_penguin = create_solitaire(PN_NUM_DECKS);

    // Register events/update/draw functions
    g_penguin->events = &penguin_events;
    g_penguin->update = &penguin_update;
    g_penguin->draw = &penguin_draw;

    // Put the buttons in the right spot
    j = 0;
    for(i = PN_TAB_A; i <= PN_TAB_G; i++) {
        g_penguin->btns[i]->active = true;
        g_penguin->btns[i]->x = 1+(j*6);
        g_penguin->btns[i]->y = 0;
        j++;
    }

    j = 0;
    for(i = PN_CELL_A; i <= PN_CELL_G; i++) {
        g_penguin->btns[i]->active = true;
        g_penguin->btns[i]->x = 45 + (j*5);
        g_penguin->btns[i]->y = 0;
        j++;
    }

    j = 0;
    for(i = PN_FND_H; i <= PN_FND_S; i++) {
        g_penguin->btns[i]->active = true;
        g_penguin->btns[i]->x = 60 + (j*5);
        g_penguin->btns[i]->y = 11;
        // Renaming the foundation buttons here
        g_penguin->btns[i]->ch = '1' + j;
        j++;
    }
    
    // Put 52 cards in the stock, and shuffle it
    fill_deck(g_penguin->decks[PN_STOCK]);
    shuffle_deck(g_penguin->decks[PN_STOCK]);

    // Deal the cards, draw the cards, enter the loop
    penguin_deal();
    penguin_update();
    penguin_draw();
    penguin_loop();

    // If the game is going to be restarted, do it here
    if(check_flag(g_penguin->flags,GFL_RESTART)) {
        ret_to_main = penguin_init();
    } else {
        ret_to_main = check_flag(g_penguin->flags, GFL_QTOMAIN);
    }
    // Cleanup
    penguin_cleanup();
    return ret_to_main;
}

void penguin_cleanup(void) {
    if(!g_penguin) return;
    destroy_solitaire(g_penguin);
    g_penguin = NULL;
}

void penguin_deal(void) {
    Card *beak = NULL;
    Card *card = NULL;
    Deck *stock= g_penguin->decks[PN_STOCK];
    Deck *deck = NULL;
    int rflag = 0, sflag = 0;

    // Draw one card from stock and put it in PN_TAB_A
    draw_card(stock, g_penguin->decks[PN_TAB_A]);

    // The first card in PN_TAB_A is the "beak", we need to find the matching
    // rank cards in the deck and move them to the appropriate foundation
    beak = g_penguin->decks[PN_TAB_A]->cards; 
    rflag = get_rank_flag(beak->flags);
    card = search_deck(stock, rflag);
    while(card) {
        // Remove card from PN_STOCK
        remove_card_from_deck(stock, card);

        // Find the suite, and put it in the appropriate foundation
        sflag = get_suite_flag(card->flags);
        switch(sflag) {
            case CD_H:
                deck = g_penguin->decks[PN_FND_H];
                break;
            case CD_D:
                deck = g_penguin->decks[PN_FND_D];
                break;
            case CD_S:
                deck = g_penguin->decks[PN_FND_S];
                break;
            case CD_C:
                deck = g_penguin->decks[PN_FND_C];
                break;
            default:
                solitaire_msg(g_penguin, "WTF HAPPENED? How did you get here?!");
                break;
        }
        add_card_to_deck(deck, card);

        // Get the next one
        card = search_deck(stock, rflag);
    }

    // Next, each tableau deck draws seven cards, except tab A which already has
    // one card in it (beak)
    draw_cards(stock, g_penguin->decks[PN_TAB_A], 6);
    draw_cards(stock, g_penguin->decks[PN_TAB_B], 7);
    draw_cards(stock, g_penguin->decks[PN_TAB_C], 7);
    draw_cards(stock, g_penguin->decks[PN_TAB_D], 7);
    draw_cards(stock, g_penguin->decks[PN_TAB_E], 7);
    draw_cards(stock, g_penguin->decks[PN_TAB_F], 7);
    draw_cards(stock, g_penguin->decks[PN_TAB_G], 7);
}

void penguin_loop(void) {
    // Main loop
    solitaire_loop(g_penguin);

    // Score check here... If score is new high score, save it
    if(g_penguin->score > g_settings->penguin_hs) {
        g_settings->penguin_hs = g_penguin->score;
    }
    if(g_penguin->score) {
        g_settings->penguin_last = g_penguin->score;
    }
    save_settings();
}

void penguin_events(void) {
    bool redraw = true;
    char ch = kb_get_char();
    int btn_id = -1; 
    switch(ch) {
        case 'A':
        case 'a': btn_id = PN_TAB_A; break;
        case 'B':
        case 'b': btn_id = PN_TAB_B; break;
        case 'C':
        case 'c': btn_id = PN_TAB_C; break;
        case 'D':
        case 'd': btn_id = PN_TAB_D; break;
        case 'E':
        case 'e': btn_id = PN_TAB_E; break;
        case 'F':
        case 'f': btn_id = PN_TAB_F; break;
        case 'G':
        case 'g': btn_id = PN_TAB_G; break;
        case 'H':
        case 'h': btn_id = PN_CELL_A; break;
        case 'I':
        case 'i': btn_id = PN_CELL_B; break;
        case 'J':
        case 'j': btn_id = PN_CELL_C; break;
        case 'K':
        case 'k': btn_id = PN_CELL_D; break;
        case 'L':
        case 'l': btn_id = PN_CELL_E; break;
        case 'M':
        case 'm': btn_id = PN_CELL_F; break;
        case 'N':
        case 'n': btn_id = PN_CELL_G; break;
        case '1': btn_id = PN_FND_H; break;
        case '2': btn_id = PN_FND_D; break;
        case '3': btn_id = PN_FND_C; break;
        case '4': btn_id = PN_FND_S; break;
        case 27:
        case 'q': solitaire_pause(g_penguin); break;
        default: redraw = false; break;
    }
    if(btn_id != -1) {
        toggle_button(g_penguin->btns[btn_id]);
    }
    if(redraw) {
        g_penguin->flags |= GFL_DRAW;
    }
}
