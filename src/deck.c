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

Card* create_card(int cflags) {
    Card *card = malloc(sizeof(Card));
    card->flags = cflags;
    card->next = NULL;
    card->prev = NULL;
    return card;
}

void destroy_card(Card *card) {
    if(!card) return;
    free(card);
}

int count_cards(Card *card) {
    if(!card) return 0;
    return (1 + count_cards(card->next));
}

Deck* create_deck(void) {
    Deck *result = malloc(sizeof(Deck));
    result->cards = NULL;
    result->count = 0;
    result->id = 0;
    return result;
}

void destroy_deck(Deck *deck) {
    Card *tmp = NULL;
    while(deck->cards) {
        tmp = deck->cards;
        deck->cards = deck->cards->next;
        destroy_card(tmp);
    }
    free(deck);
}

void add_card_to_deck(Deck *deck, Card *card) {
    if(!deck || !card) return;
    Card *tmp = deck->cards;
    if(!tmp) {
        // Deck has no cards this card is the first. 
        deck->cards = card;
    } else {
        // Find the last card, and then make it's next card the new card
        while(tmp->next) {
            tmp = tmp->next;
        }
        card->prev = tmp;
        tmp->next = card;
    }
    deck->count += 1;
}

Card* remove_card_from_deck(Deck *deck, Card *card) {
    if(!deck || !card) return NULL;
    if(!deck->cards) return NULL;
    Card *result = deck->cards;
    Card *prev = NULL;
    if(result == card) {
        deck->cards = deck->cards->next;
        result->next = NULL;
        result->prev = NULL;
    } else {
        while(result != card && result) {
            prev = result;
            result = result->next;
        }
        if(result) {
            prev->next = result->next;
            if(result->next) {
                result->next->prev = prev;
            }
            result->next = NULL;
            result->prev = NULL;
        }
    }
    deck->count -= 1;
    return result;
}

void draw_card(Deck *from, Deck *to) {
    if(!from || !to) return;
    if(!from->cards) return;
    Card *topcard = from->cards;
    add_card_to_deck(to, remove_card_from_deck(from,topcard));
}

void draw_cards(Deck *from, Deck *to, int n) {
    if(!from || !to) return;
    if(!from->cards) return;
    int i = 0;
    for(i = 0; i < n; i++) {
        draw_card(from, to);
    }
}

void add_deck(Deck *from, Deck *to) {
    if(!from || !to) return;
    while(from->cards) {
        draw_card(from,to);
    }
}

void fill_deck(Deck *deck) {
    int n = 1;
    for(n = 1; n <= 13; n++) {
        add_card_to_deck(deck, create_card(get_card(n, 'h')));
        add_card_to_deck(deck, create_card(get_card(n, 'c')));
        add_card_to_deck(deck, create_card(get_card(n, 'd')));
        add_card_to_deck(deck, create_card(get_card(n, 's')));
    }
}

Card* search_deck(Deck *deck, int cflags) {
    if(!deck) return NULL;
    if(!deck->cards) return NULL;
    Card *result = deck->cards;
    //while(!check_flag(result->flags, cflags)) {
    while(result) {
        if(check_flag(result->flags,cflags)) {
            //Found what we are looking for
            break;
        }
        result = result->next;
    }
    return result;
}

void move_top_card_to_deck(Deck *from, Deck *to) {
    if(!from || !to) return;
    if(!from->cards) return;
    Card *card = remove_card_from_deck(from, from->cards);
    add_card_to_deck(to, card);
}

void move_last_card_to_deck(Deck *from, Deck *to) {
    if(!from || !to) return;
    if(!from->cards) return;
    //Card *card = remove_card_from_deck(from, from->cards);
    Card *card = from->cards;
    while(card->next) {
        card = card->next;
    }
    remove_card_from_deck(from, card);
    add_card_to_deck(to, card);
}

