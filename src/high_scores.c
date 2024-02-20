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

void high_scores(void) {
    int xo = (g_screenW / 2) - (SCREEN_WIDTH / 2);
    int yo = (g_screenH / 2) - (SCREEN_HEIGHT / 2);
    char strbuf[80];

    // Clear the screen
    scr_clear(); 
    fill_screen_blank(g_screenbuf);

    // Draw the title
    pt_card_title((SCREEN_WIDTH / 2)-22+xo, yo+1, "High Scores");
    
    // Draw the scores
    snprintf(strbuf,79,
            "Klondike High Score:   %d", g_settings->klondike_hs);
    draw_colorstr(4,8,strbuf, WHITE, BLACK);
    snprintf(strbuf,79,
            "Penguin High Score:    0");
    draw_colorstr(4,9,strbuf, WHITE, BLACK);
    snprintf(strbuf,79,
            "Freecell High Score:   0");
    draw_colorstr(4,10,strbuf, WHITE, BLACK);
    snprintf(strbuf,79,
            "Cribbage Wins:         0"); // There is going to be a number here someday
    draw_colorstr(4,11,strbuf, WHITE, BLACK);

    // Prompt
    draw_colorstr((SCREEN_WIDTH / 2)-13,SCREEN_HEIGHT-1,
            "- Press any key to return -",
            BRIGHT_BLACK,BLACK);

    // Draw the screen
    draw_screen(g_screenbuf);

    // Wait for a keypress to return
    kb_get_bl_char();
}
