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
#include <ctype.h>

Cribbage *g_cribbage = NULL;

/*****
 * Init, cleanup, deal, and loop functions
 *****/
bool cribbage_init(void) {
    bool ret_to_main = true;
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
        g_cribbage->btns[i]->active = true;
    }

    // Set g_cribbage variables to initial values
    g_cribbage->pegP1 = 0;
    g_cribbage->pegP2 = 0;
    g_cribbage->pegC1 = 0;
    g_cribbage->pegC2 = 0;
    g_cribbage->pScore = 0;
    g_cribbage->cScore = 0;
    g_cribbage->count = 0;
    g_cribbage->msg = NULL;

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
        if(!g_cribbage->decks) break;
        if(g_cribbage->decks[i]) {
            destroy_deck(g_cribbage->decks[i]);
        }
    }
    if(g_cribbage->decks) {
        free(g_cribbage->decks);
        g_cribbage->decks = NULL;
    }
    
    for(i = 0; i < 6; i++) {
        if(!g_cribbage->btns) break;
        if(g_cribbage->btns[i]) {
            destroy_button(g_cribbage->btns[i]);
        }
    }
    if(g_cribbage->btns) {
        free(g_cribbage->btns);
        g_cribbage->btns = NULL;
    }

    if(g_cribbage->msg) {
        free(g_cribbage->msg);
        g_cribbage->msg = NULL;
    }

    if(g_cribbage) {
        free(g_cribbage);
        g_cribbage = NULL;
    }
}

void cribbage_deal(void) {
    int i = 0;
    Deck *stock = g_cribbage->decks[CR_STOCK];
    Deck *playerhand = g_cribbage->decks[CR_PLAYER];
    Deck *cpuhand = g_cribbage->decks[CR_CPU];
    g_cribbage->pcrib = false; // May eventually have a "cut the deck" see who goes first thing
    g_cribbage->pturn = false;

    // Give 6 cards to the player, 6 cards to the cpu
    for(i = 0; i < 6; i++) {
        draw_card(stock,playerhand);
        draw_card(stock,cpuhand);
    }
        
    cribbage_msg("Choose two cards to add to %s crib:", 
            (g_cribbage->pcrib ? "your" : "the computer\'s"));
    g_cribbage->flags |= GFL_DRAW | GFL_CRIBDISC;
}

void cribbage_loop(void) {
    /* Basic game loop, copied from the solitaire games. Since this isn't
     * processor intensive, this keeps it from running as fast as it can - which
     * may help on some terminal emulators to keep it from being "blinky". Also,
     * it only redraws the screen when something changes, instead of every loop.
     * Another "improvement" would be to only call the update routine when it
     * needs to as well... maybe.
     */
    long prev = current_ms();
    long lag = 0, current = 0, elapsed = 0;
    long msperframe = 33; // 16ms = ~60fps, 33ms = ~30fps
    g_cribbage->flags |= GFL_RUNNING;
    while(check_flag(g_cribbage->flags, GFL_RUNNING)) {
        current = current_ms();
        elapsed = current - prev;
        prev = current;
        lag += elapsed;
        cribbage_events();
        cribbage_update();
        if(check_flag(g_cribbage->flags,GFL_DRAW)) {
            cribbage_draw();
        }
        if(check_flag(g_cribbage->flags,GFL_RESTART)) {
            g_cribbage->flags &= ~GFL_RUNNING;
        }
        while(lag >= msperframe) {
            lag -= msperframe;
        }
    }
}

void cribbage_msg(char *fstr, ...) {
    if(g_cribbage->msg) {
        free(g_cribbage->msg);
        g_cribbage->msg = NULL;
    }
    if(!fstr) return;
    va_list args;
    va_start(args,fstr);
    int i = SCREEN_WIDTH - 1;
    g_cribbage->msg = malloc(sizeof(char) * i);
    vsnprintf(g_cribbage->msg,i,fstr,args);
    va_end(args);
}
/*****
 * Event functions (will be moved to cribbage_events.c, eventually)
 *****/

