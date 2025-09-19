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
 1   [a]   [b]   [c]   [d]   [e]   [f]   [g]   [h]  [i]  [j]  [k]  [l]  [m]  [n]
 2  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ┌  ┐ ┌  ┐ ┌  ┐ ┌  ┐ ┌  ┐ ┌  ┐ ┌  ┐                                   
 3  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗
 4  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗    P    E    N    G    U    I    N
 5  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  └  ┘ └  ┘ └  ┘ └  ┘ └  ┘ └  ┘ └  ┘
 6  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗                                                                        
 7  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗                                   
 8  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗  ╔══╗                 ┌  ┐ ┌  ┐ ┌  ┐ ┌  ┐ 
 9  ║  ║  ║  ║  ║  ║  ║  ║  ║  ║  ║  ║  ║  ║                  ♡    ♢    ♧    ♤
10  ║  ║  ║  ║  ║  ║  ║  ║  ║  ║  ║  ║  ║  ║
 1  ╚══╝  ╚══╝  ╚══╝  ╚══╝  ╚══╝  ╚══╝  ╚══╝                 └  ┘ └  ┘ └  ┘ └  ┘
 2                                                           [1]  [2]  [3]  [4]
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

  Buttons might look better/more natural underneath the tableaus/cells -
  obviously the y position of the button would have to be adjusted when a card
  is moved to/from that tableau.

  Buttons at 1,0 then x+6
  Foundation buttons at 59,11 then x+ 5
  Tableaus at 0,1 then x+6
  Cells at 44,1 then x+5
  Foundations at 59,7 then x+5
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
void penguin_draw(void) {
    int xo = (g_screenW / 2) - (SCREEN_WIDTH / 2); // Standard screen size is 80x24, defined in glyph.c
    int yo = (g_screenH / 2) - (SCREEN_HEIGHT / 2);
    int i = 0, j = 0, x = 0, y = 0, base = 0;
    char ch = '\0';
    char str[] = "PENGUIN";
    Deck *deck = NULL;
    Card *cards = NULL;

    // Clear and draw the screen buffer - this just blacks out the screen
    clear_screen(g_screenbuf);
    draw_screen(g_screenbuf);

    // Check to see if the game has been won...
    if(!check_flag(g_penguin->flags, GFL_WIN)) {
        // If it hasn't
        // Draw Buttons
        // Buttons should go underneath the tableaus, so I need to add the number of
        // the cards in the tableau + 3 to y
        for(i = PN_TAB_A; i < PN_CELL_A; i++) {
            j = g_penguin->decks[i]->count + 3;
            if(j == 3) j = 4;
            pt_button_at(g_penguin->btns[i],
                    g_penguin->btns[i]->x + xo,
                    g_penguin->btns[i]->y + j + yo);
        }
        for(i = PN_CELL_A; i < PN_STOCK; i++) {
            pt_button_at(g_penguin->btns[i],
                    g_penguin->btns[i]->x + xo,
                    g_penguin->btns[i]->y + yo);
        }

        // Draw Tableaus
        for(i = 0; i < 7; i++) {
            x = (6*i) + xo;
            // Get the current deck
            deck = g_penguin->decks[PN_TAB_A + i];
            if(deck->cards) {
                // Print the tops of all cards, except the last    
                j = 0;
                cards = deck->cards;
                while(cards->next) {
                    y = j + yo;
                    // Idea: what if we set a toggle to a keypress 
                    // to "highlight" the "next" card up from the 
                    // foundation? Maybe have it blink?
                    if(penguin_find_next_card(cards)) {
                        pt_card_top_blink(x,y,cards);
                    } else {
                        pt_card_top(x,y,cards);
                    }
                    j++;
                    cards = cards->next;
                }
                // Print the last card
                y = j + yo;
                if(penguin_find_next_card(cards)) {
                    pt_card_blink(x,y,cards);
                } else {
                    pt_card(x,y,cards);
                }
            } else {
                // print a space since there is no cards
                pt_card_space(x,yo);
            }
        }
    } else {
        // If the game has been won draw "You win!" banner
        pt_card_title(19+xo,11+yo,"You win!");
    }

    // Draw cells
    for(i = 0; i < 7; i++) {
        x = 44 + (5*i) + xo;
        y = 1 + yo;
        deck = g_penguin->decks[PN_CELL_A + i];
        if(deck->cards) {
            cards = deck->cards;
            pt_card(x,y,cards);
        } else {
            pt_card_space_ch(x,y,str[i]);
        }
    }

    // Draw foundations
    //Foundations at 44,7 then x+5
    for(i = 0; i < 4; i++) {
        deck = g_penguin->decks[PN_FND_H + i];
        x = 59 + (5*i) + xo;
        y = 7+yo;
        if(deck->cards) {
            base = get_rank(deck->cards->flags);
            cards = get_last_card(deck);
            if(cards) {
                pt_card(x,y,cards);
            }
        } else {
            switch(i) {
                case 0: pt_card_space_suite(x,y,CD_H); break;
                case 1: pt_card_space_suite(x,y,CD_D); break;
                case 2: pt_card_space_suite(x,y,CD_C); break;
                case 3: pt_card_space_suite(x,y,CD_S); break;
                default: break;
            }

        }
    }

    // Draw message
    if(g_penguin->msg) {
        scr_pt_clr(xo, 21+yo, WHITE, BLACK, "%s",
                g_penguin->msg);
    }
    // Draw score
    scr_pt_clr(xo, 22+yo, BRIGHT_WHITE, BLACK, "Score: %d",
            g_penguin->score);

    // Draw status
    if(base == 1) {
        scr_pt_clr(xo,23+yo,BRIGHT_BLACK,BLACK,
                "Base: A. High score: %d. Last score: %d.",
                g_settings->penguin_hs, g_settings->penguin_last);
    } else if(base < 11) {
        scr_pt_clr(xo,23+yo,BRIGHT_BLACK,BLACK,
                "Base: %d. High score: %d. Last score: %d.", 
                base, g_settings->penguin_hs, g_settings->penguin_last);
    } else {
        switch(base) {
            case 11: ch = 'J'; break;
            case 12: ch = 'Q'; break;
            case 13: ch = 'K'; break;
            default: ch = '?'; break;
        }
        scr_pt_clr(xo,23+yo,BRIGHT_BLACK,BLACK,
                "Base: %c. High score: %d. Last score: %d.",
                ch, g_settings->penguin_hs, g_settings->penguin_last);
    }
    g_penguin->flags &= ~GFL_DRAW;
}

bool penguin_find_next_card(Card *card) {
    // Take the rank and suite of card and see if it is exactly 1 higher than
    // the rank of the matching suite in the foundation
    bool success = false;
    Deck *fnd = NULL;
    Card *fnd_top_card = NULL;
    int i = 0;
    int card_rank = 0, fnd_rank = 0;

    if(!card) return success;
    card_rank = get_rank(card->flags);
    for(i = 0; i < 4; i++) {
        fnd = g_penguin->decks[PN_FND_H + i];
        fnd_top_card = get_last_card(fnd);
        if(!fnd_top_card) continue;
        if(card_same_suite(fnd_top_card->flags, card->flags)) {
            // We want the rank exactly one higher than the fnd rank
            fnd_rank = get_rank(fnd_top_card->flags) + 1;
            if(fnd_rank > 13) fnd_rank = 1; // Next higher than K is A
            if(fnd_rank == card_rank) {
                success = true;
                break;
            }
        }
    }
    return success;
}
