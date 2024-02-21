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

#ifndef PENGUIN_H
#define PENGUIN_H

typedef enum {
    PN_TAB_A        = 0,
    PN_TAB_B,
    PN_TAB_C,
    PN_TAB_D,
    PN_TAB_E,
    PN_TAB_F,
    PN_TAB_G,
    PN_CELL_A,
    PN_CELL_B,
    PN_CELL_C,
    PN_CELL_D,
    PN_CELL_E,
    PN_CELL_F,
    PN_CELL_G,
    PN_FND_H,
    PN_FND_D,
    PN_FND_C,
    PN_FND_S,
    PN_NUM_DECKS
} PenguinDecks;

// Note, the Penguin structure is exactly the same as the Klondike structure...
// why have two? the Klondike structure could be renamed... Solitaire?
// Something. Honestly every game would have the same decks/from/to/btns/flags
// setup.
#endif //PENGUIN_H
