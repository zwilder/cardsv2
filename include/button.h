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

#ifndef BUTTON_H
#define BUTTON_H

typedef struct Button Button;

struct Button {
    char ch;
    bool active;
    bool selected;
    int x;
    int y;
    int id;
};

Button* create_button(int x, int y, int id, char ch);
void destroy_button(Button *btn);

void pt_button(Button *btn);
void mv_button(Button *btn, int x, int y);
void pt_button_at(Button *btn, int x, int y);
void toggle_button(Button *btn);

#endif //BUTTON_H
