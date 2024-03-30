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
    int klondike_wins;
    int penguin_hs;
    int penguin_wins;
};
*/

void save_settings(void) {
    FILE *f = fopen(".cards.bin","wb+");

    if(f) {
        fwrite(&(g_settings->klondike_hs),sizeof(int),1,f);
        fwrite(&(g_settings->redcolor),sizeof(uint8_t),1,f);
        fwrite(&(g_settings->blackcolor),sizeof(uint8_t),1,f);
        fwrite(&(g_settings->deckcolor),sizeof(uint8_t),1,f);
        fwrite(&(g_settings->bgcolor),sizeof(uint8_t),1,f);
        fwrite(&(g_settings->btncolor),sizeof(uint8_t),1,f);
        fwrite(&(g_settings->btnselectcolor),sizeof(uint8_t),1,f);
        fwrite(&(g_settings->klondike_last),sizeof(int),1,f);
        fwrite(&(g_settings->klondike_wins),sizeof(int),1,f);
        fwrite(&(g_settings->penguin_hs),sizeof(int),1,f);
        fwrite(&(g_settings->penguin_last),sizeof(int),1,f);
        fwrite(&(g_settings->penguin_wins),sizeof(int),1,f);
        fwrite(&(g_settings->deckdesign),sizeof(uint8_t),1,f);
    }

    fclose(f);
}

bool load_settings(void) {
    bool success = false;
    FILE *f = fopen(".cards.bin","rb+");
    int bytesread = 0;

    if(f) {
        // These are in order of implementation, because otherwise I'd have to
        // remember to remove the .cards.bin file every time I add something new
        bytesread += fread(&(g_settings->klondike_hs), sizeof(int),1,f);
        bytesread += fread(&(g_settings->redcolor), sizeof(uint8_t),1,f);
        bytesread += fread(&(g_settings->blackcolor), sizeof(uint8_t),1,f);
        bytesread += fread(&(g_settings->deckcolor), sizeof(uint8_t),1,f);
        bytesread += fread(&(g_settings->bgcolor), sizeof(uint8_t),1,f);
        bytesread += fread(&(g_settings->btncolor), sizeof(uint8_t),1,f);
        bytesread += fread(&(g_settings->btnselectcolor), sizeof(uint8_t),1,f);
        bytesread += fread(&(g_settings->klondike_last), sizeof(int),1,f);
        bytesread += fread(&(g_settings->klondike_wins), sizeof(int),1,f);
        bytesread += fread(&(g_settings->penguin_hs), sizeof(int),1,f);
        bytesread += fread(&(g_settings->penguin_last), sizeof(int),1,f);
        bytesread += fread(&(g_settings->penguin_wins), sizeof(int),1,f);
        bytesread += fread(&(g_settings->deckdesign), sizeof(uint8_t),1,f);
        success = true;
        fclose(f);
    } else {
        g_settings->klondike_hs = 0;
        g_settings->redcolor = RED;
        g_settings->blackcolor = BRIGHT_BLACK;
        g_settings->deckcolor = BLUE;
        g_settings->deckdesign = 0;
        g_settings->bgcolor = WHITE;
        g_settings->btnselectcolor = CYAN;
        g_settings->btncolor = WHITE;
        g_settings->klondike_last = 0;
        g_settings->klondike_wins = 0;
        g_settings->penguin_hs = 0;
        g_settings->penguin_last = 0;
        g_settings->penguin_wins = 0;
    }

    return success;
}
