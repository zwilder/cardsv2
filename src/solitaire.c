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
#include <sys/time.h>

Solitaire* create_solitaire(uint8_t num_decks) {
    uint8_t i = 0;
    Solitaire *game = malloc(sizeof(Solitaire));
    game->events = NULL;
    game->update = NULL;
    game->draw = NULL;
    game->flags = GFL_NONE;
    game->score = 0;
    game->num_decks = num_decks;
    game->decks = malloc(sizeof(Deck*) * num_decks);
    game->btns = malloc(sizeof(Button*) * num_decks);
    for(i = 0; i < num_decks; i++) {
        game->decks[i] = create_deck();
        game->decks[i]->id = i;
        game->btns[i] = create_button(0,0,i,'a'+i);
    }
    game->fromref = NULL;
    game->toref = NULL;
    game->msg = NULL;
    return game;
}

void destroy_solitaire(Solitaire *game) {
    if(!game) return;
    uint8_t i = 0;
    for(i = 0; i < game->num_decks; i++) {
        if(game->decks) {
            destroy_deck(game->decks[i]);
        }
        if(game->btns) {
            destroy_button(game->btns[i]);
        }
    }
    if(game->decks) {
        free(game->decks);
    }
    if(game->btns) {
        free(game->btns);
    }
    if(game->msg) {
        free(game->msg);
    }
    game->decks = NULL;
    game->btns = NULL;
    game->fromref = NULL;
    game->toref = NULL;
    free(game);
}

void solitaire_msg(Solitaire *g, char *msg,...) {
    if(g->msg) {
        free(g->msg);
        g->msg = NULL;
    }
    if(!msg) return;
    va_list args;
    va_start(args,msg);
    int i = strlen(msg) + 1;
    g->msg = malloc(sizeof(char) * i);
    vsnprintf(g->msg,i,msg,args);
    va_end(args);
}

long current_ms(void) {
    // Helper function to return the current time in ms
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return ((tp.tv_sec * 1000) + (tp.tv_usec / 1000));
}

void solitaire_loop(Solitaire *g) {
    /* Overdesigned main loop? Probably. This is your standard,
     * events-update-render loop with a nice check to keep it at a steady
     * 30fps (otherwise it will render as fast as it can draw, and since this
     * isn't a very processor intensive program at all - it can look blinky) 
     * Update: this doesn't stop it looking blinky. Fairly certain the way I'm
     * using escape codes to clear/refresh the screen is causing the blinky.
     * Current fix is only drawing the screen when the game does something that
     * would make the display change.*/
    long prev = current_ms();
    long lag = 0, current = 0, elapsed = 0;
    long msperframe = 33; // 16ms = ~60fps, 33ms = ~30fps
    g->flags |= GFL_RUNNING;
    while(check_flag(g->flags, GFL_RUNNING)) {
        current = current_ms();
        elapsed = current - prev;
        prev = current;
        lag += elapsed;
        g->events();
        g->update();
        if(check_flag(g->flags,GFL_DRAW)) {
            g->draw();
        }
        if(check_flag(g->flags,GFL_RESTART)) {
            g->flags &= ~GFL_RUNNING;
        }
        while(lag >= msperframe) {
            lag -= msperframe;
        }
    }
}