void cribbage_events(void) {
    bool redraw = false;
    bool btnselect = false;
    int i = 0;
    char ch = kb_get_char();
    switch(ch) {
        case 'a':
            i = 0;
            btnselect = true;
            redraw = true;
            break;
        case 'b':
            i = 1;
            btnselect = true;
            redraw = true;
            break;
        case 'c':
            i = 2;
            btnselect = true;
            redraw = true;
            break;
        case 'd':
            i = 3;
            btnselect = true;
            redraw = true;
            break;
        case 'e':
            i = 4;
            btnselect = true;
            redraw = true;
            break;
        case 'f':
            i = 5;
            btnselect = true;
            redraw = true;
            break;
        case 'q':
        case 'Q':
            g_cribbage->flags &= ~GFL_RUNNING;
            g_cribbage->flags |= GFL_QTOMAIN;
            break;
        default: break;
    }
    if(btnselect) {
        if(g_cribbage->btns[i]->active) {
            toggle_button(g_cribbage->btns[i]);
        }
    }
    if(redraw) {
        g_cribbage->flags |= GFL_DRAW;
    }
}

/*****
 * Update functions (will be moved to cribbage_update.c, eventually)
 *****/
char cribbage_prompt(char *fstr, ...) {
    // Draw a prompt at the bottom of the screen, and wait for the user to
    // return a keypress
    char result = '\0';
    if(!fstr) return result;

    // Draw a blank line to "erase" the bottom of the screen
    char *prompt = malloc(SCREEN_WIDTH * sizeof(char));
    memset(prompt, ' ', SCREEN_WIDTH - 2);
    prompt[SCREEN_WIDTH - 1] = '\0';
    scr_pt_clr(0,23,WHITE,BLACK,prompt); 

    // Format the prompt
    va_list args;
    va_start(args,fstr);
    vsnprintf(prompt,SCREEN_WIDTH,fstr,args);
    va_end(args);
    // Print the prompt
    scr_pt_clr(0,23,WHITE,BLACK,prompt);

    // Show the blinking cursor
    // (Doesn't work)
    // Should have a show/hide in term_engine TODO
    //printf("\x1b[?25h\x1b[1 q"); 

    // Get a char from the user
    result = kb_get_bl_char();

    // Hide the cursor
    //printf("\x1b[?25l\x1b[0 q");

    // Change result to upper case
    if(result) {
        result = toupper(result);
    }

    free(prompt);
    return result;
}

void cribbage_cpu_to_crib(void) {
    // Have the computer choose two cards to add to the crib.
    // The computer player is an idiot right now, and just chooses two random
    // cards from their hand. Eventually, it will analyze it's hand and choose
    // appropriate discards for it's skill level. 
    int i = mt_rand(0,5);
    Deck *crib = g_cribbage->decks[CR_CRIB];
    Deck *cpuhand = g_cribbage->decks[CR_CPU];
    add_card_to_deck(crib, 
            remove_card_from_deck(cpuhand,
                get_card_at(cpuhand,i)));
    i = mt_rand(0,4);
    add_card_to_deck(crib, 
            remove_card_from_deck(cpuhand,
                get_card_at(cpuhand,i)));
}

void cribbage_update_discard(void) {
    int i = 0, count = 0;
    int id_a = 0, id_b = 0;
    char *astr = NULL, *bstr = NULL;
    Card *acard = NULL, *bcard = NULL;
    char ch = '\0';
    // Count how many buttons are selected
    count = 0;
    for(i = 0; i < 6; i++) {
        if(g_cribbage->btns[i]->selected) {
            count++;
            (id_a ? (id_b = i) : (id_a = i));
        }
    }

    // If 2, prompt the user for confirmation that these two are what they want
    // to add to the crib
    if(2 == count) {
        acard = get_card_at(g_cribbage->decks[CR_PLAYER], id_a);
        bcard = get_card_at(g_cribbage->decks[CR_PLAYER], id_b);
        astr = get_card_str(acard);
        bstr = get_card_str(bcard);
        // If user confirms, add the two cards to the crib and change state to
        // GFL_CRIBPLAY
        // If user declines, deselect all buttons
        ch = cribbage_prompt("Add the %s and %s to the crib? [y/n]", astr, bstr);
        if('Y' == ch)
        {
            // Move cards to crib, change state
            remove_card_from_deck(g_cribbage->decks[CR_PLAYER],acard);
            add_card_to_deck(g_cribbage->decks[CR_CRIB],acard);
            remove_card_from_deck(g_cribbage->decks[CR_PLAYER],bcard);
            add_card_to_deck(g_cribbage->decks[CR_CRIB],bcard);

            // TODO CPU needs to add two cards to the crib here
            cribbage_cpu_to_crib();

            g_cribbage->flags &= ~GFL_CRIBDISC;
            g_cribbage->flags |= GFL_CRIBPLAY | GFL_DRAW;
            cribbage_msg("Choose a card to play");
            for(i = 0; i < 6; i++) {
                g_cribbage->btns[i]->selected = false;
                // Deactivate last two buttons, not needed now
                if(i > 3) {
                    g_cribbage->btns[i]->active = false;
                }
            }
        } else {
            for(i = 0; i < 6; i++) {
                g_cribbage->btns[i]->selected = false;
            }
        }
    }
    if(astr) free(astr);
    if(bstr) free(bstr);
}

