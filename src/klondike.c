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

#include <sys/time.h>
#include <cards.h>

/* Basic screen layout (80x24 standard)
 *         1         2         3         4         5         6         7         8
  12345678901234567890123456789012345678901234567890123456789012345678901234567890
 1   [m]    [a]         [b]  [c]  [d]  [e]  [f]  [g]  [h]      [i]  [j]  [k]  [l]
 2   ╔══╗ ╔╔╔══╗        ╔══╗ ╔══╗ ╔══╗ ╔══╗ ╔══╗ ╔══╗ ╔══╗     ┌  ┐ ┌  ┐ ┌  ┐ ┌  ┐
 3   ║  ║ ║║║  ║        ║  ║ ╔══╗ ╔══╗ ╔══╗ ╔══╗ ╔══╗ ╔══╗      ♡    ♢    ♧    ♤
 4   ║  ║ ║║║  ║        ║  ║ ║  ║ ╔══╗ ╔══╗ ╔══╗ ╔══╗ ╔══╗ 
 5   ╚══╝ ╚╚╚══╝        ╚══╝ ║  ║ ║  ║ ╔══╗ ╔══╗ ╔══╗ ╔══╗     └  ┘ └  ┘ └  ┘ └  ┘
 6                           ╚══╝ ║  ║ ║  ║ ╔══╗ ╔══╗ ╔══╗                
 7                                ╚══╝ ║  ║ ║  ║ ╔══╗ ╔══╗      
 8                                     ╚══╝ ║  ║ ║  ║ ╔══╗             
 9                                          ╚══╝ ║  ║ ║  ║
10                                               ╚══╝ ║  ║   
 1                                                    ╚══╝  
 2
 3
 4
 5
 6
 7
 8
 9
20
 1
 2 Messages        
 3 Score: 150
 4 Stock: 21. Waste 3. Press q to exit.
           1         2         3         4         5         6         7         8
  12345678901234567890123456789012345678901234567890123456789012345678901234567890

  Stock 3,1
  Waste 8,1
  TabB  22,1
  TabC  27,1
  TabD  32,1
  TabE  37,1
  TabF  42,1
  TabG  47,1
  TabH  52,1
  FndH  61,1
  FndD  66,1
  FndC  71,1
  FndS  76,1
  Msgs  0,21
  Stat  0,23

 * Drawing reference
 * ♠ u2660, ♤ u2664
 * ♥ u2665, ♡ u2661
 * ♦ u2666, ♢ u2662
 * ♣ u2663, ♧ u2667
    ┌  ┐ u250c u2510
     
     
    └  ┘ u2514 u2518
     	0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F
U+255x	═	║	╒	╓	╔	╕	╖	╗	╘	╙	╚	╛	╜	╝	╞	╟
 */

Klondike *g_klondike = NULL;

void klondike_init(void) {
    int i = 0;
    if(g_klondike) klondike_cleanup();

    // Allocate memory for decks/buttons
    g_klondike = malloc(sizeof(Klondike));
    g_klondike->redraw = true;
    g_klondike->win = false;
    g_klondike->restart = false;
    g_klondike->score = 0;
    g_klondike->decks = malloc(NUM_DECKS * sizeof(Deck));
    g_klondike->btns = malloc(NUM_DECKS * sizeof(Button));
    for(i = 0; i < NUM_DECKS; i++) {
        g_klondike->decks[i] = create_deck();
        g_klondike->decks[i]->id = i;
        g_klondike->btns[i] = create_button(0,0,i,'a'+i);
    }

    // Put the buttons in the right spot
    g_klondike->btns[STOCK]->x = 3;
    g_klondike->btns[STOCK]->active = true;
    g_klondike->btns[WASTE]->x = 8;
    
    for(i = 0; i < 7; i++) {
        g_klondike->btns[TAB_B + i]->x = 22 + (5*i);
        g_klondike->btns[TAB_B + i]->active = true;
    }
    
    for(i = 0; i < 4; i++) {
        g_klondike->btns[FND_H + i]->x = 61 + (5*i);
        g_klondike->btns[FND_H + i]->active = true;
    }

    // Put 52 cards in the stock, and shuffle it
    fill_deck(g_klondike->decks[STOCK]);
    shuffle_deck(g_klondike->decks[STOCK]);

    // Global message is null
    g_klondike->msg = NULL;

    // Deal the cards, draw the cards, enter the loop
    klondike_deal();
    klondike_update();
    klondike_draw();
    klondike_loop();

    // Cleanup
    klondike_cleanup();

}

void klondike_cleanup(void) {
    if(!g_klondike) return;
    int i = 0;
    for(i = 0; i < NUM_DECKS; i++) {
        destroy_deck(g_klondike->decks[i]);
        destroy_button(g_klondike->btns[i]);
    }
    free(g_klondike->decks);
    free(g_klondike->btns);
    if(g_klondike->msg) {
        free(g_klondike->msg);
    }
    free(g_klondike);
    g_klondike = NULL;
}

