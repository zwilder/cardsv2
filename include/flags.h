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
#ifndef FLAGS_H 
#define FLAGS_H

#include <stdbool.h>

typedef enum {
    GFL_NONE        = 0,
    GFL_DRAW        = 1 << 1,
    GFL_RESTART     = 1 << 2,
    GFL_WIN         = 1 << 3,
    GFL_RUNNING     = 1 << 4,
    GFL_QTOMAIN     = 1 << 5,
    /*
     * It didn't make sense to me to add another whole set of flags for
     * Cribbage, with the same game flags above... so I added them here. 
     */
    GFL_CRIBCUT     = 1 << 6,
    GFL_CRIBDISC    = 1 << 7,
    GFL_CRIBSHOW    = 1 << 8,
    GFL_PCRIB       = 1 << 9,
    GFL_CCRIB       = 1 << 10,
    GFL_PTURN       = 1 << 11,
    GFL_CTURN       = 1 << 12,
} GameFlags;

bool check_flag(int mask, int flag);
void toggle_flag(int *mask, int flag);
void remove_flag(int *mask, int flag);
void engage_flag(int *mask, int flag);

#endif //FLAGS_H
