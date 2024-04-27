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

/*****
 * Cribbage drawing functions
 *****/

int peg_xofs(int score) {
    // This needs an adjustment for 121 point games, right now works fine for 61
    // point games TODO
    if(score > 30) {
        score -= 30;
    }
    int n = 0;
    n = score / 5;
    if(((score % 5) == 0) && (score >= 5)) n -= 1;

    return (score - 1 + n);
}

void cribbage_draw_peg(int sx, int ex, int sy, int score, uint8_t color) {
    /*
 *          1    1    2    2    3    3    4    4    5    5    6    6    7    7    8
  012345678901234567890123456789012345678901234567890123456789012345678901234567890
 5                    ┌───────────────────────────────────────┐                             
 6                    │ . .!..! ..... ..... ..... ..... ..... │ 5
 7                    │ . ..... ..... ..... ..... ..... ..... │    
 8                    │                                       │ 
 9                    │ . ..!.. ...!. ..... ..... ..... ..... │ 
10                    │ . ..... ..... ..... ..... ..... ..... │ 9
 1                    └───────────────────────────────────────┘
 */
    int xo = (g_screenW / 2) - (SCREEN_WIDTH / 2); // Standard screen size is 80x24, defined in glyph.c
    int yo = (g_screenH / 2) - (SCREEN_HEIGHT / 2);
    int x = peg_xofs(score);
    uint8_t board_bg = BRIGHT_BLACK;
    if((score > 0) && (score <= 30)) {
        // sx + 2 is the first hole 
        scr_pt_clr_char(sx+2+x+xo,sy+yo,color,board_bg,'!'); 
    } else if ((score > 30) && (score <= 60)) {
        //sy + 1 is the second row
        scr_pt_clr_char(ex-x+xo,sy+1+yo,color,board_bg,'!'); 
    } else if (score > 60) {
        scr_pt_clr_char(sx+xo,sy+1+yo,color,board_bg,'!'); 
    } else {
        // Score is 0
        scr_pt_clr_char(sx+xo,sy+yo,color,board_bg,'!'); 
    }
}