void cribbage_cpu_play(void) {
    // Have the cpu play a card from it's hand on to the table.
    // The cpu player is currently an idiot, so it just chooses a random, valid,
    // card and places it on the table. Eventually, it will choose a card
    // "intelligently" 
    Deck *board = g_cribbage->decks[CR_CPU_BOARD];
    Deck *cpuhand = g_cribbage->decks[CR_CPU];
    int numcards = count_cards(cpuhand->cards);
    int i = mt_rand(0,numcards);
    add_card_to_deck(board, 
            remove_card_from_deck(cpuhand,
                get_card_at(cpuhand,i)));
}

void cribbage_update_count(void) {
    // Count the value of cards on the table, adding points to the person who
    // played the last card if applicable, and resetting the count to 0 if both
    // players are "go"
    Deck *playerboard = g_cribbage->decks[CR_PLAYER_BOARD];
    Deck *cpuboard = g_cribbage->decks[CR_CPU_BOARD];
    int newcount = 0;
    Card *tmp = playerboard->cards;
    while(tmp) {
        newcount += cribbage_card_value(tmp->flags);
        tmp = tmp->next;
    }
    tmp = cpuboard->cards;
    while(tmp) {
        newcount += cribbage_card_value(tmp->flags);
        tmp = tmp->next;
    }

    g_cribbage->count = newcount;
}

void cribbage_update_play(void) {
    // Alternate turns playing cards, scoring points and increasing the count
    int i = 0, id = 0, count = 0;
    bool selected = false;
    Card *pcard = NULL;
    // Check to see if it's the player's turn,
    if(g_cribbage->pturn) {
        //Check if a button is selected
        count = 0;
        for(i = 0; i < 6; i++) {
            if(g_cribbage->btns[i]->active) {
                count += 1;
            }
            if(g_cribbage->btns[i]->selected) {
                selected = true;
                id = i;
            }
        }
        if(selected) {
            // get the card that was selected, place it on the board.
            pcard = get_card_at(g_cribbage->decks[CR_PLAYER], id);
            // End player turn
            if(pcard) {
                remove_card_from_deck(g_cribbage->decks[CR_PLAYER],pcard);
                add_card_to_deck(g_cribbage->decks[CR_PLAYER_BOARD],pcard);
                cribbage_update_count();
                g_cribbage->btns[count-1]->active = false;
                g_cribbage->pturn = false;
            }
            for(i = 0; i < 6; i++) {
                g_cribbage->btns[i]->selected = false;
            }
        }
    } else {
        // Have the computer play a card
        cribbage_cpu_play();
        cribbage_update_count();
        g_cribbage->pturn = true;
    }
}

void cribbage_update_show(void) {
    // Score hands

}

void cribbage_update(void) {
    // Calls the appropriate update routine for whichever state the game is in
    if(check_flag(g_cribbage->flags, GFL_CRIBDISC)) {
        cribbage_update_discard();
    } else if (check_flag(g_cribbage->flags, GFL_CRIBPLAY)) {
        cribbage_update_play();
    } else if (check_flag(g_cribbage->flags, GFL_CRIBSHOW)) {
        cribbage_update_show();
    }
}

