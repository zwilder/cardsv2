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
#ifndef KLONDIKE_H
#define KLONDIKE_H

typedef enum {
    WASTE       =   0,
    TAB_B,
    TAB_C,
    TAB_D,
    TAB_E,
    TAB_F,
    TAB_G,
    TAB_H,
    FND_H,
    FND_D,
    FND_C,
    FND_S,
    STOCK,
    NUM_DECKS
} KlondikeDecks;

typedef struct {
    Deck **decks; 
    Deck *fromref; 
    Deck *toref; 
    Button **btns;
    char *msg;
} Klondike;

extern Klondike* g_klondike;

void klondike_init(void);
void klondike_cleanup(void);
void klondike_deal(void);
void klondike_msg(char *msg,...);
void klondike_loop(void);
bool klondike_events(void);
void klondike_update(void);
void klondike_draw(void);

#endif //KLONDIKE_H
