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
    char ch = kb_get_bl_char();
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
    // Activate/deactivate buttons
    if(g_klondike->decks[STOCK]->count) {
        g_klondike->btns[STOCK]->active = true;
    } else {
        g_klondike->btns[STOCK]->active = false;
    }
    if(g_klondike->decks[WASTE]->count) {
        g_klondike->btns[WASTE]->active = true;
    } else {
        g_klondike->btns[WASTE]->active = false;
    }

}

void klondike_draw(void) {
    int i = 0,j = 0;
    int xo = 0, yo = 0; 
    Deck *deck = NULL;
    Card *cards = NULL;
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
    pt_card_space_suite(61+xo,1+yo, CD_H);
    pt_card_space_suite(66+xo,1+yo, CD_D);
    pt_card_space_suite(71+xo,1+yo, CD_C);
    pt_card_space_suite(76+xo,1+yo, CD_S);

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
