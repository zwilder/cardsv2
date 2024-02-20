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

    // Check win condition
    if(g_klondike->win) {
        // None of the rest of update needs to run
        return;
    }

    // Count the selected buttons
    count = 0;
    for(i = 0; i < KL_NUM_DECKS; i++) {
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
        if(id_a == KL_STOCK) {
            g_klondike->btns[id_a]->selected = false; // Deselect button
            // Draw cards from stock to waste
            if(g_klondike->decks[KL_STOCK]->count) {
                draw_cards(g_klondike->decks[KL_STOCK],g_klondike->decks[KL_WASTE],3);
            } else {
                add_deck(g_klondike->decks[KL_WASTE],g_klondike->decks[KL_STOCK]);
            }
            g_klondike->redraw = true;
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
        if(g_klondike->toref->count && g_klondike->toref->cards) {
            cflags_a = get_last_card(g_klondike->toref)->flags;
        }
        if(g_klondike->fromref->count && g_klondike->fromref->cards) {
            cflags_b = get_last_card(g_klondike->fromref)->flags;
        }
        if(g_klondike->toref->id == KL_WASTE) {
            klondike_msg("Sorry, can't move cards to the waste!");
            g_klondike->redraw = true;
        } else if (g_klondike->fromref->id >= KL_FND_H) {
            klondike_msg("Sorry, can't move cards from the foundation.");
            g_klondike->redraw = true;
        } else if (g_klondike->toref->id == KL_STOCK) {
            klondike_msg("Sorry, can't move cards to the stock.");
            g_klondike->redraw = true;
        } else if (g_klondike->toref->id >= KL_FND_H) {
            // Attempting to move a card to the foundation
            // It would be cool if the game checcked each foundation, found
            // which one is the "right one" (if any) and then moved it there all
            // automagically. 
            // Old
            if(g_klondike->toref->id == KL_FND_H) {
                if(card_hearts(cflags_b) && 
                        card_in_asc_sequence(cflags_b,cflags_a)) {
                    valid_move = true;
                }
            } else if (g_klondike->toref->id == KL_FND_D) {
                if(card_diamonds(cflags_b) && 
                        card_in_asc_sequence(cflags_b,cflags_a)) {
                    valid_move = true;
                }
            } else if (g_klondike->toref->id == KL_FND_C) {
                if(card_clubs(cflags_b) && 
                        card_in_asc_sequence(cflags_b,cflags_a)) {
                    valid_move = true;
                }
            } else if (g_klondike->toref->id == KL_FND_S) {
                if(card_spades(cflags_b) && 
                        card_in_asc_sequence(cflags_b,cflags_a)) {
                    valid_move = true;
                }
            }
            if(valid_move) {
                //Move card
                g_klondike->score += 10; // Moving a card to a foundation is 10pts
                move_last_card_to_deck(g_klondike->fromref, g_klondike->toref);
                klondike_msg(NULL);
                g_klondike->redraw = true;
            }
        } else {
            // Attempting to move card to a tableau
            if(g_klondike->fromref->id == KL_WASTE) {
                if(klondike_valid_move(cflags_a,cflags_b)) {
                    move_last_card_to_deck(g_klondike->fromref, g_klondike->toref);
                    klondike_msg(NULL);
                    g_klondike->score += 5;
                    g_klondike->redraw = true;
                }else if (!g_klondike->toref->cards && 
                        (check_flag(cflags_b, CD_K))) {
                    move_last_card_to_deck(g_klondike->fromref, g_klondike->toref);
                    klondike_msg(NULL);
                    g_klondike->redraw = true;
                }
            } else {
                klondike_check_sequence();
            }
        }

        g_klondike->btns[id_a]->selected = false;
        g_klondike->btns[id_b]->selected = false;
        g_klondike->fromref = NULL;
        g_klondike->toref = NULL;
    }

    // Activate/deactivate waste button
    if(g_klondike->decks[KL_WASTE]->count) {
        g_klondike->btns[KL_WASTE]->active = true;
    } else {
        g_klondike->btns[KL_WASTE]->active = false;
    }
    // Check win condition
    if(!g_klondike->win) {
        count = 0;
        for(i = KL_FND_H; i <= KL_FND_S; i++) {
            count += count_cards(g_klondike->decks[i]->cards); 
        }
        if(count == 52) {
            // all 52 cards are on the foundations
            g_klondike->win = true;
            // Turn off all the buttons
            for(i = 0; i < KL_NUM_DECKS; i++) {
                g_klondike->btns[i]->selected = false;
                g_klondike->btns[i]->active = false;
            }
            g_klondike->fromref = NULL;
            g_klondike->toref = NULL;
        }
    }
}

void klondike_check_sequence(void) {
    Card *card = NULL, *tocard = NULL, *prev = NULL;
    bool valid_move = false;

    // The first visible card in a tableau is the highest in sequence
    card = g_klondike->fromref->cards;
    while(card) {
        if(check_flag(card->flags, CD_UP)) break;
        card = card->next;
    }
    if(!card) {
        // If for some reason it didn't find ANY face up card, grab the last
        card = get_last_card(g_klondike->fromref);
    }

    // See how many cards are in the sequence
    // Prompt the user to see how many they want to move
    // Set card to the first card the user wants to move
    
    // Check last card of "to", to see if this move is valid
    if(!g_klondike->toref->cards) {
        //Can only move a king to an empty spot
        if(check_flag(card->flags, CD_K)) {
            // Valid, do it
            valid_move = true;
        }
    } else {
        tocard = get_last_card(g_klondike->toref);
        // If this fails, we need to check card->next until card->next is NULL
        // to see if there is a valid move
        if(klondike_valid_move(tocard->flags, card->flags)) {
            // Valid, do it
            valid_move = true;
        } else {
            // Check the next cards
            while(card->next) {
                card = card->next;
                if(klondike_valid_move(tocard->flags,card->flags)) {
                    valid_move = true;
                    break;
                }
            }
        }
    }

    // Find card BEFORE card in "from" deck (next to last card)
    if(valid_move) {
        prev = g_klondike->fromref->cards;
        if(prev != card) {
            // card isn't the only one in "from" deck
            while(prev->next != card) {
                prev = prev->next;
            }
            // prev is next to last card in fromref
            prev->next = NULL; //Sever that chain!
            g_klondike->fromref->count = count_cards(g_klondike->fromref->cards);
        } else {
            //prev == card
            //card is the only one in "from" deck
            g_klondike->fromref->cards = NULL;
        }
        if(tocard) {
            // Reattach that chain!
            tocard->next = card;
            card->prev = tocard; // Think this fixes the double llist issue
        } else {
            g_klondike->toref->cards = card;
        }
        g_klondike->toref->count = count_cards(g_klondike->toref->cards);
        g_klondike->redraw = true;
    }
}

bool klondike_valid_move(int a, int b) {
    return (card_in_asc_sequence(a,b) && card_alt_color(a,b));
}