void move_chain_card(Card *card, Deck *from, Deck *to) {
    // Move a chain of cards, starting with "card" from "from" to "to"
    if(!card || !from || !to) return;
    Card *lasttocard = get_last_card(to);
    if(!lasttocard) {
        // Moving chain to empty deck
        // Set that deck->cards = card
        to->cards = card;
        if(card->prev) {
            // Other cards in "from"
            card->prev->next = NULL;
            card->prev = NULL;
        } else {
            // No other cards in "from"
            from->cards = NULL;
        }
        // Make sure the count in both "to" and "from" is accurate.
        to->count = count_cards(to->cards);
        from->count = count_cards(from->cards);
    } else {
        // Moving chain to another card
        // First, need to check and sever the chain from the "from" deck
        if(card->prev) {
            //Other cards in "from"
            card->prev->next = NULL;
        } else {
            //No other cards in "from", need to NULL the from->cards
            from->cards = NULL;
        }
        // Reattach chain to "to deck"
        card->prev = lasttocard;
        lasttocard->next = card;
        // Make sure the count in both "to" and "from" is accurate.
        to->count = count_cards(to->cards);
        from->count = count_cards(from->cards);
    }
}

Card* get_last_card(Deck *deck) {
    if(!deck) return NULL;
    if(!deck->cards) return NULL;
    Card *card = deck->cards;
    while(card->next) {
        card = card->next;
    }
    return card;
}

Card* get_card_at(Deck *deck, int n) {
    if(!deck) return NULL;
    if(!deck->cards) return NULL;
    int i = 0;
    Card *result = deck->cards;
    while((i < n) && result) {
        result = result->next;
        i++;
    }
    return result;
}

/*****
 * Deck interaction
 *****/
int get_card(int value, char suite) {
    /* Turns a card value and a char suite (HSDC) into a bitflag int */
    int card = CD_NONE;
    switch(value) {
        case 1: card = CD_A; break;
        case 2: card = CD_2; break;
        case 3: card = CD_3; break;
        case 4: card = CD_4; break;
        case 5: card = CD_5; break;
        case 6: card = CD_6; break;
        case 7: card = CD_7; break;
        case 8: card = CD_8; break;
        case 9: card = CD_9; break;
        case 10: card = CD_10; break;
        case 11: card = CD_J; break;
        case 12: card = CD_Q; break;
        case 13: card = CD_K; break;
        default: card = CD_NONE; break;
    }
    switch(suite) {
        case 'S':
        case 's': card |= CD_S; break;
        case 'H':
        case 'h': card |= CD_H; break;
        case 'D':
        case 'd': card |= CD_D; break;
        case 'C':
        case 'c': card |= CD_C; break;
        default: break;
    }
    return card;
}

bool card_hearts(int card) {
    return ((card & CD_H) == CD_H);
}

bool card_diamonds(int card) {
    return ((card & CD_D) == CD_D);
}

bool card_spades(int card) {
    return ((card & CD_S) == CD_S);
}

bool card_clubs(int card) {
    return ((card & CD_C) == CD_C);
}

bool card_black(int card) {
    return (card_spades(card) || card_clubs(card));
}

bool card_red(int card) {
    return (card_hearts(card) || card_diamonds(card));
}

bool card_alt_color(int a, int b) {
    // Haha, clever trick here
    return (card_black(a) != card_black(b));
}

bool card_same_suit(int a, int b) {
    return (get_suite(a) == get_suite(b));
}

bool card_in_asc_sequence(int a, int b) {
    // Is the value of card 'a' exactly one higher than card 'b'?
    return (get_rank(a) == get_rank(b) + 1);
}

char get_suite(int card) {
    char result = '\0';
    if(card_hearts(card)) {
        result = 'h';
    } else if (card_diamonds(card)) {
        result = 'd';
    } else if (card_clubs(card)) {
        result = 'c';
    } else if (card_spades(card)) {
        result = 's';
    }
    return result;
}

int get_rank(int card) {
    int result = 0;
    if(check_flag(card, CD_A)) {
        result = 1;
    } else if (check_flag(card,CD_2)) {
        result = 2;
    } else if (check_flag(card,CD_2)) {
        result = 2;
    } else if (check_flag(card,CD_3)) {
        result = 3;
    } else if (check_flag(card,CD_4)) {
        result = 4;
    } else if (check_flag(card,CD_5)) {
        result = 5;
    } else if (check_flag(card,CD_6)) {
        result = 6;
    } else if (check_flag(card,CD_7)) {
        result = 7;
    } else if (check_flag(card,CD_8)) {
        result = 8;
    } else if (check_flag(card,CD_9)) {
        result = 9;
    } else if (check_flag(card,CD_10)) {
        result = 10;
    } else if (check_flag(card,CD_J)) {
        result = 11;
    } else if (check_flag(card,CD_Q)) {
        result = 12;
    } else if (check_flag(card,CD_K)) {
        result = 13;
    }
    return result;
}

