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


Button* create_button(int x, int y, int id, char ch) {
    Button *btn = malloc(sizeof(Button));
    btn->x = x;
    btn->y = y;
    btn->id = id;
    btn->ch = ch;
    btn->active = false;
    btn->selected = false;
    return btn;
}

void destroy_button(Button *btn) {
    if(!btn) return;
    free(btn);
    btn = NULL;
}

void pt_button(Button *btn) {
    if(!btn) return;
    int fg = (btn->active ? g_settings->btncolor : BRIGHT_BLACK);
    int bg = (btn->selected ? g_settings->btnselectcolor : BLACK);
    scr_pt_clr(btn->x,btn->y, fg, bg, "[%c]", btn->ch);
}

void mv_button(Button *btn, int x, int y) {
    if(!btn) return;
    btn->x = x;
    btn->y = y;
}

void pt_button_at(Button *btn, int x, int y) {
    if(!btn) return;
    int fg = (btn->active ? g_settings->btncolor : BRIGHT_BLACK);
    int bg = (btn->selected ? g_settings->btnselectcolor : BLACK);
    scr_pt_clr(x,y, fg, bg, "[%c]", btn->ch);

}

void toggle_button(Button *btn) {
    if(!btn->active) return;
    btn->selected = !(btn->selected);
}