void cribbage_draw(void) {
/*
 * 
        Box drawing reference
     	0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F
U+250x	─	━	│	┃	┄	┅	┆	┇	┈	┉	┊	┋	┌	┍	┎	┏

U+251x	┐	┑	┒	┓	└	┕	┖	┗	┘	┙	┚	┛	├	┝	┞	┟

U+252x	┠	┡	┢	┣	┤	┥	┦	┧	┨	┩	┪	┫	┬	┭	┮	┯

U+253x	┰	┱	┲	┳	┴	┵	┶	┷	┸	┹	┺	┻	┼	┽	┾	┿

U+254x	╀	╁	╂	╃	╄	╅	╆	╇	╈	╉	╊	╋	╌	╍	╎	╏

U+255x	═	║	╒	╓	╔	╕	╖	╗	╘	╙	╚	╛	╜	╝	╞	╟

U+256x	╠	╡	╢	╣	╤	╥	╦	╧	╨	╩	╪	╫	╬	╭	╮	╯

U+257x	╰	╱	╲	╳	╴	╵	╶	╷	╸	╹	╺	╻	╼	╽	╾	╿

        0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F
U+258x	▀	▁	▂	▃	▄	▅	▆	▇	█	▉	▊	▋	▌	▍	▎	▏

U+259x	▐	░	▒	▓	▔	▕	▖	▗	▘	▙	▚	▛	▜	▝	▞	▟
 * ♠ u2660, ♤ u2664
 * ♥ u2665, ♡ u2661
 * ♦ u2666, ♢ u2662
 * ♣ u2663, ♧ u2667
 */
 
/*
 *          1    1    2    2    3    3    4    4    5    5    6    6    7    7    8
  012345678901234567890123456789012345678901234567890123456789012345678901234567890
 0                                                                                
 1 ╔══╗╗╗╗            ╔══╗╔══╗╔══╗                                                          
 2 ║  ║║║║            ║♦ ║║♣ ║║♦ ║                                                                       
 3 ║  ║║║║            ║ K║║ K║║ 8║                                                              
 4 ╚══╝╝╝╝            ╚══╝╚══╝╚══╝                                                               
 5                    ┌───────────────────────────────────────┐                             
 6          ╔══╗╔══╗  │ . .!..! ..... ..... ..... ..... ..... │ 5
 7          ║  ║╔══╗  │ . ..... ..... ..... ..... ..... ..... │    
 8          ║  ║║♥ ║  │     ♤        ♡        ♢        ♧      │ 
 9          ╚══╝║ 6║  │ . ..!.. ...!. ..... ..... ..... ..... │ 
10              ╚══╝  │ . ..... ..... ..... ..... ..... ..... │ 9
 1                    └───────────────────────────────────────┘
 2                                     Count: 12
 3                    ╔══╗╔══╗╔══╗                                                             
 4                    ║♣ ║║♥ ║║♣ ║                      ╔══╗╔══╗╔══╗╔══╗╔══╗╔══╗
15                    ║ Q║║ A║║ 4║                      ║  ║║  ║║  ║║  ║║  ║║  ║
 6                    ╚══╝╚══╝╚══╝                      ║  ║║  ║║  ║║  ║║  ║║  ║
 7                                                      ╚══╝╚══╝╚══╝╚══╝╚══╝╚══╝
 8                                                      [a] [b] [c] [d] [e] [f]     
 9                                                                                  
20                                                                                  
 1 Choose cards for the CPU's crib:                                                                              
 2                                                                               
 3                                                                               

 Draw Board:
  - Draw box (20,5,41,7)
  - Draw str (21,6," . ..... ..... ..... ..... ..... ..... ")
  - Draw str (21,7," . ..... ..... ..... ..... ..... ..... ")
  - Draw str (21,9," . ..... ..... ..... ..... ..... ..... ")
  - Draw str (21,10," . ..... ..... ..... ..... ..... ..... ")
 Draw Crib at:
  - If CPU: 
    pt_card_back(4,1) 
    pt_card_back(3,1) 
    pt_card_back(2,1) 
    pt_card_back(1,1) 
  - If player:
    pt_card_back(4,14) 
    pt_card_back(3,14) 
    pt_card_back(2,14) 
    pt_card_back(1,14) 
 Draw player hand at: 54,14
 Draw count at: 37,12
 Draw player table cards at: 20,13
 Draw cpu table cards: 20,1
 Draw cpu score at: 62,6
 Draw player score at: 62,10
 Draw back at 14,6
 Draw cut at 14,7
  

*/
    int xo = (g_screenW / 2) - (SCREEN_WIDTH / 2); // Standard screen size is 80x24, defined in glyph.c
    int yo = (g_screenH / 2) - (SCREEN_HEIGHT / 2);
    int x = 0, y = 0, i = 0;
    Deck *deck = NULL;
    Card *cards = NULL;
    SList *msgs = g_cribbage->msglist;
    uint8_t board_fg = WHITE; // Might be a settings option in the future?
    uint8_t board_bg = BRIGHT_BLACK;

    // Clear screen
    scr_clear();

    if(check_flag(g_cribbage->flags, GFL_CRIBSHOW)) {
        // Draw crib
        y = (g_cribbage->pcrib ? 14 : 0);
        deck = g_cribbage->decks[CR_CRIB];
        cards = deck->cards;
        x = 1;
        while(cards) {
            if(check_flag(cards->flags, CD_UP)) {
                pt_card(x+xo,y+yo,cards);
            } else {
                pt_card_back(x+xo,y+yo);
            }
            x += 4;
            cards = cards->next;
        }
        scr_pt_clr(6+xo, y+yo+4, WHITE, BLACK, "Crib");

        // Draw player hand
        deck = g_cribbage->decks[CR_PLAYER];
        cards = deck->cards;
        x = 54;
        y = 14;
        while(cards) {
            if(check_flag(cards->flags, CD_UP)) {
                pt_card(x+xo,y+yo,cards);
            } else {
                pt_card_back(x+xo,y+yo);
            }
            x += 4;
            cards = cards->next;
        }
        scr_pt_clr(4+54+xo, y+yo+4, WHITE, BLACK, "Your hand");

        // Draw cpu hand
        deck = g_cribbage->decks[CR_CPU];
        cards = deck->cards;
        x = 54;
        y = 0;
        while(cards) {
            if(check_flag(cards->flags, CD_UP)) {
                pt_card(x+xo,y+yo,cards);
            } else {
                pt_card_back(x+xo,y+yo);
            }
            x += 4;
            cards = cards->next;
        }
        scr_pt_clr(4+54+xo, y+yo+4, WHITE, BLACK, "CPU hand");

        //Draw message/prompt
        if(g_cribbage->msg) {
            scr_pt_clr(xo,19+g_cribbage->msgpos+yo,WHITE,BLACK,"%s",g_cribbage->msg);
        }
        i = 0;
        while(msgs) {
            if(msgs->next) {
                scr_pt_clr(xo,19+i+yo,WHITE,BLACK,"%s",msgs->data);
            } else {
                scr_pt_clr(xo,19+i+yo,BRIGHT_WHITE,BLACK,"%s",msgs->data);
            }
            i += 1;
            msgs = msgs->next;
        }
        /*
        // Draw CPU points
        deck = g_cribbage->decks[CR_CPU];
        cards = deck->cards;
        score = score_cribbage_hand(cards, g_cribbage->decks[CR_STOCK]->cards);
        scr_pt_clr(xo,yo+21,WHITE,BLACK,"CPU hand: %s", score->msg);
        destroy_cribscore(score);
        // Draw crib points
        deck = g_cribbage->decks[CR_CRIB];
        cards = deck->cards;
        score = score_cribbage_hand(cards, g_cribbage->decks[CR_STOCK]->cards);
        scr_pt_clr(xo,yo+22,WHITE,BLACK,"%s: %s", 
                (g_cribbage->pcrib ? "Your crib" : "CPU's Crib"),score->msg);
        destroy_cribscore(score);
        // Draw player points
        deck = g_cribbage->decks[CR_PLAYER];
        cards = deck->cards;
        score = score_cribbage_hand(cards, g_cribbage->decks[CR_STOCK]->cards);
        scr_pt_clr(xo,yo+23,WHITE,BLACK,"Your hand: %s",score->msg);
        destroy_cribscore(score);
        */
    } else {
        // Draw crib
        y = (g_cribbage->pcrib ? 14 : 0);
        deck = g_cribbage->decks[CR_CRIB];
        cards = deck->cards;
        i = 4;
        while(cards){
            pt_card_back(i+xo,y+yo);
            i -= 1;
            cards=cards->next;
        }

        // Draw player hand
        deck = g_cribbage->decks[CR_PLAYER];
        cards = deck->cards;
        x = 54;
        y = 14;
        i = 0;
        while(cards) {
            pt_card(x+xo,y+yo,cards);
            pt_button_at(g_cribbage->btns[i],x+xo,y+yo+4);
            x += 4;
            i += 1;
            cards = cards->next;
        }

        // Draw cpu hand (for testing purposes)
        /*
        deck = g_cribbage->decks[CR_CPU];
        cards = deck->cards;
        x = 54;
        y = 0;
        i = 0;
        while(cards) {
            pt_card(x+xo,y+yo,cards);
            x += 4;
            i += 1;
            cards = cards->next;
        }
        */

        // Draw table cards
        deck = g_cribbage->decks[CR_BOARD];
        cards = deck->cards;
        x = 20;
        while(cards) {
            y = (check_flag(cards->flags,CD_CPU) ? 0 : 13);
            x += 4;
            pt_card(x+xo,y+yo,cards);
            cards = cards->next;
        }

        // Draw count
        i = g_cribbage->count;
        scr_pt_clr(37+xo,12+yo,WHITE,BLACK,"Count: %d",i);

        //Draw message/prompt
        if(g_cribbage->msg) {
            scr_pt_clr(xo,19+g_cribbage->msgpos+yo,WHITE,BLACK,"%s",g_cribbage->msg);
        }
        i = 0;
        while(msgs) {
            if(msgs->next) {
                scr_pt_clr(xo,19+i+yo,WHITE,BLACK,"%s",msgs->data);
            } else {
                scr_pt_clr(xo,19+i+yo,BRIGHT_WHITE,BLACK,"%s",msgs->data);
            }
            i += 1;
            msgs = msgs->next;
        }
    }

    // Draw cut
    pt_card_back(14+xo,6+yo);
    if(!check_flag(g_cribbage->flags, GFL_CRIBDISC)) {
        pt_card(14+xo,7+yo,g_cribbage->decks[CR_STOCK]->cards);
    }

    // Draw Board
    scr_draw_box_clr(20+xo,5+yo,40,6,board_fg,board_bg);
    scr_pt_clr(21+xo,6+yo,board_fg,board_bg,
            " . ..... ..... ..... ..... ..... ..... ");
    scr_pt_clr(21+xo,7+yo,board_fg,board_bg,
            " . ..... ..... ..... ..... ..... ..... ");
    scr_pt_clr(21+xo,8+yo,board_fg,board_bg,
            "        \u2664     \u2661           \u2662     \u2667      ");
    scr_pt_clr(21+xo,9+yo,board_fg,board_bg,
            " . ..... ..... ..... ..... ..... ..... ");
    scr_pt_clr(21+xo,10+yo,board_fg,board_bg,
            " . ..... ..... ..... ..... ..... ..... ");

    //Draw CPU pegs
    //cribbage_draw_peg(22,58,6,g_cribbage->cScore,BRIGHT_RED);
    cribbage_draw_peg(22,58,6,g_cribbage->pegC2,BRIGHT_RED);
    cribbage_draw_peg(22,58,6,g_cribbage->pegC1,BRIGHT_RED);

    //Draw player pegs 
    //cribbage_draw_peg(22,58,9,g_cribbage->pScore,BRIGHT_GREEN);
    cribbage_draw_peg(22,58,9,g_cribbage->pegP2,BRIGHT_GREEN);
    cribbage_draw_peg(22,58,9,g_cribbage->pegP1,BRIGHT_GREEN);

    //Draw scores
    scr_pt_clr(62+xo,6+yo,WHITE,BLACK,"%d",g_cribbage->cScore);
    scr_pt_clr(62+xo,10+yo,WHITE,BLACK,"%d",g_cribbage->pScore);

    
    // Turn off draw flag
    g_cribbage->flags &= ~GFL_DRAW;
}

