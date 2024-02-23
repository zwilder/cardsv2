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
#ifndef DECK_H
#define DECK_H

typedef struct Card Card;
typedef struct Deck Deck;

typedef enum {
    CD_NONE     = 1 << 0,
    CD_A        = 1 << 1,
    CD_2        = 1 << 2,
    CD_3        = 1 << 3,
    CD_4        = 1 << 4,
    CD_5        = 1 << 5,
    CD_6        = 1 << 6,
    CD_7        = 1 << 7,
    CD_8        = 1 << 8,
    CD_9        = 1 << 9,
    CD_10       = 1 << 10,
    CD_J        = 1 << 11,
    CD_Q        = 1 << 12,
    CD_K        = 1 << 13,
    CD_H        = 1 << 14,
    CD_S        = 1 << 15,
    CD_C        = 1 << 16,
    CD_D        = 1 << 17,
    CD_UP       = 1 << 18
} CardFlags;

struct Card {
    int flags;
    Card *next;
    Card *prev;
};

struct Deck {
    /*
     * Deck contains a linked list of cards, an integer count of how many cards
     * are in the deck (I guess I could properly call that a uint8_t), and an
     * integer id
     */
    Card *cards;
    uint8_t count;
    uint8_t id;
};

/*****
 * Card (Linked List) functions
 *****/
Card* create_card(int cflags);
void destroy_card(Card *card);
int count_cards(Card *card);

/*****
 * Deck functions
 *****/
Deck* create_deck(void);
void destroy_deck(Deck *deck);
void add_card_to_deck(Deck *deck, Card *card);
void fill_deck(Deck *deck);
Card* search_deck(Deck *deck, int cflags);
Card* remove_card_from_deck(Deck *deck, Card *card);
void draw_card(Deck *from, Deck *to);
void draw_cards(Deck *from, Deck *to, int n);
void add_deck(Deck *from, Deck *to);
void move_top_card_to_deck(Deck *from, Deck *to);
void move_last_card_to_deck(Deck *from, Deck *to);
Card* get_last_card(Deck *deck);
Card* get_card_at(Deck *deck, int n);

/*****
 * Deck interaction
 *****/
int get_card(int value, char suite);
bool card_hearts(int card);
bool card_diamonds(int card);
bool card_spades(int card);
bool card_clubs(int card);
bool card_black(int card);
bool card_red(int card);
bool card_alt_color(int a, int b);
bool card_same_suit(int a, int b);
bool card_in_asc_sequence(int a, int b);
char get_suite(int card);
int get_rank(int card);
int get_rank_flag(int card);
int get_suite_flag(int card);
void shuffle_deck(Deck *deck);

/*****
 * Deck drawing
 *****/
void pt_card_simple(int x, int y, Card *card);
void pt_card(int x, int y, Card *card);
void pt_card_top(int x, int y, Card *card);
void pt_card_suite(int x, int y, Card *card);
void pt_card_rank(int x, int y, Card *card);
void pt_card_left(int x, int y, Card *card);
void pt_card_back(int x, int y);
void pt_card_space(int x, int y);
void pt_card_space_suite(int x, int y, int cflags);
void pt_card_special_rank(int x, int y, int cflags, char ch);
void pt_card_title(int x, int y, char *str);

#endif //DECK_H
