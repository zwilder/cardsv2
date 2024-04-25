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
#include <ctype.h>
#include <cards.h>

bool cribbage_check_go(Deck *deck);
void cribbage_cpu_to_crib(void);
void cribbage_update_discard(void);
void cribbage_cpu_play(void);
void cribbage_update_count(void);
bool cribbage_check_go(Deck *deck);
void cribbage_add_points(int points, bool player);
void cribbage_update_play(void);
void cribbage_update_show(void);

/*****
 * Cribbage update functions
 *****/
void cribbage_deal(void) {
    int i = 0;
    Deck *stock = g_cribbage->decks[CR_STOCK];
    Deck *playerhand = g_cribbage->decks[CR_PLAYER];
    Deck *cpuhand = g_cribbage->decks[CR_CPU];
    Deck *crib = g_cribbage->decks[CR_CRIB];
    Card *card = NULL;
    
    // Switch who has the crib
    g_cribbage->pcrib = !g_cribbage->pcrib;
    g_cribbage->pturn = !g_cribbage->pcrib;

    // Put all the cards back in the stock
    add_deck(playerhand,stock);
    add_deck(cpuhand,stock);
    add_deck(crib,stock);

    // Shuffle the cards
    shuffle_deck(stock);
    
    // Remove flags from cards in stock
    card = stock->cards;
    while(card) {
        card->flags &= ~CD_UP;
        card->flags &= ~CD_PLAYER;
        card->flags &= ~CD_CPU;
        card = card->next;
    }

    // Give 6 cards to the player, 6 cards to the cpu
    for(i = 0; i < 6; i++) {
        draw_card(stock,playerhand);
        draw_card(stock,cpuhand);
    }

    // Engage flags on player/cpu cards
    card = playerhand->cards;
    while(card) {
        card->flags |= CD_PLAYER;
        card = card->next;
    }
    card = cpuhand->cards;
    while(card) {
        card->flags |= CD_CPU;
        card = card->next;
    }
        
    // Sort the players hand
    merge_sort_deck(playerhand);

    cribbage_msg("Choose two cards to add to %s crib:", 
            (g_cribbage->pcrib ? "your" : "the computer\'s"));
    g_cribbage->flags |= GFL_DRAW | GFL_CRIBDISC;
}

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
    Deck *board = g_cribbage->decks[CR_BOARD];
    Deck *cpuhand = g_cribbage->decks[CR_CPU];
    Card *card = NULL;
    Card *choice = NULL;
    int numcards = count_cards(cpuhand->cards);
    int i = mt_rand(0,numcards-1); // -1 because 0 indexed
    int priority = 0;
    //card = get_card_at(cpuhand,i);
    if(cribbage_check_go(cpuhand)) {
        // This should never happen.
        cribbage_msg("CPU: I uh, don't have any cards to play boss.");
        return;
    }
    card = cpuhand->cards;
    while(card) {
        if((cribbage_card_value(card->flags) + g_cribbage->count) == 31) {
            // Check to see if any of the CPU's cards can be played to make 31
            if(priority < 5) {
                priority = 5;
                choice = card;
            }
        } else if ((cribbage_card_value(card->flags) 
                    + g_cribbage->count) == 15) {
            // Check to see if any of the CPU's cards can be played to make 15
            if(priority < 4) {
                priority = 4;
                choice = card;
            }
        } else if(get_last_card(board)) {
            // Check for pairs
            if(get_rank(card->flags) == get_rank(get_last_card(board)->flags)) {
                if(priority < 2) {
                    priority = 2;
                    choice = card;
                }
            }
            // Check for runs
            // Check for cards that make 10, 21, or 5 (and don't play those)
        }
        card = card->next;
    }
    if(!choice) {
        choice = get_card_at(cpuhand,i);
        while((cribbage_card_value(choice->flags) + g_cribbage->count) > 31) {
            i = mt_rand(0,numcards-1);
            choice = get_card_at(cpuhand,i);
        }
    }
    add_card_to_deck(board, remove_card_from_deck(cpuhand,choice));
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
     * Idea: What if I used card flags? Turn cards "inactive" (CD_UP) when both players
     * are a go, use one deck for the "board" (with a flag CD_PLAYER/CD_CPU on
     * each card to show who it belonged to/where it needs to be drawn), last
     * card played would always be the head of the deck->cards list (in a shared
     * deck)... I think that solves ALL the problems and would require the least
     * amount of "new" code to implement.
     */
    Deck *board = g_cribbage->decks[CR_BOARD];
    Card *card = get_last_card(board);
    int score = 0;
    char *msg = malloc(80*sizeof(char));
    CribScore *boardscore = NULL;
    msg[0] = '\0';
    // Last card will never break 31 because both players are checked for valid
    // cards before this function is called. Theoretically. I should probably
    // add some error checking here.
    // Update count
    g_cribbage->count += cribbage_card_value(card->flags);
    // Check 15/31
    if(15 == g_cribbage->count) {
        score += 2;
        snprintf(msg,80,"15 for %d!", score);
    } else if (31 == g_cribbage->count) {
        score += 2;
        snprintf(msg,80,"31 for 2!");
    }
    // Check boardscore (runs and pairs)
    boardscore = score_cribbage_play(board);
    if(boardscore) {
        score += boardscore->pts;
    }
    // Add points
    cribbage_add_points(score,g_cribbage->pturn);
    // Format and send cribbage msg
    if(boardscore) {
        // Runs, pairs, and/or 15/31.
        boardscore->msg[0] = toupper(boardscore->msg[0]);
        cribbage_msg("%s: %d. %s %s",
            (g_cribbage->pturn ? "You" : "CPU"),
            g_cribbage->count, boardscore->msg, msg);
    } else if (score) {
        // 15/31
        cribbage_msg("%s: %s",
            (g_cribbage->pturn ? "You" : "CPU"),
            msg);
    } else {
        // Nada
        cribbage_msg("%s: %d",
            (g_cribbage->pturn ? "You" : "CPU"),
            g_cribbage->count);
    }
    free(msg);
}

