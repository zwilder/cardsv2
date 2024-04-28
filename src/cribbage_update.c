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
void cribbage_check_win(void);
void cribbage_add_points(int points, bool player);
void cribbage_update_win(void);
void cribbage_update_play(void);
void cribbage_show_points(Card *hand, bool player, char *msg);
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
        g_cribbage->flags |= GFL_DRAW;
    }
    if(astr) free(astr);
    if(bstr) free(bstr);
}

void cribbage_cpu_play(void) {
    // Have the cpu play a card from it's hand on to the table.
    // The cpu player is currently an idiot, so it just chooses a random, valid,
    // card and places it on the table. Eventually, it will choose a card
    // "intelligently" 
    // TODO: For some bizarre reason, during a test game the computer played a
    // card that brought the total to 33 before claiming a point for go. Not
    // sure how that happened and I haven't been able to duplicate it, but
    // clearly a bug.
    Deck *board = g_cribbage->decks[CR_BOARD];
    Deck *cpuhand = g_cribbage->decks[CR_CPU];
    Card *card = NULL;
    Card *choice = NULL;
    int numcards = count_cards(cpuhand->cards);
    int i = mt_rand(0,numcards-1); // -1 because 0 indexed
    int priority = 0;
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

void cribbage_check_win(void) {
    // TODO: Option for long/short game
    // TODO: SKUNKS
    bool gameover = false;
    if(g_cribbage->pScore >= 61) {
        //TODO: Add flash
        cribbage_msg("You win!");
        gameover = true;
    } else if (g_cribbage->cScore >= 61) {
        cribbage_msg("You lose!");
        gameover = true;
    }
    if(gameover) {
        g_cribbage->flags |= GFL_WIN;
    }
}

void cribbage_add_points(int points, bool player) {
    // Update score and peg locations
    uint8_t *peg1 = (player ? &(g_cribbage->pegP1) : &(g_cribbage->pegC1));
    uint8_t *peg2 = (player ? &(g_cribbage->pegP2) : &(g_cribbage->pegC2));
    uint8_t *score = (player ? &(g_cribbage->pScore) : &(g_cribbage->cScore));

    // TODO: This is silly. Fix it.
    if(*peg1 > *peg2) {
        *peg2 = *peg1 + points;
    } else {
        *peg1 = *peg2 + points;
    } 
    *score += points;
}

void cribbage_update_win(void) {
    char ch = cribbage_prompt("Another round? (y/n):");
    if(ch == 'Y') {
        // Start a new round
        g_cribbage->flags &= ~GFL_CRIBDISC;
        g_cribbage->flags &= ~GFL_CRIBPLAY;
        g_cribbage->flags &= ~GFL_CRIBSHOW;
        g_cribbage->count = 0;
        g_cribbage->pScore = 0;
        g_cribbage->cScore = 0;
        cribbage_deal();
    } else {
        g_cribbage->flags &= ~GFL_RUNNING;
        g_cribbage->flags |= GFL_QTOMAIN;
    }
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
        // Award point for last card
        pcard = get_last_card(g_cribbage->decks[CR_BOARD]);
        player = check_flag(pcard->flags, CD_PLAYER);
        if(g_cribbage->count != 31) {
            cribbage_msg("%s: 1 for last.", (player ? "You" : "CPU"));
            cribbage_add_points(1,player);
        }
        // Change state
        cribbage_prompt("Press any key to continue...");
        g_cribbage->flags &= ~GFL_CRIBPLAY;
        g_cribbage->flags |= GFL_CRIBSHOW;
        cribbage_clear_msg();
        return;
    } 
    // Check to see if both players have a go
    if(cribbage_check_go(cdeck) && cribbage_check_go(pdeck)) {
        //reset count to 0, last person who played a card gets a point (31s are
        //counted in update_count)        
        pcard = get_last_card(g_cribbage->decks[CR_BOARD]);
        player = check_flag(pcard->flags, CD_PLAYER);
        if(player && g_cribbage->pturn) {
            g_cribbage->pturn = !g_cribbage->pturn;
        } else {
            if(g_cribbage->count != 31) {
                if(count_cards(g_cribbage->decks[CR_PLAYER]->cards)) {
                    cribbage_msg("You: Go.");
                }
                if(count_cards(g_cribbage->decks[CR_CPU]->cards)) {
                    cribbage_msg("CPU: Go.");
                }
                cribbage_msg("%s: %d. 1 for last.", (player ? "You" : "CPU"), 
                        g_cribbage->count);
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
        return;
    }

    // Check to see if it's the player's turn,
    if(g_cribbage->pturn) {
        //Check if player has a go
        if(cribbage_check_go(pdeck)) {
            //cribbage_msg("You: Go.");
            cribbage_prompt("You: Go. (Press any key)");
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
    } else if(!check_flag(g_cribbage->flags, GFL_WIN)){
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

    //Check win condition
    cribbage_check_win();
}

void cribbage_show_points(Card *hand, bool player, char *msg) {
    CribScore *score = score_cribbage_hand(hand, g_cribbage->decks[CR_STOCK]->cards);
    cribbage_add_points(score->pts, player);
    cribbage_msg("%s: %s", msg, score->msg);
    cribbage_prompt("Press any key to continue...");
    destroy_cribscore(score);
}

void cribbage_flip_cards(Card *cards) {
    Card *tmp = cards;
    while(tmp) {
        tmp->flags |= CD_UP;
        tmp = tmp->next;
    }
}

void cribbage_update_show(void) {
    // Score hands
    Card *pcard = NULL;
    int i = 0;

    pcard = g_cribbage->decks[CR_BOARD]->cards;
    if(pcard) {
        //Move cards from the board back to the hand
        while(pcard) {
            remove_card_from_deck(g_cribbage->decks[CR_BOARD],pcard);
            pcard->flags &= ~CD_UP;
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

        // Award points
        if(g_cribbage->pcrib) {
            //Computer
            cribbage_flip_cards(g_cribbage->decks[CR_CPU]->cards);
            cribbage_show_points(g_cribbage->decks[CR_CPU]->cards, false, "CPU hand");
            cribbage_check_win();
            if(!check_flag(g_cribbage->flags, GFL_WIN)) {
                //Player (TODO: make player state points first/instead)
                cribbage_flip_cards(g_cribbage->decks[CR_PLAYER]->cards);
                cribbage_show_points(g_cribbage->decks[CR_PLAYER]->cards, true, "Your hand");
                cribbage_check_win();
            }
        } else {
            //Player
            cribbage_flip_cards(g_cribbage->decks[CR_PLAYER]->cards);
            cribbage_show_points(g_cribbage->decks[CR_PLAYER]->cards, true, "Your hand");
            cribbage_check_win();
            //Computer
            if(!check_flag(g_cribbage->flags, GFL_WIN)) {
                cribbage_flip_cards(g_cribbage->decks[CR_CPU]->cards);
                cribbage_show_points(g_cribbage->decks[CR_CPU]->cards, false, "CPU hand");
                cribbage_check_win();
            }
        }
        //Crib
        if(!check_flag(g_cribbage->flags, GFL_WIN)) {
            cribbage_flip_cards(g_cribbage->decks[CR_CRIB]->cards);
            cribbage_show_points(g_cribbage->decks[CR_CRIB]->cards, g_cribbage->pcrib,
                    (g_cribbage->pcrib ? "Your crib" : "CPU's Crib"));
            cribbage_check_win();
        }

        if(!check_flag(g_cribbage->flags, GFL_WIN)) {
            // Move to next round
            cribbage_clear_msg();
            for(i = 0; i < 6; i++) {
                g_cribbage->btns[i]->active = true;
            }
            g_cribbage->count = 0;
            g_cribbage->flags &= ~GFL_CRIBSHOW;
            cribbage_deal();
        }
    }
}

void cribbage_update(void) {
    // Calls the appropriate update routine for whichever state the game is in
    if(check_flag(g_cribbage->flags, GFL_WIN)) {
        cribbage_update_win();
    } else if(check_flag(g_cribbage->flags, GFL_CRIBDISC)) {
        cribbage_update_discard();
    } else if (check_flag(g_cribbage->flags, GFL_CRIBPLAY)) {
        cribbage_update_play();
    } else if (check_flag(g_cribbage->flags, GFL_CRIBSHOW)) {
        cribbage_update_show();
    }
}

