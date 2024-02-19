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

/*****
 * This is a silly include file, but it's here because this function is generic
 * enough that I could pull this out and make it a "Toolbox" file pretty
 * easily... and I might work on that soon.
 *****/
#ifndef COLOR_PICKER_H
#define COLOR_PICKER

uint8_t color_picker_menu(const uint8_t initcolor, char *prompt);

#endif //COLOR_PICKER