int get_rank_flag(int card) {
    // Just need to return the flag & ~(suites)
    return (card & ~(CD_H | CD_D | CD_C | CD_S));
}

int get_suite_flag(int card) {
    // Just need to return the flag & ~(rank)
    return (card & 
            ~(CD_A | CD_2 | CD_3 | CD_4 | CD_5 | CD_6 | 
              CD_7 | CD_8 | CD_9 | CD_10 | CD_J | CD_Q | CD_K));
}
void shuffle_deck(Deck *deck) {
    int rounds = 500; // Seems about right for a truly random shuffle
    int i,j,n;
    int size = deck->count;
    Card *tmp = NULL;
    for(i = 0; i < rounds; i++) {
        tmp = deck->cards;
        if(!tmp) break;
        n = mt_rand(0,size - 1);
        j = 0;
        while(j != n) {
            tmp = tmp->next;
            j++;
        }
        add_card_to_deck(deck, remove_card_from_deck(deck,tmp));
    }
}
/*****
 * Deck drawing
 *****/
/* 
 * Reference:
 * ♠ u2660, ♤ u2664
 * ♥ u2665, ♡ u2661
 * ♦ u2666, ♢ u2662
 * ♣ u2663, ♧ u2667
     	0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F
U+250x  ─   ━   │   ┃   ┄   ┅   ┆   ┇   ┈   ┉   ┊   ┋   ┌   ┍   ┎   ┏

U+251x  ┐   ┑   ┒   ┓   └   ┕   ┖   ┗   ┘   ┙   ┚   ┛   ├   ┝   ┞   ┟

U+255x	═	║	╒	╓	╔	╕	╖	╗	╘	╙	╚	╛	╜	╝	╞	╟

U+258x	▀	▁	▂	▃	▄	▅	▆	▇	█	▉	▊	▋	▌	▍	▎	▏

U+259x	▐	░	▒	▓	▔	▕	▖	▗	▘	▙	▚	▛	▜	▝	▞	▟

U+256x	╠	╡	╢	╣	╤	╥	╦	╧	╨	╩	╪	╫	╬	╭	╮	╯

╔══╗╔══╗╔══╗╔══╗╔══╗╔══╗╔══╗╔══╗╔══╗
║♥ ║║♥ ║║♥ ║║♥ ║║♥ ║║♥ ║║♥ ║║♥ ║║♥ ║
║ A║║ 2║║ 3║║ 4║║ 5║║ 6║║ 7║║ 8║║ 9║
╚══╝╚══╝╚══╝╚══╝╚══╝╚══╝╚══╝╚══╝╚══╝
╔══╗╔══╗╔══╗╔══╗
║♥ ║║♥ ║║♥ ║║♥ ║
║10║║ J║║ Q║║ K║
╚══╝╚══╝╚══╝╚══╝

╔♦Q╗
╔♣J╗
╔══╗
║♥ ║
║10║
╚══╝
╔╔╔══╗
♦♣║♥ ║
QJ║10║
╚╚╚══╝
╔══╗╔══╗╔══╗╔══╗╔══╗
║░░║║▛▜║║▗▜║║▚▞║║╠╣║
║░░║║▙▟║║▙▘║║▞▚║║╠╣║
╚══╝╚══╝╚══╝╚══╝╚══╝
u2591u2591 u259bu259c u2597u259c u259au259e
u2591u2591 u2599u259f u2599u2598 u259eu259a
┌  ┐
 
 
└  ┘
*/
void pt_card_simple(int x, int y, Card *card) {
    int cflags = card->flags;
    int fg = (card_red(cflags) ? g_settings->redcolor : g_settings->blackcolor);
    int bg = g_settings->bgcolor;
    char *fstr = malloc(8 * sizeof(char));
    char *rankstr = malloc(4 * sizeof(char));
    int rank = get_rank(cflags);
    if(13 == rank) {
        snprintf(rankstr,4,"K");
    } else if (12 == rank) {
        snprintf(rankstr,4,"Q");
    } else if (11 == rank) {
        snprintf(rankstr,4,"J");
    } else if (1 == rank) {
        snprintf(rankstr,4,"A");
    } else {
        snprintf(rankstr,4,"%d",rank);
    }
    switch(get_suite(cflags)) {
        case 'h': snprintf(fstr, 8, "\u2665%s", rankstr); break;
        case 'd': snprintf(fstr, 8, "\u2666%s", rankstr); break;
        case 'c': snprintf(fstr, 8, "\u2663%s", rankstr); break;
        case 's': snprintf(fstr, 8, "\u2660%s", rankstr); break;
        default: break;
    }
    scr_pt_clr(x,y,fg,bg,"%s",fstr);
    scr_reset();
    free(fstr);
    free(rankstr);
}

