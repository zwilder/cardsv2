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

#ifndef DRAW_H
#define DRAW_H

extern const int SCREEN_WIDTH; /* glyph.c */
extern const int SCREEN_HEIGHT; /* glyph.c */
extern Glyph *g_screenbuf; /* draw.c */

void init_screenbuf(void);
void close_screenbuf(void);
void draw_glyph(int x, int y, Glyph g);
void draw_screen(Glyph *screen);

#endif //DRAW_H
