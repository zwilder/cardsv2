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

/*****
 * Cribbage update functions
 *****/

void cribbage_cpu_to_crib(void) {
    // Have the computer choose two cards to add to the crib.
    // The computer player is an idiot right now, and just chooses two random
    // cards from their hand. Eventually, it will analyze it's hand and choose
    // appropriate discards for it's skill level. 
    int i = mt_rand(0,5);
    Deck *crib = g_cribbage->decks[CR_CRIB];
    Deck *cpuhand = g_cribbage->decks[CR_CPU];
    add_card_to_deck(crib, 
            remove_card_from_deck(cpuhand,
                get_card_at(cpuhand,i)));
    i = mt_rand(0,4);
    add_card_to_deck(crib, 
            remove_card_from_deck(cpuhand,
                get_card_at(cpuhand,i)));
}

void cribbage_update_discard(void) {
    int i = 0, count = 0;
    int id_a = 0, id_b = 0;
    char *astr = NULL, *bstr = NULL;
    Card *acard = NULL, *bcard = NULL;
    char ch = '\0';
    // Count how many buttons are selected
    count = 0;
    for(i = 0; i < 6; i++) {
        if(g_cribbage->btns[i]->selected) {
            count++;
            (id_a ? (id_b = i) : (id_a = i));
        }
    }

    // If 2, prompt the user for confirmation that these two are what they want
    // to add to the crib
    if(2 == count) {
        acard = get_card_at(g_cribbage->decks[CR_PLAYER], id_a);
        bcard = get_card_at(g_cribbage->decks[CR_PLAYER], id_b);
        astr = get_card_str(acard);
        bstr = get_card_str(bcard);
        // If user confirms, add the two cards to the crib and change state to
        // GFL_CRIBPLAY
        // If user declines, deselect all buttons
        ch = cribbage_prompt("Add the %s and %s to the crib? [y/n]", astr, bstr);
        if('Y' == ch)
        {
            // Move cards to crib, change state
            remove_card_from_deck(g_cribbage->decks[CR_PLAYER],acard);
            add_card_to_deck(g_cribbage->decks[CR_CRIB],acard);
            remove_card_from_deck(g_cribbage->decks[CR_PLAYER],bcard);
            add_card_to_deck(g_cribbage->decks[CR_CRIB],bcard);

            // TODO CPU needs to add two cards to the crib here
            cribbage_cpu_to_crib();

            g_cribbage->flags &= ~GFL_CRIBDISC;
            g_cribbage->flags |= GFL_CRIBPLAY | GFL_DRAW;
            cribbage_msg("Choose a card to play");
            for(i = 0; i < 6; i++) {
                g_cribbage->btns[i]->selected = false;
                // Deactivate last two buttons, not needed now
                if(i > 3) {
                    g_cribbage->btns[i]->active = false;
                }
            }
        } else {
            for(i = 0; i < 6; i++) {
                g_cribbage->btns[i]->selected = false;
            }
        }
    }
    if(astr) free(astr);
    if(bstr) free(bstr);
}

void cribbage_cpu_play(void) {
    // Have the cpu play a card from it's hand on to the table.
    // The cpu player is currently an idiot, so it just chooses a random, valid,
    // card and places it on the table. Eventually, it will choose a card
    // "intelligently" 
    Deck *board = g_cribbage->decks[CR_CPU_BOARD];
    Deck *cpuhand = g_cribbage->decks[CR_CPU];
    int numcards = count_cards(cpuhand->cards);
    int i = mt_rand(0,numcards);
    i = 0;
    add_card_to_deck(board, 
            remove_card_from_deck(cpuhand,
                get_card_at(cpuhand,i)));
}

void cribbage_update_count(void) {
    // Count the value of cards on the table, adding points to the person who
    // played the last card if applicable, and resetting the count to 0 if both
    // players are "go"
    Deck *playerboard = g_cribbage->decks[CR_PLAYER_BOARD];
    Deck *cpuboard = g_cribbage->decks[CR_CPU_BOARD];
    int newcount = 0;
    Card *tmp = playerboard->cards;
    while(tmp) {
        newcount += cribbage_card_value(tmp->flags);
        tmp = tmp->next;
    }
    tmp = cpuboard->cards;
    while(tmp) {
        newcount += cribbage_card_value(tmp->flags);
        tmp = tmp->next;
    }

    g_cribbage->count = newcount;
}

void cribbage_update_play(void) {
    // Alternate turns playing cards, scoring points and increasing the count
    int i = 0, id = 0, count = 0;
    bool selected = false;
    Card *pcard = NULL;
    // Check to see if both player and cpu hands are empty
    if((count_cards(g_cribbage->decks[CR_PLAYER]->cards) == 0) &&
            (count_cards(g_cribbage->decks[CR_CPU]->cards) == 0)) {
        cribbage_prompt("Press any key to continue...");
        g_cribbage->flags &= ~GFL_CRIBPLAY;
        g_cribbage->flags |= GFL_CRIBSHOW | GFL_DRAW;
        //Move cards from the board back to the hand
        add_deck(g_cribbage->decks[CR_PLAYER_BOARD], g_cribbage->decks[CR_PLAYER]);
        add_deck(g_cribbage->decks[CR_CPU_BOARD], g_cribbage->decks[CR_CPU]);
        return;
    }
    // Check to see if it's the player's turn,
    if(g_cribbage->pturn) {
        //Check if a button is selected
        count = 0;
        for(i = 0; i < 6; i++) {
            if(g_cribbage->btns[i]->active) {
                count += 1;
            }
            if(g_cribbage->btns[i]->selected) {
                selected = true;
                id = i;
            }
        }
        if(selected) {
            // get the card that was selected, place it on the board.
            pcard = get_card_at(g_cribbage->decks[CR_PLAYER], id);
            // End player turn
            if(pcard) {
                remove_card_from_deck(g_cribbage->decks[CR_PLAYER],pcard);
                add_card_to_deck(g_cribbage->decks[CR_PLAYER_BOARD],pcard);
                cribbage_update_count();
                g_cribbage->btns[count-1]->active = false;
                g_cribbage->pturn = false;
            }
            for(i = 0; i < 6; i++) {
                g_cribbage->btns[i]->selected = false;
            }
            g_cribbage->flags |= GFL_DRAW;
        }
    } else {
        // Have the computer play a card
        cribbage_cpu_play();
        cribbage_update_count();
        g_cribbage->pturn = true;
        g_cribbage->flags |= GFL_DRAW;
    }
}

void cribbage_update_show(void) {
    // Score hands

}

void cribbage_update(void) {
    // Calls the appropriate update routine for whichever state the game is in
    if(check_flag(g_cribbage->flags, GFL_CRIBDISC)) {
        cribbage_update_discard();
    } else if (check_flag(g_cribbage->flags, GFL_CRIBPLAY)) {
        cribbage_update_play();
    } else if (check_flag(g_cribbage->flags, GFL_CRIBSHOW)) {
        cribbage_update_show();
    }
}

