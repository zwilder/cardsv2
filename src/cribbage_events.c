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

/*****
 * Cribbage event functions
 *****/

void cribbage_events(void) {
    bool redraw = false;
    bool btnselect = false;
    int i = 0;
    char ch = kb_get_char();
    switch(ch) {
        case 'a':
            i = 0;
            btnselect = true;
            redraw = true;
            break;
        case 'b':
            i = 1;
            btnselect = true;
            redraw = true;
            break;
        case 'c':
            i = 2;
            btnselect = true;
            redraw = true;
            break;
        case 'd':
            i = 3;
            btnselect = true;
            redraw = true;
            break;
        case 'e':
            i = 4;
            btnselect = true;
            redraw = true;
            break;
        case 'f':
            i = 5;
            btnselect = true;
            redraw = true;
            break;
        case 'q':
        case 'Q':
            if(cribbage_prompt("Really quit? (y/n)") == 'Y') {
                g_cribbage->flags &= ~GFL_RUNNING;
                g_cribbage->flags |= GFL_QTOMAIN;
            } else {
                redraw = true;
            }
            break;
        default: break;
    }
    if(btnselect) {
        if(g_cribbage->btns[i]->active) {
            toggle_button(g_cribbage->btns[i]);
        }
    }
    if(redraw) {
        g_cribbage->flags |= GFL_DRAW;
    }
}

