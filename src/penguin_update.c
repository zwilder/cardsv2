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

void penguin_update(void) {
    int i = 0, count = 0;
    int id_a = 0, id_b = 0;

    // Check win condition
    if(check_flag(g_penguin->flags, GFL_WIN)) {
        // None of the rest of update needs to run
        return;
    }

    // Count the selected buttons
    count = 0;
    for(i = 0; i < PN_NUM_DECKS; i++) {
        if(g_penguin->btns[i]->selected) {
            count++;
            (id_a ? (id_b = i) : (id_a = i));
        }
    }

    // Check the button count
    if(0 == count) {
        // If 0, NULL "from/to" references
        g_penguin->fromref = NULL;
        g_penguin->toref = NULL;
    } else if (1 == count) {
        // If 1, set "from" reference IF there is a card in the "from" deck
        g_penguin->fromref = g_penguin->decks[id_a]; 
        if(!g_penguin->fromref->cards) {
            // No cards in "from", NULL reference deactivate button
            g_penguin->btns[id_a]->selected = false;
            g_penguin->fromref = NULL;
            solitaire_msg(g_penguin, "No card at %c to move.", 
                    g_penguin->btns[id_a]->ch);
        }
    } else if (2 == count) {
        // If 2, set "to" reference
        if(g_penguin->fromref == g_penguin->decks[id_a]) {
            g_penguin->toref = g_penguin->decks[id_b];
        } else {
            g_penguin->toref = g_penguin->decks[id_a];
        }
        // check move
        if((g_penguin->fromref->id >= PN_FND_H) &&
               (g_penguin->fromref->id <= PN_FND_S)) {
            solitaire_msg(g_penguin, "Sorry, can't move cards from the foundation.");
        }else if((g_penguin->toref->id >= PN_CELL_A) && 
                (g_penguin->toref->id <= PN_CELL_G)) {
            // Moving to cell
            // Can only have one card in each cell
            // Is cell empty?
            if(!g_penguin->toref->cards) {
                move_last_card_to_deck(g_penguin->fromref,g_penguin->toref);
                solitaire_msg(g_penguin, " ");
            } else if((g_penguin->fromref->id >= PN_CELL_A) &&
                    (g_penguin->fromref->id <= PN_CELL_G)) {
                // Moving from a cell to an occupied cell, swap the cards.
                penguin_cell_swap();
            } else {
                solitaire_msg(g_penguin, "Only one card in each cell.");
            }
        } else if ((g_penguin->toref->id >= PN_TAB_A) &&
                (g_penguin->toref->id <= PN_TAB_G)) {
            // Moving to a tableau
            penguin_tableau_move();
        } else if ((g_penguin->toref->id >= PN_FND_H) &&
                (g_penguin->toref->id <= PN_FND_S)) {
            // Moving to a foundation
            penguin_foundation_move();
        } else {
            solitaire_msg(g_penguin, "What are you trying to do?");
        }

        // Deselect buttons
        g_penguin->btns[id_a]->selected = false;
        g_penguin->btns[id_b]->selected = false;
        // NULL "from/to" refs
        g_penguin->fromref = NULL;
        g_penguin->toref = NULL;
    }

    // Check win condition
    if(!check_flag(g_penguin->flags, GFL_WIN)) {
        count = 0;
        for(i = PN_FND_H; i <= PN_FND_S; i++) {
            count += count_cards(g_penguin->decks[i]->cards); 
        }
        if(count == 52) {
            // all 52 cards are on the foundations
            g_penguin->flags |= GFL_WIN;
            g_settings->penguin_wins += 1;
            // Turn off all the buttons
            for(i = 0; i < KL_NUM_DECKS; i++) {
                g_penguin->btns[i]->selected = false;
                g_penguin->btns[i]->active = false;
            }
            g_penguin->fromref = NULL;
            g_penguin->toref = NULL;
            g_penguin->score += 50;
        }
    }
}

