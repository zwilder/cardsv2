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

/*
struct Settings {
    uint8_t redcolor;
    uint8_t blackcolor;
    uint8_t deckcolor;
    uint8_t bgcolor;
    uint8_t btncolor;
    uint8_t btnselectcolor;
    int klondike_hs;
};
*/

void save_game(void) {
    FILE *f = fopen(".cards.bin","wb+");

    if(f) {
        fwrite(&(g_settings->klondike_hs),sizeof(int),1,f);
    }

    fclose(f);
}

bool load_game(void) {
    bool success = false;
    FILE *f = fopen(".cards.bin","rb+");
    int bytesread = 0;

    if(f) {
        bytesread += fread(&(g_settings->klondike_hs), sizeof(int),1,f);
        success = true;
        fclose(f);
    } else {
        g_settings->klondike_hs = 0;
    }

    return success;
}
