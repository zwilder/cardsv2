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

// These should all be updated to be preceded with "KL_" at some point
typedef enum {
    KL_WASTE       =   0,
    KL_TAB_B,
    KL_TAB_C,
    KL_TAB_D,
    KL_TAB_E,
    KL_TAB_F,
    KL_TAB_G,
    KL_TAB_H,
    KL_FND_H,
    KL_FND_D,
    KL_FND_C,
    KL_FND_S,
    KL_STOCK,
    KL_NUM_DECKS
} KlondikeDecks;

extern Solitaire* g_klondike;

void klondike_init(void);
void klondike_cleanup(void);
void klondike_deal(void);
void klondike_loop(void);
void klondike_events(void);
void klondike_pause(void);
void klondike_update(void);
void klondike_check_sequence(void);
bool klondike_valid_move(int a, int b);
void klondike_draw(void);

#endif //KLONDIKE_H
