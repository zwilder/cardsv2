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
    g_cribbage->msglist = NULL;

    //g_cribbage->msglist = create_slist("First message. %d",1001);
    //slist_push(&(g_cribbage->msglist), "Second message. %d",2);
    //slist_push(&(g_cribbage->msglist), "Third message. %d",3);
    //slist_push(&(g_cribbage->msglist), "Fourth message. %d",4);

    g_cribbage->msgpos = 0;
    g_cribbage->flags = GFL_NONE;

    // Put 52 cards in the stock, and shuffle it
    fill_deck(g_cribbage->decks[CR_STOCK]);
    shuffle_deck(g_cribbage->decks[CR_STOCK]);

    // Deal the cards, draw the cards, enter the loop
    // cut deck to see who goes first here maybe?
    g_cribbage->pcrib = false; 
    g_cribbage->pturn = true;
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

    if(g_cribbage->msglist) {
        destroy_slist(&g_cribbage->msglist);
    }

    if(g_cribbage) {
        free(g_cribbage);
        g_cribbage = NULL;
    }
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
        if(check_flag(g_cribbage->flags,GFL_PAUSE)) {
            // In case I need to pause the loop? Might be a good idea or a
            // terrible idea. A better implementation would just loop, skipping
            // the regular update/events and then end when the user enters a
            // keypress. Update: No shocker here, it doesn't work well
            //cribbage_prompt("Press any key to continue...");
            g_cribbage->flags &= ~GFL_PAUSE;
        }
    }
}
void cribbage_clear_msg(void) {
    //Push 4 blank messages - might be ok to just destroy the message list?
    //destroy_slist(&g_cribbage->msglist); // This is blinky?
    cribbage_msg(" ");
    cribbage_msg(" ");
    cribbage_msg(" ");
}

void cribbage_msg(char *fstr, ...) {
    /*
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
    */
    // Cribbage should maintain an SList of messages, and then print all of them
    // in order on the screen
    
    if(!fstr) return;
    SList *tmp = NULL;
    char *msg = NULL;
    va_list args;
    va_start(args, fstr);
    int i = vsnprintf(NULL, 0, fstr, args);
    va_end(args);
    i += 1;
    msg = malloc(sizeof(char) * i);
    if(!msg) return;
    va_start(args,fstr);
    vsnprintf(msg, i, fstr, args);
    va_end(args);

    g_cribbage->msgpos += 1;
    if(g_cribbage->msgpos > 4) {
        tmp = slist_pop_node(&g_cribbage->msglist);
        destroy_slist(&tmp);
        slist_push(&g_cribbage->msglist,"%s",msg);
    } else {
        slist_push(&g_cribbage->msglist,"%s",msg);
    }
    free(msg);
}

char cribbage_prompt(char *fstr, ...) {
    // Draw a prompt at the bottom of the screen, and wait for the user to
    // return a keypress
    char result = '\0';
    int xo = (g_screenW / 2) - (SCREEN_WIDTH / 2); // Standard screen size is 80x24, defined in glyph.c
    int yo = (g_screenH / 2) - (SCREEN_HEIGHT / 2);
    if(!fstr) return result;
    g_cribbage->flags |= GFL_DRAW;
    cribbage_draw();

    // Draw a blank line to "erase" the bottom of the screen
    char *prompt = malloc(SCREEN_WIDTH * sizeof(char));
    memset(prompt, ' ', SCREEN_WIDTH - 2);
    prompt[SCREEN_WIDTH - 2] = '\0';
    scr_pt_clr(0+xo,23+yo,WHITE,BLACK,prompt); 

    // Format the prompt
    va_list args;
    va_start(args,fstr);
    vsnprintf(prompt,SCREEN_WIDTH,fstr,args);
    va_end(args);
    // Print the prompt
    scr_pt_clr(xo,23+yo,WHITE,BLACK,prompt);

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
