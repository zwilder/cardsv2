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

void klondike_draw(void) {
    int i = 0,j = 0;
    int xo = 0, yo = 0; 
    int x = 0;
    Deck *deck = NULL;
    Card *cards = NULL, *cda = NULL, *cdb = NULL, *cdc = NULL;
    xo = (g_screenW / 2) - (SCREEN_WIDTH / 2); // Standard screen size is 80x24, defined in glyph.c
    yo = (g_screenH / 2) - (SCREEN_HEIGHT / 2);

    // Clear and draw the screen buffer - this just blacks out the screen
    clear_screen(g_screenbuf);
    draw_screen(g_screenbuf);
    if(g_klondike->win) {
        //YOU WIN!
        pt_card_title((g_screenW / 2) - 16, 
                (g_screenH / 2) - 2,
                "YOU WIN!");
    }

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
        /*
        cdc = get_last_card(deck); // Last
        if(cdc->prev) cdb = cdc->prev; // Second to last
        if(cdb->prev) cda = cdb->prev; // Third to last
        */
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
            if(!check_flag(cards->flags, CD_UP)) {
                engage_flag(&(cards->flags),CD_UP);
                g_klondike->score += 5; // Flipping over a tab card is 5pts
            }
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

    // Draw score
    scr_pt_clr(xo, 22+yo, BRIGHT_WHITE, BLACK, "Score: %d",
            g_klondike->score);

    // Draw status
    scr_pt_clr(xo,23+yo,BRIGHT_BLACK,BLACK,
            "High score: %d. Stock: %d. Waste: %d. Press [q] to quit, [r] to restart.",
            g_settings->klondike_hs,
            g_klondike->decks[STOCK]->count,
            g_klondike->decks[WASTE]->count);

    // Reset drawing functions
    //scr_reset();
    g_klondike->redraw = false;
}
