/*
* Cards
* Copyright (C) Zach Wilder 2024-2025
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void save_settings(void) {
    int i = 0;
    struct stat st = {0};
    char *pathstr = malloc(180 * sizeof(char));
    snprintf(pathstr, 180, "%s/.zwsl", getenv("HOME"));
    if(stat(pathstr, &st) == -1) {
        mkdir(pathstr, 0700);
    }
    snprintf(pathstr, 180, "%s/.zwsl/cards.bin", getenv("HOME"));
    //FILE *f = fopen(".cards.bin","wb+");
    FILE *f = fopen(pathstr,"wb+");

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
        
        if(g_settings->klondike_scores) {
            for(i = 0; i < 5; i++) {
                fwrite(&(g_settings->klondike_scores[i].score),sizeof(int),1,f);
                fwrite(&(g_settings->klondike_scores[i].namelength),sizeof(int),1,f);
                fwrite(g_settings->klondike_scores[i].name,sizeof(char),
                        g_settings->klondike_scores[i].namelength,f);

                fwrite(&(g_settings->klondike_scores[i].datelength),sizeof(int),1,f);
                fwrite(g_settings->klondike_scores[i].date,sizeof(char),
                        g_settings->klondike_scores[i].datelength,f);
            }
        }
        if(g_settings->penguin_scores) {
            for(i = 0; i < 5; i++) {
                fwrite(&(g_settings->penguin_scores[i].score),sizeof(int),1,f);
                fwrite(&(g_settings->penguin_scores[i].namelength),sizeof(int),1,f);
                fwrite(g_settings->penguin_scores[i].name,sizeof(char),
                        g_settings->penguin_scores[i].namelength,f);

                fwrite(&(g_settings->penguin_scores[i].datelength),sizeof(int),1,f);
                fwrite(g_settings->penguin_scores[i].date,sizeof(char),
                        g_settings->penguin_scores[i].datelength,f);
            }
        }
        
    }

    fclose(f);
    free(pathstr);
}

bool load_settings(void) {
    bool success = false;
    char *pathstr = malloc(180 * sizeof(char));
    snprintf(pathstr, 180, "%s/.zwsl/cards.bin", getenv("HOME"));
    //FILE *f = fopen(".cards.bin","rb+");
    FILE *f = fopen(pathstr,"rb+");
    int bytesread = 0;
    int i = 0;

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
        
        // Need to create blank data to read data into.
        g_settings->klondike_scores = malloc(sizeof(Highscore) * 5);
        for(i = 0; i < 5; i++) {
            bytesread += fread(&(g_settings->klondike_scores[i].score),
                    sizeof(int), 1,f);

            // TODO probably should add some error checking here, make sure name
            // and date arent NULL
            bytesread += fread(&(g_settings->klondike_scores[i].namelength),
                    sizeof(int), 1,f);
            g_settings->klondike_scores[i].name = 
                malloc(sizeof(char) * g_settings->klondike_scores[i].namelength);
            bytesread += fread(g_settings->klondike_scores[i].name, sizeof(char),
                    g_settings->klondike_scores[i].namelength,f);
            
            bytesread += fread(&(g_settings->klondike_scores[i].datelength),
                    sizeof(int), 1,f);
            g_settings->klondike_scores[i].date = 
                malloc(sizeof(char) * g_settings->klondike_scores[i].datelength);
            bytesread += fread(g_settings->klondike_scores[i].date, sizeof(char),
                    g_settings->klondike_scores[i].datelength,f);
        }
        
        g_settings->penguin_scores = malloc(sizeof(Highscore) * 5);
        for(i = 0; i < 5; i++) {
            bytesread += fread(&(g_settings->penguin_scores[i].score),
                    sizeof(int), 1,f);

            // TODO probably should add some error checking here, make sure name
            // and date arent NULL
            bytesread += fread(&(g_settings->penguin_scores[i].namelength),
                    sizeof(int), 1,f);
            g_settings->penguin_scores[i].name = 
                malloc(sizeof(char) * g_settings->penguin_scores[i].namelength);
            bytesread += fread(g_settings->penguin_scores[i].name, sizeof(char),
                    g_settings->penguin_scores[i].namelength,f);
            
            bytesread += fread(&(g_settings->penguin_scores[i].datelength),
                    sizeof(int), 1,f);
            g_settings->penguin_scores[i].date = 
                malloc(sizeof(char) * g_settings->penguin_scores[i].datelength);
            bytesread += fread(g_settings->penguin_scores[i].date, sizeof(char),
                    g_settings->penguin_scores[i].datelength,f);
        }
        
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
        g_settings->klondike_scores = create_highscores();
        g_settings->penguin_scores = create_highscores();
    }

    free(pathstr);
    return success;
}
