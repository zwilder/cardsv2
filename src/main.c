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

int main(int argc, char **argv) {
    term_init(); // Initialize the terminal
    init_genrand(time(NULL)); // Seed the pnrg
    
    scr_clear();
    pt_card_title((g_screenW / 2) - 12, 0,
            "Cards!");
    scr_reset();
    kb_get_bl_char();

    /*
    Deck *testdeck = create_deck();
    Deck *hand = create_deck();
    Card *card = NULL;
    int i = 0;
    fill_deck(testdeck);
    shuffle_deck(testdeck);
    while(testdeck->count) {
        scr_clear();
        pt_card_title((g_screenW / 2) - 12,
                0, "Cards!");
        scr_pt_clr((g_screenW / 2) - 7,
                g_screenH / 2, BLACK, WHITE, 
                "Deck size: %d", testdeck->count);
        scr_pt_clr((g_screenW / 2) - 7,
                (g_screenH / 2)+1, BLACK, WHITE, 
                "Hand size: %d", hand->count);
        draw_card(testdeck,hand);
        //card = remove_card_from_deck(testdeck, testdeck->cards);
        //add_card_to_deck(hand,card);
        card = hand->cards;
        while(card->next) {
            card = card->next;
        }
        pt_card_back(1,1);
        pt_card(1,2,card);
        scr_reset();
        kb_get_bl_char();
    }
    
    scr_clear();
    add_deck(hand,testdeck);
    shuffle_deck(testdeck);
    draw_cards(testdeck, hand, 5);
    card = hand->cards;
    while(card->next) {
        pt_card_left(1+i, 1, card);
        card = card->next;
        i++;
    }
    pt_card(1+i,1,card);
    scr_reset();
    kb_get_bl_char();

    destroy_deck(hand);
    destroy_deck(testdeck);

    */
    term_close(); // Reset the terminal
    return 0;
}