/*0         1         2         3         4         5         6         7         8
  012345678901234567890123456789012345678901234567890123456789012345678901234567890
 0     ╔══╗
 1     ║██║                                                                          
 2     ║██║
 3     ╚══╝
 4
 5     ╔═════════════════════════════════════════════════════════════════════════╗  
 6     ║ 9  .....8.....8.....7.....7.....6.....6.....5.....5.....4.....4.....  3 ║  
 7     ║. 0      5     0     5     0     5     0     5     0     5     0      5 .║  
 8     ║   \.....;.....;.....;.....;.....;.....;.....;.....;.....;.....;...../.  ║  
 9 ╔══╗║. .                                                                   . .║ 
10 ╔══╗║  .                                                                   .  ║   
 1 ║♥ ║║. .                                                                   . .║  
 2 ║ J║║  .                                                                   .  ║  
 3 ╚══╝║. ./.....1.....1.....1.....1.....|.!..!;.....;.....;.....;.....;.....\. .║  
 4     ║  9      0     0     1     1     S     5     1     1     2     2      3  ║  
 5     ║.5  .....0.....5.....0.....5.....|.....;..!..0....!5.....0.....5.....  0.║  
 6     ╚═════════════════════════════════════════════════════════════════════════╝  
 7                                                                                
 8                                                                                
 9                                                                                
20                                                                                
 1                                                                                
 2                                                                                
 3                 ,~*'`'*~,                                                      
 4                                                                                
 The large board took FOREVER to draw and then I didn't like it as much as the
 small board. Keeping it here just for fun and because future me might have the
 wild idea to make a full size board again
*/
