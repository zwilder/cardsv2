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

void klondike_update(void) {
    int i = 0, count = 0;
    int id_a = 0, id_b = 0;
    int cflags_a = 0, cflags_b = 0;
    bool valid_move = false;
    // Activate/deactivate waste button
    if(g_klondike->decks[WASTE]->count) {
        g_klondike->btns[WASTE]->active = true;
    } else {
        g_klondike->btns[WASTE]->active = false;
    }

    // Count the selected buttons
    for(i = 0; i < NUM_DECKS; i++) {
        if(g_klondike->btns[i]->selected) {
            count++;
            (id_a ? (id_b = i) : (id_a = i));
        }
    }

    // If 0, NULL from/to references
    if(0 == count) {
        g_klondike->fromref = NULL;
        g_klondike->toref = NULL;
    } else if (1 == count) {
    // If 1, set from reference OR draw cards from stock
        // Only one button active, so we know it's id_a
        if(id_a == STOCK) {
            g_klondike->btns[id_a]->selected = false; // Deselect button
            // Draw cards from stock to waste
            if(g_klondike->decks[STOCK]->count) {
                draw_cards(g_klondike->decks[STOCK],g_klondike->decks[WASTE],3);
            } else {
                add_deck(g_klondike->decks[WASTE],g_klondike->decks[STOCK]);
            }
        } else {
            g_klondike->fromref = g_klondike->decks[id_a]; 
        }
    } else if (2 == count) {
    // If 2, set to reference, check move, and NULL from/to refs
        if(g_klondike->fromref == g_klondike->decks[id_a]) {
            g_klondike->toref = g_klondike->decks[id_b];
        } else {
            g_klondike->toref = g_klondike->decks[id_a];
        }

        //Check move here
        if(g_klondike->toref->count) {
            cflags_a = get_last_card(g_klondike->toref)->flags;
        }
        if(g_klondike->fromref->count) {
            cflags_b = get_last_card(g_klondike->fromref)->flags;
        }
        if(g_klondike->toref->id == WASTE) {
            klondike_msg("Sorry, can't move cards to the waste!");
        } else if (g_klondike->fromref->id >= FND_H) {
            klondike_msg("Sorry, can't move cards from the foundation.");
        } else if (g_klondike->toref->id == STOCK) {
            klondike_msg("Sorry, can't move cards to the stock.");
        } else if (g_klondike->toref->id >= FND_H) {
            // Attempting to move a card to the foundation
            if(g_klondike->toref->id == FND_H) {
                if(card_hearts(cflags_b) && 
                        card_in_asc_sequence(cflags_b,cflags_a)) {
                    valid_move = true;
                }
            } else if (g_klondike->toref->id == FND_D) {
                if(card_diamonds(cflags_b) && 
                        card_in_asc_sequence(cflags_b,cflags_a)) {
                    valid_move = true;
                }
            } else if (g_klondike->toref->id == FND_C) {
                if(card_clubs(cflags_b) && 
                        card_in_asc_sequence(cflags_b,cflags_a)) {
                    valid_move = true;
                }
            } else if (g_klondike->toref->id == FND_S) {
                if(card_spades(cflags_b) && 
                        card_in_asc_sequence(cflags_b,cflags_a)) {
                    valid_move = true;
                }
            }
            if(valid_move) {
                //Move card
                move_last_card_to_deck(g_klondike->fromref, g_klondike->toref);
                klondike_msg(NULL);
            }
        } else {
            // Attempting to move card to a foundation
            if(card_alt_color(cflags_b,cflags_a) && 
                    card_in_asc_sequence(cflags_a,cflags_b)) {
                // Add check here for moving multiple cards
                move_last_card_to_deck(g_klondike->fromref, g_klondike->toref);
                klondike_msg(NULL);
            } else if (!g_klondike->toref->count && (13 == get_rank(cflags_b))) {
                move_last_card_to_deck(g_klondike->fromref, g_klondike->toref);
                klondike_msg(NULL);
            }
        }

        g_klondike->btns[id_a]->selected = false;
        g_klondike->btns[id_b]->selected = false;
        g_klondike->fromref = NULL;
        g_klondike->toref = NULL;
    }

}