void pt_card(int x, int y, Card *card) {
    int cflags = card->flags;
    int fg = (card_red(cflags) ? g_settings->redcolor : g_settings->blackcolor);
    int bg = g_settings->bgcolor;
    char *sstr = malloc(8 * sizeof(char));
    char *rankstr = malloc(4 * sizeof(char));
    int rank = get_rank(cflags);
    if(13 == rank) {
        snprintf(rankstr,4,"K");
    } else if (12 == rank) {
        snprintf(rankstr,4,"Q");
    } else if (11 == rank) {
        snprintf(rankstr,4,"J");
    } else if (1 == rank) {
        snprintf(rankstr,4,"A");
    } else {
        snprintf(rankstr,4,"%d",rank);
    }
    switch(get_suite(cflags)) {
        case 'h': snprintf(sstr, 8, "\u2665"); break;
        case 'd': snprintf(sstr, 8, "\u2666"); break;
        case 'c': snprintf(sstr, 8, "\u2663"); break;
        case 's': snprintf(sstr, 8, "\u2660"); break;
        default: break;
    }

    scr_pt_clr(x,y,fg,bg,"\u2554\u2550\u2550\u2557");
    scr_pt_clr(x,y+1,fg,bg,"\u2551%s \u2551",sstr);
    if(10 == rank) {
        scr_pt_clr(x,y+2,fg,bg,"\u2551%s\u2551",rankstr);
    } else {
        scr_pt_clr(x,y+2,fg,bg,"\u2551 %s\u2551",rankstr);
    }
    scr_pt_clr(x,y+3,fg,bg,"\u255A\u2550\u2550\u255D");
    free(sstr);
    free(rankstr);
}

void pt_card_top(int x, int y, Card *card) {
    /*
     * Prints the top of the card for vertical stacking like:
    ╔♣10
    ╔♦Q╗
    ╔♣J╗
    ╔══╗
    ║♥ ║
    ║10║
    ╚══╝
    */
    int rank = get_rank(card->flags);
    int fg = (card_red(card->flags) ? g_settings->redcolor : g_settings->blackcolor);
    int bg = g_settings->bgcolor;
    scr_pt_clr(x,y,fg, bg, "\u2554");
    pt_card_simple(x+1,y,card);
    if(10 != rank) {
        scr_pt_clr(x+3,y,fg,bg, "\u2557");
    }
}

void pt_card_suite(int x, int y, Card *card) {
    int fg = (card_red(card->flags) ? g_settings->redcolor : g_settings->blackcolor);
    int bg = g_settings->bgcolor;
    switch(get_suite(card->flags)) {
        case 'h': scr_pt_clr(x,y,fg,bg,"\u2665"); break;
        case 'd': scr_pt_clr(x,y,fg,bg,"\u2666"); break;
        case 'c': scr_pt_clr(x,y,fg,bg,"\u2663"); break;
        case 's': scr_pt_clr(x,y,fg,bg,"\u2660"); break;
        default: break;
    }
}

