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

/*
 * Events used here are not used anywhere else, with the exception of the
 * general 'high_scores(void)' event declared in cards.h
 */
char scores_events(void);
void scores_draw(const char scores_state);
void scores_draw_solitaire(const char gameid);
void scores_draw_penguin(void);
void scores_draw_cribbage(void);
void scores_draw_klondike(void);

/*
 * External events
 */

void high_scores(void) {
    // Using a char as a state flag, because it's quick and easy and requires no
    // other thought.
    // k - klondike
    // p - penguin
    // c - cribbage
    // q - return to main menu
    char scores_state = 'k'; 
    while(scores_state != 'q') {
        scores_draw(scores_state);
        scores_state = scores_events();
    }

    // Clear the screen
    scr_clear(); 
    fill_screen_blank(g_screenbuf);
}

Highscore* create_highscores(void) {
    Highscore *scores = malloc(sizeof(Highscore) * 5);
    int i = 0;
    for(i = 0; i < 5; i++) {
        scores[i].score = 0; 
        scores[i].namelength = 4;
        scores[i].name = malloc(sizeof(char) * 4);
        snprintf(scores[i].name,4, "...");
        scores[i].datelength = 12;
        scores[i].date = malloc(sizeof(char) * 12);
        snprintf(scores[i].date, 12, "01 Jan 1970");
    }
    return scores;
}

void cleanup_high_scores(Highscore *scores) {
    if(!scores) return;
    int i = 0;
    
    for(i = 0; i < 5; i++) {
        if(scores[i].name) {
            free(scores[i].name);
        }
        if(scores[i].date) {
            free(scores[i].date);
        }
    }
    
    free(scores);
    scores = NULL;
}

void add_new_score_chrono(Highscore *scores, int score) {
    /*
     * Add a new score to a chronological score list.
     * (Newest score is in scores[0], oldest in scores[4])
     */
    if(!scores) return;
    const char *user_env = getenv("USER");
    const char *user_str = user_env ? user_env : "Unknown";
    struct tm *timeptr;
    time_t t;
    char strbuf[180];

    // Get the current time
    t = time(NULL);
    timeptr = localtime(&t);

    // Format the date string
    if(!timeptr || strftime(strbuf, 180, "%d %b %Y", timeptr) == 0) {
        snprintf(strbuf, 180, "01 Jan 1970");
    }
      
    // Clear the memory of the bottom score in scores
    if(scores[4].name) {
        free(scores[4].name);
    }
    if(scores[4].date) {
        free(scores[4].date);
    }

    // Move everything in scores array down
    memmove(&scores[1], &scores[0], sizeof(Highscore) * 4);

    // Add latest score to top score
    scores[0].score = score;
    scores[0].namelength = strlen(user_str) + 1;
    scores[0].name = malloc(sizeof(char) * scores[0].namelength);
    snprintf(scores[0].name, scores[0].namelength, "%s", user_str);
    scores[0].datelength = strlen(strbuf) + 1;
    scores[0].date = malloc(sizeof(char) * scores[0].datelength);
    snprintf(scores[0].date, scores[0].datelength, "%s", strbuf);
}

/*
 * Internal events
 */

char scores_events(void) {
    // Wait for a keypress
    char ch = kb_get_bl_char();

    // Validate and normalize input
    // (TODO: I'm out of practice and this is janky)
    switch(ch) {
        case 'k': 
        case 'K':
            ch = 'k';
            break;
        case 'p':
        case 'P':
            ch = 'p';
            break;
        case 'c':
        case 'C':
            ch = 'c';
            break;
        case 'Q':
        default:
            ch = 'q';
            break;
    }
    return ch;
}

void scores_draw(const char scores_state) {
    int xo = (g_screenW / 2) - (SCREEN_WIDTH / 2);
    int yo = (g_screenH / 2) - (SCREEN_HEIGHT / 2);
    // Clear the screen
    scr_clear(); 
    fill_screen_blank(g_screenbuf);

    // Draw the title
    pt_card_title((SCREEN_WIDTH / 2)-22+xo, yo+1, "High Scores");

    // Draw the 'tabs' and return prompt
    draw_colorstr((SCREEN_WIDTH / 2)-18,SCREEN_HEIGHT-2,
            "[k] Klondike [p] Penguin [c] Cribbage",
            BRIGHT_BLACK,BLACK);
    draw_colorstr((SCREEN_WIDTH / 2)-13,SCREEN_HEIGHT-1,
            "- Press any other key to return -",
            BRIGHT_BLACK,BLACK);

    // Draw whichever scores are needed for the current state
    switch(scores_state) {
        case 'p':
            //scores_draw_penguin();
            scores_draw_solitaire('p');
            break;
        case 'c':
            scores_draw_cribbage();
            break;
        case 'k': 
        default:
            //scores_draw_klondike();
            scores_draw_solitaire('k');
            break;
    }

    // Draw the screen
    draw_screen(g_screenbuf);
}

