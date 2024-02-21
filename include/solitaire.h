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
#ifndef SOLITAIRE_H
#define SOLITAIRE_H

typedef enum {
    GFL_NONE        = 0,
    GFL_DRAW        = 1 << 1,
    GFL_RESTART     = 1 << 2,
    GFL_WIN         = 1 << 3,
    GFL_RUNNING     = 1 << 4
} GameFlags;

typedef struct {
    uint8_t num_decks; // The number of decks (hands, tableaus etc)
    Deck **decks; // The "decks" (card spots) above
    Button **btns; // Buttons for each deck above
    Deck *fromref; // A reference to where a move originates
    Deck *toref; // A reference to where the move is going
    char *msg; // String pointer for messages
    uint8_t flags; // Bitflags
    int score; // Current game score

    /* Depreciated */
    bool redraw; // Flag to let the main loop know to redraw the screen
    bool restart; // Flag to restart the game
    bool win; // Flag for win condition
    bool running; // Flag for main loop
} Solitaire;

Solitaire* create_solitaire(uint8_t num_decks); // Create an empty soliaire game
void destroy_solitaire(Solitaire *game); // Destroy a solitaire game

#endif // SOLITAIRE_H
