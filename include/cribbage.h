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
#ifndef CRIBBAGE_H
#define CRIBBAGE_H

typedef enum {
    CR_PLAYER           = 0,
    CR_PLAYER_BOARD,
    CR_CPU,
    CR_CPU_BOARD,
    CR_CRIB,
    CR_STOCK,
    CR_NUM_DECKS
} CribbageDecks;

typedef struct {
    uint8_t qty;
    uint8_t pts;
    char *msg;
} CribScore;

typedef struct {
    Deck **decks;
    uint8_t flags; // GameFlags defined in flags.h
    bool pcrib; // Player crib
    bool pturn; // CPU crib
    uint8_t pegP1; // Peg locations Player/Computer
    uint8_t pegP2;
    uint8_t pegC1;
    uint8_t pegC2;
    uint8_t pScore; // Player score
    uint8_t cScore; // Computer score
    Button **btns;
    char *msg; 
} Cribbage;

/*****
 * cribbage.c
 *****/
bool cribbage_init(void);
void cribbage_cleanup(void);
void cribbage_deal(void);
void cribbage_msg(char *fstr, ...);
void cribbage_loop(void);
void cribbage_events(void);
void cribbage_update(void);
void cribbage_draw(void);

/*****
 * cribscore.c
 *****/
int cribbage_card_value(int card);
int score_cribbage_hand(Card *hand, Card *flop);
CribScore* count_runs(Card *hand, Card *flop);
CribScore* count_flush(Card *hand, Card *flop);
CribScore* count_15s(Card *hand, Card *flop);
CribScore* count_pairs(Card *hand, Card *flop);
CribScore* count_nobs(Card *hand, Card *flop);

#endif //CRIBBAGE_H