void scores_draw_solitaire(const char gameid) {
    /*
     * A combination function to first take which solitaire game we're drawing,
     * and then draw it. Keeps all the solitaire games high score pages uniform
     * without having to repeat code. 
     * TODO
     */
    int i = 0;
    char strbuf[80];
    char game[10];
    int high = 0, wins = 0;
    Highscore *scores = NULL;

    switch(gameid) {
        case 'p':
            //Penguin
            snprintf(game,80,"Penguin ");
            high = g_settings->penguin_hs;
            wins = g_settings->penguin_wins;
            scores = g_settings->penguin_scores;
            draw_colorstr((SCREEN_WIDTH / 2)-5,SCREEN_HEIGHT-2,
                    "[p] Penguin ",
                    g_settings->btncolor,g_settings->btnselectcolor);
            break;
        case 'k':
            //Klondike
            snprintf(game,80,"Klondike");
            high = g_settings->klondike_hs;
            wins = g_settings->klondike_wins;
            scores = g_settings->klondike_scores;
            draw_colorstr((SCREEN_WIDTH / 2)-18,SCREEN_HEIGHT-2,
                    "[k] Klondike",
                    g_settings->btncolor,g_settings->btnselectcolor);
            break;
        default:
            // Don't continue if gameid doesnt match
            return;
    }
            
    snprintf(strbuf,79,"%s High Score:    %d", game, high);
    draw_colorstr(4,8,strbuf, WHITE, BLACK);
    snprintf(strbuf,79, "               Wins:    %d", wins);
    draw_colorstr(4,9,strbuf, WHITE, BLACK);
    snprintf(strbuf,79, "      Last 5 Scores:");
    draw_colorstr(4,10,strbuf, WHITE, BLACK);

    if(scores) {
        for(i = 0; i < 5; i++) {
            if(scores[i].score == 0) {
                snprintf(strbuf,79,
                        "                         0 - TBD");
                draw_colorstr(4,11+i,strbuf, BRIGHT_BLACK, BLACK);
                continue;
            } else if(scores[i].score < 10) {
                snprintf(strbuf,79,
                        "                         %d - %s ... %s", 
                        scores[i].score,
                        scores[i].name, 
                        scores[i].date);
            } else if (scores[i].score < 100) {
                snprintf(strbuf,79,
                        "                        %d - %s ... %s", 
                        scores[i].score,
                        scores[i].name, 
                        scores[i].date);
            } else {
                snprintf(strbuf,79,
                        "                       %d - %s ... %s", 
                        scores[i].score,
                        scores[i].name, 
                        scores[i].date);
            }
            draw_colorstr(4,11+i,strbuf, WHITE, BLACK);
        }
    }
}

void scores_draw_penguin(void) {
    /*
     * Draw the Penguin high score and last 5 scores.
     * This function (and the klondike score fucntion) are almost identical and
     * could (probably) be combined into a single function with some logic.
     * TODO
     */
    int i = 0;
    char strbuf[80];

    snprintf(strbuf,79,
            "Penguin High Score:    %d", g_settings->penguin_hs);
    draw_colorstr(4,8,strbuf, WHITE, BLACK);
    snprintf(strbuf,79,
            "              Wins:    %d", g_settings->penguin_wins);
    draw_colorstr(4,9,strbuf, WHITE, BLACK);
    snprintf(strbuf,79,
            "      Last 5 Scores:");
    draw_colorstr(4,10,strbuf, WHITE, BLACK);

    if(g_settings->penguin_scores) {
        for(i = 0; i < 5; i++) {
            if(g_settings->penguin_scores[i].score < 10) {
                snprintf(strbuf,79,
                        "                         %d - %s ... %s", 
                        g_settings->penguin_scores[i].score,
                        g_settings->penguin_scores[i].name, 
                        g_settings->penguin_scores[i].date);
            } else if (g_settings->penguin_scores[i].score < 100) {
                snprintf(strbuf,79,
                        "                        %d - %s ... %s", 
                        g_settings->penguin_scores[i].score,
                        g_settings->penguin_scores[i].name, 
                        g_settings->penguin_scores[i].date);
            } else {
                snprintf(strbuf,79,
                        "                       %d - %s ... %s", 
                        g_settings->penguin_scores[i].score,
                        g_settings->penguin_scores[i].name, 
                        g_settings->penguin_scores[i].date);
            }
            draw_colorstr(4,11+i,strbuf, WHITE, BLACK);
        }
    }
    
    // Highlight the current 'tab'
    draw_colorstr((SCREEN_WIDTH / 2)-5,SCREEN_HEIGHT-2,
            "[p] Penguin ",
            g_settings->btncolor,g_settings->btnselectcolor);

}

