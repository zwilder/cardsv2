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
void draw_str(int x, int y, char *str);
void draw_str_vec(Vec2i a, char *str);
void draw_colorstr(int x, int y, char *str, uint8_t fg, uint8_t bg);
void draw_colorstr_vec(Vec2i a, char *str, uint8_t fg, uint8_t bg);
void draw_hline(int x, int y, int w, uint8_t color);
void draw_hline_vec(Vec2i a, int w, uint8_t color);
void draw_vline(int x, int y, int h, uint8_t color);
void draw_vline_vec(Vec2i a, int h, uint8_t color);
void draw_solid_box(int x, int y, int w, int h, uint8_t color);
void draw_solid_box_vec(Vec2i a, Vec2i d, uint8_t color);
void draw_solid_box_rect(Rect a, uint8_t color);
void draw_box(int x, int y, int w, int h, uint8_t fg, uint8_t bg);
void draw_box_vec(Vec2i a, Vec2i d, uint8_t fg, uint8_t bg);
void draw_box_rect(Rect a, uint8_t fg, uint8_t bg);
void draw_msg_box(char *msg, uint8_t fg, uint8_t bg);
char draw_cmenu(SList *menu, uint8_t fg, uint8_t bg, uint8_t boxcolor);
char draw_menu_basic(SList *menu);
char draw_menu(SList *menu, uint8_t fg, uint8_t bg);
char draw_menu_nobox(SList *menu, uint8_t fg, uint8_t bg);

#endif //DRAW_H
