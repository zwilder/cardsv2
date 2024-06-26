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

#include <flags.h>

bool check_flag(uint32_t mask, uint32_t flag) {
    /* Is flag on? */
    return((mask & flag) == flag);
}
void toggle_flag(uint32_t *mask, uint32_t flag) {
    /* Toggles flag on/off */
    *mask ^= flag;
}
void remove_flag(uint32_t *mask, uint32_t flag) {
    /* Turns off flag */
    *mask &= ~flag;
}
void engage_flag(uint32_t *mask, uint32_t flag) {
    /* Turns on flag */
    *mask |= flag;
}