void klondike_deal(void) {
    int i = 0, j = 0;
    Card *tmp = NULL;
    for(i = 0; i < 7; i++) {
        j = i + 1;
        // Draw cards from the deck to put on the tableau
        draw_cards(g_klondike->decks[STOCK],g_klondike->decks[TAB_B + i], j);

        // Turn the top card (last card) faceup
        tmp = g_klondike->decks[TAB_B + i]->cards;
        while(tmp->next) {
            tmp = tmp->next;
        }
        engage_flag(&(tmp->flags), CD_UP);
    }
}

void klondike_msg(char *msg,...) {
    if(g_klondike->msg) {
        free(g_klondike->msg);
        g_klondike->msg = NULL;
    }
    if(!msg) return;
    va_list args;
    va_start(args,msg);
    int i = strlen(msg) + 1;
    g_klondike->msg = malloc(sizeof(char) * i);
    vsnprintf(g_klondike->msg,i,msg,args);
    va_end(args);
}

long current_ms(void) {
    // Helper function to return the current time in ms
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return ((tp.tv_sec * 1000) + (tp.tv_usec / 1000));
}

void klondike_loop(void) {
    /* Overdesigned main loop? Probably. This is your standard,
     * events-update-render loop with a nice check to keep it at a steady
     * 30fps (otherwise it will render as fast as it can draw, and since this
     * isn't a very processor intensive program at all - it can look blinky) 
     * Update: this doesn't stop it looking blinky. Fairly certain the way I'm
     * using escape codes to clear/refresh the screen is causing the blinky.
     * Current fix is only drawing the screen when the game does something that
     * would make the display change.*/
    bool running = true;
    long prev = current_ms();
    long lag = 0, current = 0, elapsed = 0;
    long msperframe = 33; // 16ms = ~60fps, 33ms = ~30fps
    while(running) {
        current = current_ms();
        elapsed = current - prev;
        prev = current;
        lag += elapsed;

        running = klondike_events();
        klondike_update();
        if(g_klondike->redraw) {
            klondike_draw();
        }
        if(g_klondike->restart) {
            running = false;
        }
        while(lag >= msperframe) {
            lag -= msperframe;
        }
    }
    // Score check here... If score is new high score, save it
    if(g_klondike->score > g_settings->klondike_hs) {
        g_settings->klondike_hs = g_klondike->score;
        save_game();
    }
    // If the game is going to be restarted, do it here
    if(g_klondike->restart) {
        klondike_init();
    }
}

bool klondike_events(void) {
    bool running = true, redraw = false;
    char ch = kb_get_char();
    switch(ch) {
        case 'A':
        case 'a': toggle_button(g_klondike->btns[WASTE]); 
                  redraw = true; 
                  break;
        case 'B':
        case 'b': toggle_button(g_klondike->btns[TAB_B]);
                  redraw = true;
                  break;
        case 'C':
        case 'c': toggle_button(g_klondike->btns[TAB_C]);
                  redraw = true;
                  break;
        case 'D':
        case 'd': toggle_button(g_klondike->btns[TAB_D]);
                  redraw = true;
                  break;
        case 'E':
        case 'e': toggle_button(g_klondike->btns[TAB_E]);
                  redraw = true;
                  break;
        case 'F':
        case 'f': toggle_button(g_klondike->btns[TAB_F]);
                  redraw = true;
                  break;
        case 'G':
        case 'g': toggle_button(g_klondike->btns[TAB_G]);
                  redraw = true;
                  break;
        case 'H':
        case 'h': toggle_button(g_klondike->btns[TAB_H]);
                  redraw = true;
                  break;
        case 'I':
        case 'i': toggle_button(g_klondike->btns[FND_H]);
                  redraw = true;
                  break;
        case 'J':
        case 'j': toggle_button(g_klondike->btns[FND_D]);
                  redraw = true;
                  break;
        case 'K':
        case 'k': toggle_button(g_klondike->btns[FND_C]);
                  redraw = true;
                  break;
        case 'L':
        case 'l': toggle_button(g_klondike->btns[FND_S]);
                  redraw = true;
                  break;
        case 'M':
        case 'm': toggle_button(g_klondike->btns[STOCK]);
                  redraw = true;
                  break;
        case 27: 
                  settings_menu(); 
                  redraw = true; 
                  break;
        case 'Q':
        case 'q': running = false; break;
        case 'R':
        case 'r': g_klondike->restart = true; break;
        default: break;
    }
    if(redraw) {
        g_klondike->redraw = redraw;
    }
    return running;
}
