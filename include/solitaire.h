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

typedef struct {
    void (*events)(void);
    void (*update)(void);
    void (*draw)(void);
    uint8_t num_decks; // The number of decks (hands, tableaus etc)
    Deck **decks; // The "decks" (card spots) above
    Button **btns; // Buttons for each deck above
    Deck *fromref; // A reference to where a move originates
    Deck *toref; // A reference to where the move is going
    char *msg; // String pointer for messages
    uint32_t flags; // GameFlags defined in flags.h
    int score; // Current game score
} Solitaire;

Solitaire* create_solitaire(uint8_t num_decks); // Create an empty soliaire game
void destroy_solitaire(Solitaire *game); // Destroy a solitaire game
void solitaire_msg(Solitaire *g, char *msg,...);
long current_ms(void);
void solitaire_loop(Solitaire *g);
void solitaire_pause(Solitaire *g);
char solitaire_prompt(char *fstr, ...);

#endif // SOLITAIRE_H
