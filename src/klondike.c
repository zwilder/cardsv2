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

/* Basic screen layout (80x24 standard)
 *         1         2         3         4         5         6         7         8
  12345678901234567890123456789012345678901234567890123456789012345678901234567890
 1   [m]    [a]         [b]  [c]  [d]  [e]  [f]  [g]  [h]      [i]  [j]  [k]  [l]
 2   ╔══╗ ╔╔╔══╗        ╔══╗ ╔══╗ ╔══╗ ╔══╗ ╔══╗ ╔══╗ ╔══╗     ┌  ┐ ┌  ┐ ┌  ┐ ┌  ┐
 3   ║  ║ ║║║  ║        ║  ║ ╔══╗ ╔══╗ ╔══╗ ╔══╗ ╔══╗ ╔══╗      ♡    ♢    ♧    ♤
 4   ║  ║ ║║║  ║        ║  ║ ║  ║ ╔══╗ ╔══╗ ╔══╗ ╔══╗ ╔══╗ 
 5   ╚══╝ ╚╚╚══╝        ╚══╝ ║  ║ ║  ║ ╔══╗ ╔══╗ ╔══╗ ╔══╗     └  ┘ └  ┘ └  ┘ └  ┘
 6                           ╚══╝ ║  ║ ║  ║ ╔══╗ ╔══╗ ╔══╗                
 7                                ╚══╝ ║  ║ ║  ║ ╔══╗ ╔══╗ 
 8                                     ╚══╝ ║  ║ ║  ║ ╔══╗             
 9                                          ╚══╝ ║  ║ ║  ║
10                                               ╚══╝ ║  ║   
 1                                                    ╚══╝
 2
 3
 4
 5
 6
 7
 8
 9
20
 1
 2 Messages        
 3
 4 Stock: 21. Waste 3. Press q to exit.
           1         2         3         4         5         6         7         8
  12345678901234567890123456789012345678901234567890123456789012345678901234567890

  Stock 3,1
  Waste 8,1
  TabB  22,1
  TabC  27,1
  TabD  32,1
  TabE  37,1
  TabF  42,1
  TabG  47,1
  TabH  52,1
  FndH  61,1
  FndD  66,1
  FndC  71,1
  FndS  76,1
  Msgs  0,21
  Stat  0,23

 * Drawing reference
 * ♠ u2660, ♤ u2664
 * ♥ u2665, ♡ u2661
 * ♦ u2666, ♢ u2662
 * ♣ u2663, ♧ u2667
    ┌  ┐ u250c u2510
     
     
    └  ┘ u2514 u2518
     	0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F
U+255x	═	║	╒	╓	╔	╕	╖	╗	╘	╙	╚	╛	╜	╝	╞	╟
 */

Klondike *g_klondike = NULL;

void klondike_init(void) {
    int i = 0;
    if(g_klondike) klondike_cleanup();

    // Allocate memory for decks/buttons
    g_klondike = malloc(sizeof(Klondike));
    g_klondike->decks = malloc(NUM_DECKS * sizeof(Deck));
    g_klondike->btns = malloc(NUM_DECKS * sizeof(Button));
    for(i = 0; i < NUM_DECKS; i++) {
        g_klondike->decks[i] = create_deck();
        g_klondike->decks[i]->id = i;
        g_klondike->btns[i] = create_button(0,0,i,'a'+i);
    }

    // Put the buttons in the right spot
    g_klondike->btns[STOCK]->x = 3;
    g_klondike->btns[STOCK]->active = true;
    g_klondike->btns[WASTE]->x = 8;
    
    for(i = 0; i < 7; i++) {
        g_klondike->btns[TAB_B + i]->x = 22 + (5*i);
        g_klondike->btns[TAB_B + i]->active = true;
    }
    
    for(i = 0; i < 4; i++) {
        g_klondike->btns[FND_H + i]->x = 61 + (5*i);
        g_klondike->btns[FND_H + i]->active = true;
    }

    // Put 52 cards in the stock, and shuffle it
    fill_deck(g_klondike->decks[STOCK]);
    shuffle_deck(g_klondike->decks[STOCK]);

    // Global message is null
    g_klondike->msg = NULL;

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
    int i = 0;
    for(i = 0; i < NUM_DECKS; i++) {
        destroy_deck(g_klondike->decks[i]);
        destroy_button(g_klondike->btns[i]);
    }
    free(g_klondike->decks);
    free(g_klondike->btns);
    if(g_klondike->msg) {
        free(g_klondike->msg);
    }
    free(g_klondike);
}

void klondike_deal(void) {
    int i = 0, j = 0;
    Card *tmp = NULL;
    for(i = 0; i < 7; i++) {
        j = i + 1;
        // Draw cards from the deck to put on the tableau
        draw_cards(g_klondike->decks[STOCK],g_klondike->decks[TAB_B + i], j);

        // Turn the top card (last card) faceup
        tmp = g_klondike->decks[TAB_B + i]->cards;
        while(tmp->next) {
            tmp = tmp->next;
        }
        engage_flag(&(tmp->flags), CD_UP);
    }
}

void klondike_msg(char *msg,...) {
    if(g_klondike->msg) {
        free(g_klondike->msg);
        g_klondike->msg = NULL;
    }
    if(!msg) return;
    va_list args;
    va_start(args,msg);
    int i = strlen(msg) + 1;
    g_klondike->msg = malloc(sizeof(char) * i);
    vsnprintf(g_klondike->msg,i,msg,args);
    va_end(args);
}

void klondike_loop(void) {
    bool running = true;
    while(running) {
        running = klondike_events();
        klondike_update();
        klondike_draw();
    }
}

