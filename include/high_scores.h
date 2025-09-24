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
#ifndef HIGH_SCORES_H 
#define HIGH_SCORES_H

typedef struct Highscore Highscore;

struct Highscore {
    int score;
    int namelength;
    char *name;
    int datelength;
    char *date;
};

void high_scores(void); // Should really be called "show_high_scores" TODO
Highscore* create_highscores(void);
void cleanup_high_scores(Highscore *scores);
void add_new_score_chrono(Highscore *scores, int score);
#endif // HIGH_SCORES_H