bool cribbage_check_go(Deck *deck) {
    Card *cards = NULL;
    int count = g_cribbage->count;
    cards = deck->cards;
    if(!cards) return true;
    while(cards) {
        if((cribbage_card_value(cards->flags) + count) <= 31) {
             return false;
        }
        cards = cards->next;
    }
    return true;
}

void cribbage_add_points(int points, bool player) {
    // Update score and peg locations
    uint8_t *peg1 = (player ? &(g_cribbage->pegP1) : &(g_cribbage->pegC1));
    uint8_t *peg2 = (player ? &(g_cribbage->pegP2) : &(g_cribbage->pegC2));
    uint8_t *score = (player ? &(g_cribbage->pScore) : &(g_cribbage->cScore));

    if(*peg1 > *peg2) {
        *peg2 = *peg1 + points;
    } else {
        *peg1 = *peg2 + points;
    }
    *score += points;
}

void cribbage_update_play(void) {
    // Alternate turns playing cards, scoring points and increasing the count
    int i = 0, id = 0, count = 0;
    bool selected = false, player = false;
    char *msg = NULL;
    Card *pcard = NULL;
    Deck *pdeck = g_cribbage->decks[CR_PLAYER];
    Deck *cdeck = g_cribbage->decks[CR_CPU];

    // Check to see if both player and cpu hands are empty
    if((count_cards(g_cribbage->decks[CR_PLAYER]->cards) == 0) &&
            (count_cards(g_cribbage->decks[CR_CPU]->cards) == 0)) {
        g_cribbage->flags &= ~GFL_CRIBPLAY;
        g_cribbage->flags |= GFL_CRIBSHOW | GFL_DRAW;
        //Move cards from the board back to the hand
        //(This will move to cribbage_update_show())
        cribbage_prompt("Press any key to continue...");
        pcard = g_cribbage->decks[CR_BOARD]->cards;
        while(pcard) {
            remove_card_from_deck(g_cribbage->decks[CR_BOARD],pcard);
            if(check_flag(pcard->flags, CD_PLAYER)) {
                add_card_to_deck(g_cribbage->decks[CR_PLAYER],pcard);
            } else {
                add_card_to_deck(g_cribbage->decks[CR_CPU],pcard);
            }
            pcard = g_cribbage->decks[CR_BOARD]->cards;
        }
        merge_sort_deck(g_cribbage->decks[CR_PLAYER]);
        merge_sort_deck(g_cribbage->decks[CR_CPU]);
        merge_sort_deck(g_cribbage->decks[CR_CRIB]);
        return;
    }

    // Check to see if it's the player's turn,
    if(g_cribbage->pturn) {
        //Check if player has a go
        if(cribbage_check_go(pdeck)) {
            cribbage_msg("You: Go.");
            //cribbage_prompt("You: Go. (Press any key)");
            g_cribbage->pturn = false;
            g_cribbage->flags |= GFL_DRAW;
            return;
        }

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
                // Need to check to see if it's valid first
                if(cribbage_card_value(pcard->flags) + g_cribbage->count <= 31) {
                    remove_card_from_deck(g_cribbage->decks[CR_PLAYER],pcard);
                    add_card_to_deck(g_cribbage->decks[CR_BOARD],pcard);
                    cribbage_update_count();
                    g_cribbage->btns[count-1]->active = false;
                    g_cribbage->pturn = false;
                } else {
                    msg = get_card_str(pcard);
                    cribbage_msg("You can't play the %s, try again.",msg);
                    if(msg) free(msg);
                }
            }
            for(i = 0; i < 6; i++) {
                g_cribbage->btns[i]->selected = false;
            }
            g_cribbage->flags |= GFL_DRAW;
        }
    } else {
        // Have the computer play a card
        g_cribbage->flags |= GFL_DRAW;
        if(cribbage_check_go(cdeck)) {
            cribbage_msg("CPU: Go.");
            //cribbage_prompt("CPU: Go. (Press any key)");
        } else {
            cribbage_cpu_play();
            cribbage_update_count();
        }
        g_cribbage->pturn = true;
    }
    // Check to see if both players have a go
    if(cribbage_check_go(cdeck) && cribbage_check_go(pdeck)) {
        //reset count to 0, last person who played a card gets a point (31s are
        //counted in update_count)        
        pcard = get_last_card(g_cribbage->decks[CR_BOARD]);
        player = check_flag(pcard->flags, CD_PLAYER);
        if(player && g_cribbage->pturn) {
            // Player's turn and last card played was player or
            // CPU's turn and last card played was CPU
            // This give's the other player an opportunity to say "go"
            // TODO: This logic doesn't work out. Saying "go" is skipped most
            // times, but not all times. Problem is likely that this function is
            // chungus and does too many things.
            g_cribbage->pturn = !g_cribbage->pturn;
        } else {
            if(g_cribbage->count != 31) {
                cribbage_msg("%s: Last for 1.", (player ? "You" : "CPU"));
                cribbage_add_points(1,player);
            }
            g_cribbage->count = 0;
            g_cribbage->flags |= GFL_DRAW;
        }
        // Turn all cards in CR_BOARD inactive - CD_UP
        pcard = g_cribbage->decks[CR_BOARD]->cards;
        while(pcard) {
            pcard->flags |= CD_UP;
            pcard = pcard->next;
        }
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

