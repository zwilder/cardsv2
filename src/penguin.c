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

void penguin_init(void) {
    // Allocate memory for decks/buttons
    g_penguin = create_solitaire(PN_NUM_DECKS);

    // Put the buttons in the right spot
    
    // Put 52 cards in the stock, and shuffle it
    fill_deck(g_penguin->decks[PN_STOCK]);
    shuffle_deck(g_penguin->decks[PN_STOCK]);

    // Deal the cards, draw the cards, enter the loop
    penguin_deal();
    penguin_update();
    penguin_draw();
    penguin_loop();

    // Cleanup
    penguin_cleanup();
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
                break;
        }
        add_card_to_deck(deck, card);

        // Get the next one
        card = search_deck(g_penguin->decks[PN_STOCK], rflag);
    }

    // Next, each tableau deck draws seven cards, except tab A which already has
    // one card in it (beak)
    draw_cards(stock, g_klondike->decks[PN_TAB_A], 6);
    draw_cards(stock, g_klondike->decks[PN_TAB_B], 7);
    draw_cards(stock, g_klondike->decks[PN_TAB_C], 7);
    draw_cards(stock, g_klondike->decks[PN_TAB_D], 7);
    draw_cards(stock, g_klondike->decks[PN_TAB_E], 7);
    draw_cards(stock, g_klondike->decks[PN_TAB_F], 7);
    draw_cards(stock, g_klondike->decks[PN_TAB_G], 7);
}

void penguin_loop(void) {

}

void penguin_events(void) {

}

void penguin_update(void) {

}

void penguin_draw(void) {
/* Basic screen layout (80x24 standard)
 *         1         2         3         4         5         6         7         8
  12345678901234567890123456789012345678901234567890123456789012345678901234567890
 1   [a]   [b]   [c]   [d]   [e]   [f]   [g]   [h]  [i]  [j]  [k]  [l]  [m]  [n]
 2  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ┌  ┐ ┌  ┐ ┌  ┐ ┌  ┐ ┌  ┐ ┌  ┐ ┌  ┐                                   
 3  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗
 4  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗    P    E    N    G    U    I    N
 5  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  └  ┘ └  ┘ └  ┘ └  ┘ └  ┘ └  ┘ └  ┘
 6  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗    
 7  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗
 8  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ┌  ┐ ┌  ┐ ┌  ┐ ┌  ┐ 
 9  ║  ║  ║  ║  ║  ║  ║  ║  ║  ║  ║  ║  ║  ║    ♡    ♢    ♧    ♤
10  ║  ║  ║  ║  ║  ║  ║  ║  ║  ║  ║  ║  ║  ║
 1  ╚══╝  ╚══╝  ╚══╝  ╚══╝  ╚══╝  ╚══╝  ╚══╝  └  ┘ └  ┘ └  ┘ └  ┘
 2                                             [1]  [2]  [3]  [4]
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
 3 Score: 150
 4 Press [q] to open the menu.
           1         2         3         4         5         6         7         8
  12345678901234567890123456789012345678901234567890123456789012345678901234567890

  Buttons at 3,0 then x+6
  Tableaus at 2,1 then x+6
  Cells at 44,1 then x+5
  Foundations at 44,7 then x+5
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

}
