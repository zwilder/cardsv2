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
    /*
     * This function needs to check which card was just played to the board
     * (last card in CPU_BOARD if !pturn, in PLAYER_BOARD if pturn), and compare
     * it to the card last played to the board (last card in CPU_BOARD if pturn,
     * last card in PLAYER_BOARD if !pturn). If it's an invalid move (makes
     * count exceed 31) return false. If it's a valid move, check to see if it
     * makes count 15 or 31 grant points. Check for runs, check for pairs/three
     * of a kind/four of a kind, grant points. If it's 31, reset count to 0 -
     * remember last card in that series so stop looking when we reach it when
     * analyzing cards (maybe this needs to be a Card* in Cribbage?)
     *
     * Idea: What if I used card flags? Turn cards "inactive" when both players
     * are a go, use one deck for the "board" (with a flag CD_PLAYER/CD_CPU on
     * each card to show who it belonged to/where it needs to be drawn), last
     * card played would always be the head of the deck->cards list (in a shared
     * deck)... I think that solves ALL the problems and would require the least
     * amount of "new" code to implement.
     */
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

bool cribbage_check_go(Deck *deck) {
    Card *cards = NULL;
    int count = g_cribbage->count;
    cards = deck->cards;
    while(cards) {
        if((cribbage_card_value(cards->flags) + count) < 31) {
             return false;
        }
        cards = cards->next;
    }
    return true;
}
void cribbage_update_play(void) {
    // Alternate turns playing cards, scoring points and increasing the count
    int i = 0, id = 0, count = 0;
    bool selected = false;
    Card *pcard = NULL;
    Deck *pdeck = g_cribbage->decks[CR_PLAYER];
    Deck *cdeck = g_cribbage->decks[CR_CPU];

    // Check to see if both players have a go
    if(cribbage_check_go(cdeck) && cribbage_check_go(pdeck)) {
        //reset count to 0, last person who played a card gets a point (2 if
        //count is 31), and set g_cribbage->lastcard to last card played.
    }

    // Check to see if both player and cpu hands are empty
    if((count_cards(g_cribbage->decks[CR_PLAYER]->cards) == 0) &&
            (count_cards(g_cribbage->decks[CR_CPU]->cards) == 0)) {
        g_cribbage->flags &= ~GFL_CRIBPLAY;
        g_cribbage->flags |= GFL_CRIBSHOW | GFL_DRAW;
        //Move cards from the board back to the hand
        //(This will move to cribbage_update_show())
        cribbage_prompt("Press any key to continue...");
        add_deck(g_cribbage->decks[CR_PLAYER_BOARD], g_cribbage->decks[CR_PLAYER]);
        add_deck(g_cribbage->decks[CR_CPU_BOARD], g_cribbage->decks[CR_CPU]);
        return;
    }
    // Check to see if it's the player's turn,
    if(g_cribbage->pturn) {
        //Check if player has a go
        //TODO

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
        // Check first if CPU has a go TODO
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