bool klondike_events(void) {
    bool running = true;
    char ch = kb_get_char();
    switch(ch) {
        case 'A':
        case 'a': toggle_button(g_klondike->btns[WASTE]); break;
        case 'B':
        case 'b': toggle_button(g_klondike->btns[TAB_B]); break;
        case 'C':
        case 'c': toggle_button(g_klondike->btns[TAB_C]); break;
        case 'D':
        case 'd': toggle_button(g_klondike->btns[TAB_D]); break;
        case 'E':
        case 'e': toggle_button(g_klondike->btns[TAB_E]); break;
        case 'F':
        case 'f': toggle_button(g_klondike->btns[TAB_F]); break;
        case 'G':
        case 'g': toggle_button(g_klondike->btns[TAB_G]); break;
        case 'H':
        case 'h': toggle_button(g_klondike->btns[TAB_H]); break;
        case 'I':
        case 'i': toggle_button(g_klondike->btns[FND_H]); break;
        case 'J':
        case 'j': toggle_button(g_klondike->btns[FND_D]); break;
        case 'K':
        case 'k': toggle_button(g_klondike->btns[FND_C]); break;
        case 'L':
        case 'l': toggle_button(g_klondike->btns[FND_S]); break;
        case 'M':
        case 'm': toggle_button(g_klondike->btns[STOCK]); break;
        case 'Q':
        case 'q': running = false; break;
        default: break;
    }

    return running;
}

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

void klondike_draw(void) {
    int i = 0,j = 0;
    int xo = 0, yo = 0; 
    int x = 0;
    Deck *deck = NULL;
    Card *cards = NULL, *cda = NULL, *cdb = NULL, *cdc = NULL;
    xo = (g_screenW / 2) - 40; // Standard screen size is 80x24
    yo = (g_screenH / 2) - 12;

    // Clear screen
    scr_clear();

    // Draw Buttons
    for(i = 0; i < NUM_DECKS; i++) {
        pt_button_at(g_klondike->btns[i], 
                g_klondike->btns[i]->x + xo,
                g_klondike->btns[i]->y + yo);
    }

    // Draw Stock
    if(g_klondike->decks[STOCK]->cards) {
        pt_card_back(3+xo,1+yo);
    } else {
        pt_card_space(3+xo,1+yo);
    }

    // Draw waste
    if(g_klondike->decks[WASTE]->cards) {
        // Draw the last three cards on the waste pile
        // This is kinda ugly, but it works so... its ok?
        x = 8;
        deck = g_klondike->decks[WASTE];
        cda = get_card_at(deck, deck->count - 3); //Third to last
        cdb = get_card_at(deck, deck->count - 2); //Second to last
        cdc = get_card_at(deck, deck->count - 1); //Last
        if((cda == cdb) || (cda == cdc)) cda = NULL; //In case it loops around?
        if((cdb == cdc)) cdb = NULL; // Same
        if(cda && cdb && cdc) {
            pt_card_left(x+xo,1+yo,cda);
            pt_card_left(x+xo+1,1+yo,cdb);
            pt_card(x+xo+2,1+yo,cdc);
        } else if (cdb && cdc) {
            pt_card_left(x+xo,1+yo,cdb);
            pt_card(x+xo+1,1+yo,cdc);
        } else if (cdc) {
            pt_card(x+xo,1+yo,cdc);
        }
    }

    // Draw Tableaus
    for(i = 0; i < 7; i++) {
        deck = g_klondike->decks[TAB_B + i];
        if(deck->cards) {
            cards = deck->cards;
            j = 0;
            // Print the tops of all cards, except last
            while(cards->next) {
                if(check_flag(cards->flags, CD_UP)) {
                    pt_card_top(22+(5*i)+xo,1+j+yo,cards);
                } else {
                    pt_card_back(22+(5*i)+xo,1+j+yo);
                }
                j++;
                cards = cards->next;
            }
            // Print the last card (which is always face up)
            engage_flag(&(cards->flags),CD_UP);
            pt_card(22+(5*i)+xo,1+j+yo,cards);
        }
    }

    // Draw Foundations
    if(g_klondike->decks[FND_H]->count) {
        pt_card(61+xo,1+yo,
                get_last_card(g_klondike->decks[FND_H]));
    } else {
        pt_card_space_suite(61+xo,1+yo, CD_H);
    }
    if(g_klondike->decks[FND_D]->count) {
        pt_card(66+xo,1+yo,
                get_last_card(g_klondike->decks[FND_D]));
    } else {
        pt_card_space_suite(66+xo,1+yo, CD_D);
    }
    if(g_klondike->decks[FND_C]->count) {
        pt_card(71+xo,1+yo,
                get_last_card(g_klondike->decks[FND_C]));
    } else {
        pt_card_space_suite(71+xo,1+yo, CD_C);
    }
    if(g_klondike->decks[FND_S]->count) {
        pt_card(76+xo,1+yo,
                get_last_card(g_klondike->decks[FND_S]));
    } else {
        pt_card_space_suite(76+xo,1+yo, CD_S);
    }

    // Draw msg
    if(g_klondike->msg) {
        scr_pt_clr(xo, 21+yo, WHITE, BLACK, "%s",
                g_klondike->msg);
    }

    // Draw status
    scr_pt_clr(xo,23+yo,WHITE,BLACK,
            "Stock: %d. Waste: %d. Press q to quit.",
            g_klondike->decks[STOCK]->count,
            g_klondike->decks[WASTE]->count);

    // Reset drawing functions
    scr_reset();
}
