/*
* Cards
* Copyright (C) Zach Wilder 2022,2024
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

/*
 * Most of this file was written for the original Cards project - it worked well
 * there, and I see no reason (currently) to not reuse it.
 *
 * Modifications from the original project:
 *  - "Deck" linked list object is now "Card" linked list object
 *  - "->card" on Deck object replaced with "->flags" on Card object
 *  - count_deck() replaced with count_cards()
 *  - get_cribbage_card_value() replaced with cribbage_card_value()
 *  - get_value() replaced with get_rank()
 *  - Checking if two cards have the same suite by calling "get_card_ch(A) ==
 *    get_card_ch(B)" replaced with the much more gooder "card_same_suite(A,B)"
 *  - score_cribbage_hand returns a CribScore with the message and point value
 *    instead of printing directly on the screen. 
 *  - count_15s now prints the number of fifteens as a word and not a number
 *  - count_15s fixed to include all combinations of cards (I missed a couple in
 *    the brute force counting originally).
 */

int cribbage_card_value(int card) {
    int result = get_rank(card);
    if(result > 10) {
        return 10;
    } else {
        return result;
    }
}

CribScore* create_cribscore(int qty, int pts, char *msg,...) {
    CribScore *score = malloc(sizeof(CribScore));
    va_list args;
    va_start(args, msg);
    score->qty = qty;
    score->pts = pts;
    score->msg = malloc(80 * sizeof(char));
    vsnprintf(score->msg, 80, msg, args);
    return score;
}

void destroy_cribscore(CribScore *score) {
    if(!score) return;
    if(score->msg) {
        free(score->msg);
        score->msg = NULL;
    }
    free(score);
    score = NULL;
}

CribScore* score_cribbage_hand(Card *hand, Card *flop) {
    char *buf = malloc(80 * sizeof(char));
    buf[0] = '\0'; 
    int score = 0;
    CribScore *fifteens = count_15s(hand, flop);
    CribScore *runs = count_runs(hand, flop);
    CribScore *pairs = count_pairs(hand,flop);
    CribScore *flush = count_flush(hand,flop);
    CribScore *nobs = count_nobs(hand,flop);
    CribScore *result = NULL;
    if(fifteens) {
        score += fifteens->pts;
        strcat(buf, fifteens->msg);
    }
    if(runs) {
        score += runs->pts;
        if(buf[0] != '\0') strcat(buf, ", ");
        strcat(buf, runs->msg);
    }
    if(pairs) {
        score += pairs->pts;
        if(buf[0] != '\0') strcat(buf, ", ");
        strcat(buf, pairs->msg);
    }
    if(flush) {
        /*Need to remember to add something to check if this is the crib...
         * since we need all 5 cards to be the same for a flush to count in the
         * crib */
        score += flush->pts;
        if(buf[0] != '\0') strcat(buf, ", ");
        strcat(buf, flush->msg);
    }
    if(nobs) {
        score += nobs->pts;
        if(buf[0] != '\0') strcat(buf, ", and ");
        strcat(buf, nobs->msg);
    }
    if(score) {
        //scr_pt(0, g_screenH - 3, "%s - %d points!", buf, score);
        //snprintf(msg,80,"%s - %d points.",buf,score);
        buf[0] = toupper(buf[0]);
        result = create_cribscore(0,score,"%s - %d points!",buf,score);
    } else {
        //snprintf(msg,80,"No points.");
        result = create_cribscore(0,0,"No points!");
    }
    destroy_cribscore(fifteens);
    destroy_cribscore(runs);
    destroy_cribscore(pairs);
    destroy_cribscore(flush);
    destroy_cribscore(nobs);
    free(buf);
    return result;
}

CribScore* count_runs(Card *hand, Card *flop) {
    if(!hand) return 0;
    if(!flop) return 0;
    if(count_cards(hand) != 4) return 0;
    CribScore *result = NULL;
    uint8_t matrix[13][5] = {{ 0 }};
    int x,y,cur,prev,r,m,br,bm;
    int cards[5];
    cards[0] = hand->flags;
    cards[1] = hand->next->flags;
    cards[2] = hand->next->next->flags;
    cards[3] = hand->next->next->next->flags;
    cards[4] = flop->flags;
    for(x = 0; x < 13; x++) {
        for(y = 0; y < 5; ++y) {
            if(get_rank(cards[y]) == (x+1)) {
                if((cards[y] & CD_H) == CD_H) matrix[x][0] += 1;
                if((cards[y] & CD_C) == CD_C) matrix[x][1] += 1;
                if((cards[y] & CD_D) == CD_D) matrix[x][2] += 1;
                if((cards[y] & CD_S) == CD_S) matrix[x][3] += 1;
            }
        }
        matrix[x][4] = matrix[x][0] + matrix[x][1] + matrix[x][2] + matrix[x][3];
    }
    br = bm = r = m = 1;
    cur = 0;
    prev = matrix[0][4];
    for(x = 1; x < 13; x++) {
        cur = matrix[x][4];
        if(cur && prev) {
            r++;
            if(cur > 1) m = cur;
        } else {
            if(r > br) {
                br = r;
                bm = m;
            }
            r = 1;
            m = 1;
        }
        prev = cur;
    }
    if(r > br) {
        br = r;
        bm = m;
    }
    if(br >= 3) {
        if(bm == 1) {
            result = create_cribscore(1, br, "a run of %d for %d", br,br);
        } else if(bm == 2) {
            result = create_cribscore(bm, br*bm, 
                    "a double run of %d for %d", br,br*bm);
        } else if(bm == 3) {
            result = create_cribscore(bm, br*bm, 
                    "a triple run of %d for %d", br,br*bm);
        }
    }
    return result;
}

