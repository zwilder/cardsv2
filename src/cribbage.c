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

Cribbage *g_cribbage = NULL;

bool cribbage_init(void) {
    bool ret_to_main = false;
    int i = 0;

    // Cleanup first if needed
    if(g_cribbage) cribbage_cleanup();
    
    // Allocate memory for Cribbage, decks, and buttons
    g_cribbage = malloc(sizeof(Cribbage));
    g_cribbage->decks = malloc(sizeof(Deck*) * CR_NUM_DECKS);
    for(i = 0; i < CR_NUM_DECKS; i++) {
        g_cribbage->decks[i] = create_deck();
        g_cribbage->decks[i]->id = i;
    }
    g_cribbage->btns = malloc(sizeof(Button*) * 6); //6 buttons, 1 for each card
    for(i = 0; i < 6; i++) {
        g_cribbage->btns[i] = create_button(0,0,i,'a'+i);
    }

    // Put 52 cards in the stock, and shuffle it
    fill_deck(g_cribbage->decks[CR_STOCK]);
    shuffle_deck(g_cribbage->decks[CR_STOCK]);

    // Deal the cards, draw the cards, enter the loop
    cribbage_deal();
    cribbage_draw();
    cribbage_loop();

    // If the game is going to be restarted, do it here
    if(check_flag(g_cribbage->flags,GFL_RESTART)) {
        ret_to_main = cribbage_init();
    } else {
        ret_to_main = check_flag(g_cribbage->flags, GFL_QTOMAIN);
    }

    // Cleanup
    cribbage_cleanup();
    return ret_to_main;
}

void cribbage_cleanup(void) {
    if(!g_cribbage) return;
    int i = 0;
    for(i = 0; i < CR_NUM_DECKS; i++) {
        if(g_cribbage->decks[i]) {
            destroy_deck(g_cribbage->decks[i]);
        }
    }
    if(g_cribbage->decks) {
        free(g_cribbage->decks);
        g_cribbage->decks = NULL;
    }
    
    for(i = 0; i < 6; i++) {
        if(g_cribbage->btns[i]) {
            destroy_button(g_cribbage->btns[i]);
        }
    }
    if(g_cribbage->btns) {
        free(g_cribbage->btns);
        g_cribbage->btns = NULL;
    }
}

void cribbage_deal(void) {

}

void cribbage_loop(void) {

}

void cribbage_events(void) {

}

void cribbage_update(void) {

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
*/
/*
  012345678901234567890123456789012345678901234567890123456789012345678901234567890
 0                                                                                  
 1                                                                                  
 2                                                                                  
 3                                                                                  
 4                                                                                  
 5                                                                                  
 6                                                                                  
 7                                                                                  
 8                    ┌───────────────────────────────────────┐                             
 9              ╔══╗  │ . .¡..¡ ..... ..... ..... ..... ..... │ 5 
10              ║♥ ║  │ . ..... ..... ..... ..... ..... ..... │    
 1              ║ 6║  │                                       │ 
 2              ╚══╝  │ . ..¡.. ...¡. ..... ..... ..... ..... │ 
 3                    │ . ..... ..... ..... ..... ..... ..... │ 9
 4                    └───────────────────────────────────────┘
15                                     Count: 12
 6                    ╔══╗╔══╗╔══╗                                                             
 7                    ║♣ ║║♥ ║║♣ ║                                                                 
 8                    ║ Q║║ A║║ 4║                                                                 
 9                    ╚══╝╚══╝╚══╝                                                                 
20                                                                                  
 1                                                                                  
 2                                                                                  
 3                                                                                  
*/
/*         1         2         3         4         5         6         7         8
  12345678901234567890123456789012345678901234567890123456789012345678901234567890
 1                                                ╔╔╔═══╗                                           
 2             ╔╔═══╗ ♠═K═╗ ♣═J═╗ ♥═10╗ ♥═A═╗     ║║║╔╗/║                                          
 3             ║║╔╗/║ ║   ║ ║   ║ ║   ║ ║   ║     ║║║╚║╗║                                   
 4             ║║╚║╗║ ║ K ║ ║ J ║ ║ 10║ ║ A ║     ║║║/╚╝║                                   
 5             ║║/╚╝║ ║   ║ ║   ║ ║   ║ ║   ║     ╚╚╚═══╝                                             
 6             ╚╚═══╝ ╚═K═♠ ╚═J═♣ ╚═10♥ ╚═A═♥
 7                                                                                          
 8                    ╔═══════════════════════════════════════╗                             
 9             ╔♥═6═╗ ║ . .¡..¡ ..... ..... ..... ..... ..... ║ 5 
10             ║║   ║ ║ . ..... ..... ..... ..... ..... ..... ║    
 1             ║║ 6 ║ ║                                       ║ 
 2             ║║   ║ ║ . ..¡.. ...¡. ..... ..... ..... ..... ║ 
 3             ╚╚═6═♥ ║ . ..... ..... ..... ..... ..... ..... ║ 9
 4                    ╚═══════════════════════════════════════╝
 5                                     Count: 12
 6                    ♣═Q═╗ ♥═A═╗ ♣═4═╗                            
 7                    ║   ║ ║   ║ ║   ║           ♣═4═╗ ♣═4═╗      
 8                    ║ Q ║ ║ A ║ ║ 4 ║           ║   ║ ║   ║  
 9                    ║   ║ ║   ║ ║   ║           ║ 4 ║ ║ 4 ║  
20                    ╚═Q═♣ ╚═A═♥ ╚═4═♣           ║   ║ ║   ║     
 1                                                ╚═4═♣ ╚═4═♣    
 2                                                [ a ] [ s ]
 3zwilder: 3 15's for 6, 2 runs of 3 for 6, pair for 2 - 14 points!
 4computer: Pair - 2
 *   	0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F
U+255x	═	║	╒	╓	╔	╕	╖	╗	╘	╙	╚	╛	╜	╝	╞	╟
*/
}
