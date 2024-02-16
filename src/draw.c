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
 * A lot (most) of this file was lifted from the Goblin Caves project - and
 * since the Glyph/screen buffer style drawing setup from that project can't
 * support fancy unicode characters, this project has a weird thing where it
 * uses both the term_engine.h/c and this to draw things to the screen. The idea
 * here is that a lot of what I wrote for Goblin Caves could be used here -
 * dialog boxes, menus, etc. 
 *
 *****/

Glyph *g_screenbuf = NULL;

void init_screenbuf(void) {
    g_screenbuf = create_screen();
    clear_screen(g_screenbuf);
}

void close_screenbuf(void) {
    // This function seems pointless but for consistency it exists.
    if(!g_screenbuf) return;
    destroy_screen(g_screenbuf);
}

void draw_glyph(int x, int y, Glyph g) {
    /* Draw a glyph in the appropriate offset spot for the size of the user's
     * terminal screen */
    int dx = (g_screenW / 2) - (SCREEN_WIDTH / 2);
    int dy = (g_screenH / 2) - (SCREEN_HEIGHT / 2);
    if(g.fg >= BRIGHT_BLACK) {
        scr_set_style(ST_BOLD);
    } else {
        scr_set_style(ST_NONE);
    }
    scr_pt_clr_char(x+dx,y+dy,g.fg,g.bg,g.ch);
}

void draw_screen(Glyph *screen) {
    /* Take a standard array of Glyphs, length SCREEN_WIDTH x SCREEN_HEIGHT, and
     * render it on the screen. */
    int x, y, i;
    for(x = 0; x < SCREEN_WIDTH; x++) {
        for(y = 0; y < SCREEN_HEIGHT; y++) {
            i = get_screen_index(x,y);
            if(i > (SCREEN_WIDTH * SCREEN_HEIGHT - 1)) {
                break;
            }
            draw_glyph(x,y,screen[i]);
        }
    }
}