CribScore* count_flush(Card *hand, Card *flop) {
    if(!hand) return 0;
    if(!flop) return 0;
    if(count_cards(hand) != 4) return 0;
    CribScore *result = NULL;
    int cards[5];
    cards[0] = hand->flags;
    cards[1] = hand->next->flags;
    cards[2] = hand->next->next->flags;
    cards[3] = hand->next->next->next->flags;
    cards[4] = flop->flags;
    uint8_t matrix[4][5] = {{ 0 }};
    int x,y,n;
    for(y = 0; y < 4; y++) {
        if((cards[y] & CD_H) == CD_H) matrix[0][y] += 1;
        if((cards[y] & CD_S) == CD_S) matrix[1][y] += 1;
        if((cards[y] & CD_D) == CD_D) matrix[2][y] += 1;
        if((cards[y] & CD_C) == CD_C) matrix[3][y] += 1;
    }
    n = 1;
    for(x = 0; x < 4; x++) {
        matrix[x][4] = matrix[x][0] + matrix[x][1] + matrix[x][2] + matrix[x][3];
        if(matrix[x][4] > n) n = matrix[x][4];
    }
    if(n == 4) {
        if(card_same_suite(cards[4],cards[0])) {
            result = create_cribscore(1,5, "flush for 5");
        } else {
            result = create_cribscore(1,4, "flush for 4");
        }
    }

    return result;
}

CribScore* count_nobs(Card *hand, Card *flop) {
    if(!hand) return 0;
    if(!flop) return 0;
    if(count_cards(hand) != 4) return 0;
    CribScore *result = NULL;
    int i = 0;
    int A = hand->flags;
    int B = hand->next->flags;
    int C = hand->next->next->flags;
    int D = hand->next->next->next->flags;
    int E = flop->flags;
    if(((A & CD_J) == CD_J) && (card_same_suite(E,A))) i++;
    if(((B & CD_J) == CD_J) && (card_same_suite(E,B))) i++;
    if(((C & CD_J) == CD_J) && (card_same_suite(E,C))) i++;
    if(((D & CD_J) == CD_J) && (card_same_suite(E,D))) i++;
    if(i) {
        result = create_cribscore(1, 1, "one for his nobs!");
    }
    return result;
}

CribScore* count_pairs(Card *hand, Card *flop) {
    if(!hand) return 0;
    if(!flop) return 0;
    if(count_cards(hand) != 4) return 0;
    CribScore *result = NULL;
    int i = 0;
    int A = get_rank(hand->flags);
    int B = get_rank(hand->next->flags);
    int C = get_rank(hand->next->next->flags);
    int D = get_rank(hand->next->next->next->flags);
    int E = get_rank(flop->flags);
    if(A == B) i++;
    if(A == C) i++;
    if(A == D) i++;
    if(A == E) i++;
    if(B == C) i++;
    if(B == D) i++;
    if(B == E) i++;
    if(C == D) i++;
    if(C == E) i++;
    if(D == E) i++;
    if(i == 1) {
        result = create_cribscore(1, 2, "pair for 2");
    } else if(i == 2) {
        result = create_cribscore(2, 4, "two pairs for 4");
    } else if(i == 3) {
        result = create_cribscore(3, 6, "a pair royale for 6");
    } else if(i == 6) {
        result = create_cribscore(6, 12, "a double pair royale for 12");
    }
    return result;
}


CribScore* count_15s(Card *hand, Card *flop) {
    if(!hand) return 0;
    if(!flop) return 0;
    if(count_cards(hand) != 4) return 0;
    CribScore *result = NULL;
    char *num = malloc(80*sizeof(char));
    int i = 0;
    int A = cribbage_card_value(hand->flags);
    int B = cribbage_card_value(hand->next->flags);
    int C = cribbage_card_value(hand->next->next->flags);
    int D = cribbage_card_value(hand->next->next->next->flags);
    int E = cribbage_card_value(flop->flags);
    // Two cards
    if(A+B == 15) i++;
    if(A+C == 15) i++;
    if(A+D == 15) i++;
    if(A+E == 15) i++;
    if(B+C == 15) i++;
    if(B+D == 15) i++;
    if(B+E == 15) i++;
    if(C+D == 15) i++;
    if(C+E == 15) i++;
    if(D+E == 15) i++;
    // Three cards
    if(A+B+C == 15) i++;
    if(A+B+D == 15) i++;
    if(A+B+E == 15) i++;
    if(A+C+D == 15) i++;
    if(A+C+E == 15) i++;
    if(A+D+E == 15) i++;
    if(B+C+D == 15) i++;
    if(B+C+E == 15) i++;
    if(B+D+E == 15) i++;
    if(C+D+E == 15) i++;
    // Four cards
    if(A+B+C+D == 15) i++;
    if(A+B+C+E == 15) i++;
    if(A+B+D+E == 15) i++;
    if(A+C+D+E == 15) i++;
    if(B+C+D+E == 15) i++;
    // Five cards
    if(A+B+C+D+E == 15) i++;
    
    // Highest possible number of 15s in a single hand is eight
    switch(i) {
        case(1):
            snprintf(num,80,"one");
            break;
        case(2):
            snprintf(num,80,"two");
            break;
        case(3):
            snprintf(num,80,"three");
            break;
        case(4):
            snprintf(num,80,"four");
            break;
        case(5):
            snprintf(num,80,"five");
            break;
        case(6):
            snprintf(num,80,"six");
            break;
        case(7):
            snprintf(num,80,"seven");
            break;
        case(8):
            snprintf(num,80,"eight");
            break;
        default:
            break;
    }
    if(i == 1) {
        result = create_cribscore(1, 2, "15 for 2");
    } else if (i) {
        result = create_cribscore(i, i*2, "%s 15s for %d", num, i*2);
    }
    free(num);
    return result;
}
