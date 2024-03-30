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
#ifndef CARDS_H
#define CARDS_H

/*****
 * Toolbox
 *****/
#include <mt19937.h>
#include <vec2i.h>
#include <rect.h>
#include <slist.h>
#include <glyph.h>
#include <term_engine.h>

/*****
 * Cards - Project
 *****/
#include <flags.h>
#include <deck.h>
#include <button.h>
#include <settings.h>
#include <save.h>
#include <draw.h> // Toolbox candidate
#include <color_picker.h> // Toolbox candidate

/*****
 * Cards - Games
 *****/
#include <solitaire.h> // Solitaire game struct
#include <klondike.h>
#include <penguin.h>

/*****
 * Utility and Helper functions
 *****/
void main_menu(void); // main_menu.c
void high_scores(void); // high_scores.c
void help_menu(void); // help.c
void help_penguin(void); // help.c
void help_klondike(void); // help.c
long current_ms(void); // solitaire.c

#endif //CARDS_H