void pt_card_rank(int x, int y, Card *card) {
    int rank = get_rank(card->flags);
    int fg = (card_red(card->flags) ? g_settings->redcolor : g_settings->blackcolor);
    int bg = g_settings->bgcolor;
    if(13 == rank) {
        scr_pt_clr(x,y,fg,bg,"K");
    } else if (12 == rank) {
        scr_pt_clr(x,y,fg,bg,"Q");
    } else if (11 == rank) {
        scr_pt_clr(x,y,fg,bg,"J");
    } else if (1 == rank) {
        scr_pt_clr(x,y,fg,bg,"A");
    } else {
        scr_pt_clr(x,y,fg,bg,"%d",rank);
    }
}

void pt_card_left(int x, int y, Card *card) {
    /*
     * Prints the left side of the card so the cards can be stacked liked so:
    ╔╔╔╔══╗
    ♦♦♣║♥ ║
    1QJ║10║
    0╚╚╚══╝
    */
    int rank = get_rank(card->flags);
    int fg = (card_red(card->flags) ? g_settings->redcolor : g_settings->blackcolor);
    int bg = g_settings->bgcolor;
    scr_pt_clr(x,y,fg, bg, "\u2554");
    pt_card_suite(x,y+1,card);
    if(10 != rank) {
        pt_card_rank(x,y+2,card);
        scr_pt_clr(x,y+3,fg,bg,"\u255A");
    } else {
        scr_pt_clr(x,y+2,fg,bg,"1");
        scr_pt_clr(x,y+3,fg,bg,"0");
    }
}

void pt_card_back(int x, int y) {
    /*
     * Prints the card back like this - might change this to later have
     * different backs or designs or something.
    ╔══╗╔══╗╔══╗╔══╗╔══╗
    ║░░║║▛▜║║▗▜║║▚▞║║  ║
    ║░░║║▙▟║║▙▘║║▞▚║║  ║
    ╚══╝╚══╝╚══╝╚══╝╚══╝
    u2591u2591 u259bu259c u2597u259c u259au259e
    u2591u2591 u2599u259f u2599u2598 u259eu259a
    */
    int bg = g_settings->deckcolor;
    int fg = g_settings->bgcolor;
    switch(g_settings->deckdesign) {
        case 0:
            /*╔══╗
             *║▚▞║
             *║▞▚║ 
             *╚══╝
             */
            scr_pt_clr(x,y,fg,bg,"\u2554\u2550\u2550\u2557");
            scr_pt_clr(x,y+1,fg,bg,"\u2551\u259a\u259e\u2551");
            scr_pt_clr(x,y+2,fg,bg,"\u2551\u259e\u259a\u2551");
            scr_pt_clr(x,y+3,fg,bg,"\u255A\u2550\u2550\u255D");
            break;
        case 1:
            /*╔══╗
             *║░░║
             *║░░║ 
             *╚══╝
             */
            scr_pt_clr(x,y,fg,bg,"\u2554\u2550\u2550\u2557");
            scr_pt_clr(x,y+1,fg,bg,"\u2551\u2591\u2591\u2551");
            scr_pt_clr(x,y+2,fg,bg,"\u2551\u2591\u2591\u2551");
            scr_pt_clr(x,y+3,fg,bg,"\u255A\u2550\u2550\u255D");
            break;
        case 2:
            /*╔══╗
             *║▛▜║
             *║▙▟║ 
             *╚══╝
             */
            scr_pt_clr(x,y,fg,bg,"\u2554\u2550\u2550\u2557");
            scr_pt_clr(x,y+1,fg,bg,"\u2551\u259b\u259c\u2551");
            scr_pt_clr(x,y+2,fg,bg,"\u2551\u2599\u259f\u2551");
            scr_pt_clr(x,y+3,fg,bg,"\u255A\u2550\u2550\u255D");
            break;
        case 3:
            /*╔══╗
             *║▗▜║
             *║▙▘║ 
             *╚══╝
             */
            scr_pt_clr(x,y,fg,bg,"\u2554\u2550\u2550\u2557");
            scr_pt_clr(x,y+1,fg,bg,"\u2551\u2597\u259c\u2551");
            scr_pt_clr(x,y+2,fg,bg,"\u2551\u2599\u2598\u2551");
            scr_pt_clr(x,y+3,fg,bg,"\u255A\u2550\u2550\u255D");
            break;
        case 4:
            /*╔══╗
             *║▗▜║
             *║▙▟║ 
             *╚══╝
             */
            scr_pt_clr(x,y,fg,bg,"\u2554\u2550\u2550\u2557");
            scr_pt_clr(x,y+1,fg,bg,"\u2551\u2597\u259c\u2551");
            scr_pt_clr(x,y+2,fg,bg,"\u2551\u2599\u259f\u2551");
            scr_pt_clr(x,y+3,fg,bg,"\u255A\u2550\u2550\u255D");
            break;
        default:
            /*╔══╗
             *║  ║
             *║  ║ 
             *╚══╝
             */
            scr_pt_clr(x,y,fg,bg,"\u2554\u2550\u2550\u2557");
            scr_pt_clr(x,y+1,fg,bg,"\u2551  \u2551");
            scr_pt_clr(x,y+2,fg,bg,"\u2551  \u2551");
            scr_pt_clr(x,y+3,fg,bg,"\u255A\u2550\u2550\u255D");
            break;
    }
}