void penguin_foundation_move(void) {
    bool valid_move = false;
    Card *fromcard = get_last_card(g_penguin->fromref);
    Card *tocard = get_last_card(g_penguin->toref);
    Card *seqcard = NULL;
    int base = 0, i = 0, seqcount = 0;
    //Moving card from somwhere to a foundation
    //Is the foundation empty? Check to see if card is the beak.
    if(!g_penguin->toref->cards) {
        // Find base
        for(i = PN_FND_H; i <= PN_FND_S; i++) {
            if(!g_penguin->decks[i]->cards) continue;
            base = get_rank(g_penguin->decks[i]->cards->flags);
            break;
        }
        // Check fromref
        if(get_rank(fromcard->flags) == base) {
            valid_move = true;
        }
    } else if(penguin_valid_move(fromcard->flags, tocard->flags)) {
        //Is the "from" card the same suite as the "to" card?
        //Is the "from" card one rank higher than the "to" card?
        valid_move = true;
    }

    if(valid_move) {
        // Find highest card in sequence above the valid move
        seqcard = fromcard;
        while(seqcard->prev) {
            if(!penguin_valid_move(seqcard->prev->flags, seqcard->flags)) {
                break;
            }
            seqcard = seqcard->prev;
            seqcount += 1; // Keep track of how many in sequence for points
        }
        // Move one to seqcount cards "from" to "to"
        for(i = seqcount; i >= 0; i--) {
            move_last_card_to_deck(g_penguin->fromref,g_penguin->toref);
        }

        // Give points, 15 for each card, moved to the foundation
        g_penguin->score += 15 + (15*seqcount);
        solitaire_msg(g_penguin, "Moved %d cards for %d points!", seqcount+1, 
                (15 + (15*seqcount)));
         
    } else {
        solitaire_msg(g_penguin, "Invalid move, try something else.");
    }
}

void penguin_tableau_move(void) {
    bool valid_move = false;
    Card *fromcard = get_last_card(g_penguin->fromref);
    Card *tocard = get_last_card(g_penguin->toref);
    Card *seqcard = NULL;
    int highcard = penguin_find_high_card();
    if(!tocard) {
        //If tocard is NULL, is fromcard one lower than the base card?
        if(get_rank(fromcard->flags) == highcard) {
            valid_move = true;
        }
    } else if (penguin_valid_move(tocard->flags,fromcard->flags)) {
        // Check fromcard to tocard
        valid_move = true;
    } 
    // Move the card if valid
    if(valid_move) {
        if((g_penguin->fromref->id >= PN_TAB_A) &&
                (g_penguin->fromref->id <= PN_TAB_G)) {
            g_penguin->score += 5;
        }
        move_last_card_to_deck(g_penguin->fromref,g_penguin->toref);
        solitaire_msg(g_penguin, " ");
    } else {
        solitaire_msg(g_penguin, "Invalid move, try something else.");
    }
    
    if(!valid_move) {
        // Check to see if last card is in sequence, if it is compare first card of
        // sequence to tocard. 
        seqcard = fromcard;
        while(seqcard->prev) {
            //if seqcard->prev is one higher and same suit then...
            //continue, if not break
            if(!penguin_valid_move(seqcard->prev->flags, seqcard->flags)) {
                break;
            }
            seqcard = seqcard->prev; 
        }
        // if seqcard != from card then we want to test seqcard and tocard
        if(!tocard) {
            //attempting to move a sequence to a blank spot
            if(get_rank(seqcard->flags) == highcard) {
                move_chain_card(seqcard, g_penguin->fromref, g_penguin->toref);
                solitaire_msg(g_penguin, " ");
            } else {
                solitaire_msg(g_penguin, "Invalid move, high card is %d", highcard);
            }
        } else if(penguin_valid_move(tocard->flags,seqcard->flags)) {
            if((g_penguin->fromref->id >= PN_TAB_A) &&
                    (g_penguin->fromref->id <= PN_TAB_G)) {
                g_penguin->score += 5;
            }
            move_chain_card(seqcard, g_penguin->fromref, g_penguin->toref);
            solitaire_msg(g_penguin, " ");
        }
    }
}

void penguin_cell_swap(void) {
    if(!g_penguin->fromref || !g_penguin->toref) return;
    Deck *from = g_penguin->fromref;
    Deck *to = g_penguin->toref;
    Card *tmp = from->cards;
    if((from->id >= PN_CELL_A) && (from->id <= PN_CELL_G) &&
           (to->id >= PN_CELL_A) && (to->id <= PN_CELL_G)) {
        // Swap the cards
        from->cards = to->cards;
        to->cards = tmp;
        solitaire_msg(g_penguin, " ");
    } 
}

bool penguin_asc_sequence(int a, int b) {
    // In penguin, cards "loop" around, ie JQKA23 is a valid sequence. So, an
    // A is one higher than a K. Is 'a' exactly one higher than 'b'?
    if(card_in_asc_sequence(a,b)) {
        return true;
    } else if(get_rank(a) == 1 && get_rank(b) == 13) {
        return true;
    } else {
        return false;
    }
}

bool penguin_valid_move(int a, int b) {
    if(card_same_suit(a,b) && penguin_asc_sequence(a,b)) {
        return true;
    }
    return false;
}

int penguin_find_base(void) {
    int i = 0;
    for(i = PN_FND_H; i <= PN_FND_S; i++) {
        if(!g_penguin->decks[i]->cards) continue;
        return get_rank(g_penguin->decks[i]->cards->flags);
    }
    return 0; // Shouldn't reach this point
}

int penguin_find_high_card(void) {
    int result = penguin_find_base() - 1;
    if(result == 0) result = 13;
    return result;
}
