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

#ifndef SETTINGS_H
#define SETTINGS_H

/*****
 * Global settings structure
 *****/
typedef struct Settings Settings;

struct Settings {
    uint8_t redcolor;
    uint8_t blackcolor;
    uint8_t deckcolor;
    uint8_t bgcolor;
    uint8_t btncolor;
    uint8_t btnselectcolor;
    int klondike_hs;
};

extern Settings *g_settings;

/*****
 * Settings functions
 *****/
void init_settings(void);
void close_settings(void);
void settings_menu(void);

#endif //SETTINGS_H