void pt_card_space(int x, int y) {
    /*
     * Prints a blank space for a card
u250c u2510
u2514 u2518
    ┌  ┐
     
     
    └  ┘
    */
    scr_pt_clr(x,y,BRIGHT_BLACK,BLACK,"\u250C  \u2510");
    scr_pt_clr(x,y+3,BRIGHT_BLACK,BLACK,"\u2514  \u2518");
}

void pt_card_space_suite(int x, int y, int cflags) {
    /* 
     * As above, but with a hollow suite symbol in the space
     */
    pt_card_space(x,y);
    if(card_hearts(cflags)) {
        scr_pt_clr(x+1,y+1,BRIGHT_BLACK,BLACK,"\u2661");
    } else if(card_diamonds(cflags)) {
        scr_pt_clr(x+1,y+1,BRIGHT_BLACK,BLACK,"\u2662");
    } else if(card_spades(cflags)) {
        scr_pt_clr(x+1,y+1,BRIGHT_BLACK,BLACK,"\u2664");
    } else if(card_clubs(cflags)) {
        scr_pt_clr(x+1,y+1,BRIGHT_BLACK,BLACK,"\u2667");
    }
}

void pt_card_space_ch(int x, int y, char ch) {
    /*
     * Same as pt_card_space, but with a character in the rank
    ┌  ┐ u250c u2510
       
       
    └  ┘ u2514 u2518
     */
    pt_card_space(x,y);
    scr_pt_char(x+2,y+2,ch);
}

void pt_card_special_rank(int x, int y, int cflags, char ch) {
/*
 * ♠ u2660, ♤ u2664
 * ♥ u2665, ♡ u2661
 * ♦ u2666, ♢ u2662
 * ♣ u2663, ♧ u2667
 */
    char *suite = malloc(6 * sizeof(char));
    int fg = (card_red(cflags) ? g_settings->redcolor : g_settings->blackcolor);
    int bg = g_settings->bgcolor;
    if(card_hearts(cflags)) {
        strcpy(suite,"\u2665");
    } else if (card_diamonds(cflags)) {
        strcpy(suite,"\u2666");
    } else if (card_clubs(cflags)) {
        strcpy(suite,"\u2663");
    } else if (card_spades(cflags)) {
        strcpy(suite,"\u2660");
    }
    scr_pt_clr(x,y,fg,bg,"\u2554\u2550\u2550\u2557");
    scr_pt_clr(x,y+1,fg,bg,"\u2551%s \u2551",suite);
    scr_pt_clr(x,y+2,fg,bg,"\u2551 %c\u2551",ch);
    scr_pt_clr(x,y+3,fg,bg,"\u255A\u2550\u2550\u255D");

    free(suite);
}

void pt_card_title(int x, int y, char *str) {
    int i = 0;
    int cflags = 0;
    for(i = 0; str[i] != '\0'; i++) {
        if(mt_bool()) {
            cflags = (i % 2 == 0 ? CD_H : CD_S);
        } else {
            cflags = (i % 2 == 0 ? CD_D : CD_C);
        }
        pt_card_special_rank(x + (i * 4), y, cflags, str[i]);
    }
    pt_card_back(x-4,y);
    pt_card_back(x + (i *4),y);
}