void scores_draw_cribbage(void) {
    //int xo = (g_screenW / 2) - (SCREEN_WIDTH / 2);
    //int yo = (g_screenH / 2) - (SCREEN_HEIGHT / 2);
    char strbuf[80];

    // Cribbage 
    snprintf(strbuf,79,
            "Cribbage Wins:   [In progress]");
    draw_colorstr(4,8,strbuf, WHITE, BLACK);

    // Highlight the current 'tab'
    draw_colorstr((SCREEN_WIDTH / 2)+7,SCREEN_HEIGHT-2,
            "[c] Cribbage ",
            g_settings->btncolor,g_settings->btnselectcolor);
}

void scores_draw_klondike(void) {
    //int xo = (g_screenW / 2) - (SCREEN_WIDTH / 2);
    //int yo = (g_screenH / 2) - (SCREEN_HEIGHT / 2);
    char strbuf[80];
    int i = 0;
    // Klondike
    snprintf(strbuf,79,
            "Klondike High Score:   %d", g_settings->klondike_hs);
    draw_colorstr(4,8,strbuf, WHITE, BLACK);
    snprintf(strbuf,79,
            "               Wins:   %d", g_settings->klondike_wins);
    draw_colorstr(4,9,strbuf, WHITE, BLACK);
    snprintf(strbuf,79,
            "      Last 5 Scores:");
    draw_colorstr(4,10,strbuf, WHITE, BLACK);
    
    if(g_settings->klondike_scores) {
        for(i = 0; i < 5; i++) {
            if(g_settings->klondike_scores[i].score < 10) {
                snprintf(strbuf,79,
                        "                         %d - %s ... %s", 
                        g_settings->klondike_scores[i].score,
                        g_settings->klondike_scores[i].name, 
                        g_settings->klondike_scores[i].date);
            } else if (g_settings->klondike_scores[i].score < 100) {
                snprintf(strbuf,79,
                        "                        %d - %s ... %s", 
                        g_settings->klondike_scores[i].score,
                        g_settings->klondike_scores[i].name, 
                        g_settings->klondike_scores[i].date);
            } else {
                snprintf(strbuf,79,
                        "                       %d - %s ... %s", 
                        g_settings->klondike_scores[i].score,
                        g_settings->klondike_scores[i].name, 
                        g_settings->klondike_scores[i].date);
            }
            draw_colorstr(4,11+i,strbuf, WHITE, BLACK);
        }
    }
    

    // Highlight the current 'tab'
    draw_colorstr((SCREEN_WIDTH / 2)-18,SCREEN_HEIGHT-2,
            "[k] Klondike",
            g_settings->btncolor,g_settings->btnselectcolor);
}

void high_scores_old(void) {
    int xo = (g_screenW / 2) - (SCREEN_WIDTH / 2);
    int yo = (g_screenH / 2) - (SCREEN_HEIGHT / 2);
    char strbuf[80];

    // Clear the screen
    scr_clear(); 
    fill_screen_blank(g_screenbuf);

    // Draw the title
    pt_card_title((SCREEN_WIDTH / 2)-22+xo, yo+1, "High Scores");
    
    // Draw the scores
    // Klondike
    snprintf(strbuf,79,
            "Klondike High Score:   %d", g_settings->klondike_hs);
    draw_colorstr(4,8,strbuf, WHITE, BLACK);
    snprintf(strbuf,79,
            "Wins:   %d", g_settings->klondike_wins);
    draw_colorstr((SCREEN_WIDTH / 2),8,strbuf, WHITE, BLACK);
    // Penguin
    snprintf(strbuf,79,
            "Penguin High Score:    %d", g_settings->penguin_hs);
    draw_colorstr(4,9,strbuf, WHITE, BLACK);
    snprintf(strbuf,79,
            "Wins:   %d", g_settings->penguin_wins);
    draw_colorstr((SCREEN_WIDTH / 2),9,strbuf, WHITE, BLACK);
    // Freecell
    snprintf(strbuf,79,
            "Freecell High Score:   0");
    draw_colorstr(4,10,strbuf, WHITE, BLACK);
    // Cribbage
    snprintf(strbuf,79,
            "Cribbage Wins:         0");
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
