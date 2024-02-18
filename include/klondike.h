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
    Deck **decks; // The "decks" (card spots) above
    Deck *fromref; // A reference to where a move originates
    Deck *toref; // A reference to where the move is going
    Button **btns; // Buttons for each deck above
    char *msg; // String pointer for messages
    bool redraw; // Flag to let the main loop know to redraw the screen
    bool restart; // Flag to restart the game
    bool win; // Flag for win condition
    bool running; // Flag for main loop
    int score; // Current game score
} Klondike;

extern Klondike* g_klondike;

void klondike_init(void);
void klondike_cleanup(void);
void klondike_deal(void);
void klondike_msg(char *msg,...);
void klondike_loop(void);
void klondike_events(void);
void klondike_pause(void);
void klondike_update(void);
void klondike_check_sequence(void);
bool klondike_valid_move(int a, int b);
void klondike_draw(void);

#endif //KLONDIKE_H