/*****
 * Drawing functions (will be moved to cribbage_draw.c, eventually)
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
    uint8_t board_fg = WHITE; // Might be a settings option in the future?
    uint8_t board_bg = BRIGHT_BLACK;

    // Clear screen
    scr_clear();

    // Draw crib
    y = (g_cribbage->pcrib ? 14 : 1);
    deck = g_cribbage->decks[CR_CRIB];
    cards = deck->cards;
    i = 4;
    while(cards){
        pt_card_back(i+xo,y);
        i -= 1;
        cards=cards->next;
    }

    // Draw cut
    pt_card_back(14+xo,6+yo);
    pt_card(14+xo,7+yo,g_cribbage->decks[CR_STOCK]->cards);

    // Draw player hand
    deck = g_cribbage->decks[CR_PLAYER];
    cards = deck->cards;
    x = 54;
    y = 14;
    i = 0;
    while(cards) {
        pt_card(x,y,cards);
        pt_button_at(g_cribbage->btns[i],x,y+4);
        x += 4;
        i += 1;
        cards = cards->next;
    }

    // Draw player table cards
    deck = g_cribbage->decks[CR_PLAYER_BOARD];
    cards = deck->cards;
    x = 20;
    y = 13;
    while(cards) {
        pt_card(x,y,cards);
        x += 4;
        cards = cards->next;
    }

    // Draw cpu table cards
    deck = g_cribbage->decks[CR_CPU_BOARD];
    cards = deck->cards;
    x = 20;
    y = 1;
    while(cards) {
        pt_card(x,y,cards);
        x += 4;
        cards = cards->next;
    }

    // Draw count
    //i = cribbage_get_count();
    i = g_cribbage->count;
    scr_pt_clr(37+xo,12+yo,WHITE,BLACK,"Count: %d",i);

    // Draw Board
    scr_draw_box_clr(20+xo,5+yo,40,6,board_fg,board_bg);
    scr_pt_clr(21+xo,6+yo,board_fg,board_bg,
            " . ..... ..... ..... ..... ..... ..... ");
    scr_pt_clr(21+xo,7+yo,board_fg,board_bg,
            " . ..... ..... ..... ..... ..... ..... ");
 /*
 * ♠ u2660, ♤ u2664
 * ♥ u2665, ♡ u2661
 * ♦ u2666, ♢ u2662
 * ♣ u2663, ♧ u2667
 */
    scr_pt_clr(21+xo,8+yo,board_fg,board_bg,
            "     \u2664        \u2661        \u2662        \u2667      ");
    scr_pt_clr(21+xo,9+yo,board_fg,board_bg,
            " . ..... ..... ..... ..... ..... ..... ");
    scr_pt_clr(21+xo,10+yo,board_fg,board_bg,
            " . ..... ..... ..... ..... ..... ..... ");

    //Draw CPU pegs
    // TODO: This should go off of pegC1, pegC2 not cScore
    cribbage_draw_peg(22,58,6,g_cribbage->cScore,BRIGHT_RED);
    //cribbage_draw_peg(22,58,6,g_cribbage->pegC1,BRIGHT_RED);
    //cribbage_draw_peg(22,58,6,g_cribbage->pegC2,RED);

    //Draw player pegs 
    // TODO: This should go off of pegP1, pegP2 not cScore
    cribbage_draw_peg(22,58,9,g_cribbage->pScore,BRIGHT_GREEN);
    //cribbage_draw_peg(22,58,6,g_cribbage->pegP1,BRIGHT_GREEN);
    //cribbage_draw_peg(22,58,6,g_cribbage->pegP2,GREEN);

    //Draw scores
    scr_pt_clr(62+xo,6+yo,WHITE,BLACK,"%d",g_cribbage->cScore);
    scr_pt_clr(62+xo,10+yo,WHITE,BLACK,"%d",g_cribbage->pScore);

    //Draw message/prompt
    if(g_cribbage->msg) {
        scr_pt_clr(xo,22+yo,WHITE,BLACK,"%s",g_cribbage->msg);
    }
    
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
