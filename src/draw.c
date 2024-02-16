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
 * A lot (most) of this file was lifted from the Goblin Caves project - and
 * since the Glyph/screen buffer style drawing setup from that project can't
 * support fancy unicode characters, this project has a weird thing where it
 * uses both the term_engine.h/c and this to draw things to the screen. The idea
 * here is that a lot of what I wrote for Goblin Caves could be used here -
 * dialog boxes, menus, etc. 
 *
 * Actually, after stripping most of the Goblin Caves stuff from this file it's
 * **almost** generic enough that I could throw this in the Toolbox - like an
 * addendum to Glyph.
 *
 *****/

Glyph *g_screenbuf = NULL;

void init_screenbuf(void) {
    g_screenbuf = create_screen();
    clear_screen(g_screenbuf);
}

void close_screenbuf(void) {
    // This function seems pointless but for consistency it exists.
    if(!g_screenbuf) return;
    destroy_screen(g_screenbuf);
}

void draw_glyph(int x, int y, Glyph g) {
    /* Draw a glyph in the appropriate offset spot for the size of the user's
     * terminal screen */
    int dx = (g_screenW / 2) - (SCREEN_WIDTH / 2);
    int dy = (g_screenH / 2) - (SCREEN_HEIGHT / 2);
    if(g.fg >= BRIGHT_BLACK) {
        scr_set_style(ST_BOLD);
    } else {
        scr_set_style(ST_NONE);
    }
    scr_pt_clr_char(x+dx,y+dy,g.fg,g.bg,g.ch);
}

void draw_screen(Glyph *screen) {
    /* Take a standard array of Glyphs, length SCREEN_WIDTH x SCREEN_HEIGHT, and
     * render it on the screen. */
    int x, y, i;
    for(x = 0; x < SCREEN_WIDTH; x++) {
        for(y = 0; y < SCREEN_HEIGHT; y++) {
            i = get_screen_index(x,y);
            if(i > (SCREEN_WIDTH * SCREEN_HEIGHT - 1)) {
                break;
            }
            draw_glyph(x,y,screen[i]);
        }
    }
}

void draw_str(int x, int y, char *str) {
    /* Draws a string on the global buffer, white fg/black bg by default.*/
    if(!str) return;
    int i,j,k;
    k=0;
    for(i = x; i < x + strlen(str); i++) {
        j = get_screen_index(i,y);
        g_screenbuf[j].ch = str[k];
        g_screenbuf[j].fg = WHITE;
        g_screenbuf[j].bg = BLACK;
        k++;
    }
}

void draw_str_vec(Vec2i a, char *str) {
    draw_str(a.x,a.y,str); 
}

void draw_colorstr(int x, int y, char *str, uint8_t fg, uint8_t bg) {
    /* Draws a string in color on the global buffer. */
    if(!str) return;
    int i,j,k;
    k = 0;
    for(i = x; i < x + strlen(str); i++) {
        j = get_screen_index(i,y);
        g_screenbuf[j].ch = str[k];
        g_screenbuf[j].fg = fg;
        g_screenbuf[j].bg = bg;
        k++;
    }
}

void draw_colorstr_vec(Vec2i a, char *str, uint8_t fg, uint8_t bg) {
    draw_colorstr(a.x,a.y,str,fg,bg);
}

void draw_hline(int x, int y, int w, uint8_t color) {
    /* Starting at x,y and advancing x w times, draw */
    if((x < 0) || (y < 0) || (w <=0 )) return;
    int i,j;
    for(i = x; i < (x+w); i++) {
        j = get_screen_index(i,y);
        g_screenbuf[j].ch = '.';
        g_screenbuf[j].fg = color;
        g_screenbuf[j].bg = color;
    }
}

void draw_hline_vec(Vec2i a, int w, uint8_t color) {
    draw_hline(a.x,a.y,w,color);
}

void draw_vline(int x, int y, int h, uint8_t color) {
    /* Starting at x,y and advancing y h times, draw */
    if((x < 0) || (y < 0) || (h <=0 )) return;
    int i,j;
    for(i = y; i < (y+h); i++) {
        j = get_screen_index(x,i);
        g_screenbuf[j].ch = '.';
        g_screenbuf[j].fg = color;
        g_screenbuf[j].bg = color;
    }
}

void draw_vline_vec(Vec2i a, int h, uint8_t color) {
    draw_vline(a.x,a.y,h,color);
}

void draw_solid_box(int x, int y, int w, int h, uint8_t color) {
    int i;
    for(i = y; i < (y + h); i++) {
        draw_hline(x,i,w,color);
    }
}

void draw_solid_box_vec(Vec2i a, Vec2i d, uint8_t color) {
    draw_solid_box(a.x,a.y,d.x,d.y,color);
}

void draw_solid_box_rect(Rect a, uint8_t color) {
    draw_solid_box_vec(a.pos,a.dim, color);
}

void draw_box(int x, int y, int w, int h, uint8_t fg, uint8_t bg) {
    draw_solid_box(x,y,w,h,fg);
    draw_solid_box(x+1,y+1,w-2,h-2,bg); 
}

void draw_box_vec(Vec2i a, Vec2i d, uint8_t fg, uint8_t bg) {
    draw_box(a.x,a.y,d.x,d.y,fg,bg);
}

void draw_box_rect(Rect a, uint8_t fg, uint8_t bg) {
    draw_box_vec(a.pos, a.dim, fg,bg);
}

void draw_msg_box(char *msg, uint8_t fg, uint8_t bg) {
    /* Draw a message in a box at the center of the screen. */
    int maxw = (SCREEN_WIDTH * 2) / 3;
    int x,y,w,h,i;
    SList *lines = NULL, *slistit = NULL;
    if(strlen(msg) > maxw) {
        /* We need line wrapping */
        lines = slist_linewrap(msg, maxw);
        h = slist_count(lines) + 2;
        x = (SCREEN_WIDTH / 2) - (maxw / 2) - 1;
        y = (SCREEN_HEIGHT / 2) - (h / 2);
        draw_solid_box(x,y,maxw,h,bg);
        slistit = lines;
        i = 1;
        while(slistit) {
            draw_colorstr(x+1,y+i,slistit->data,fg,bg);
            i++;
            slistit = slistit->next;
        }
        destroy_slist(&lines);
    } else {
        /* No line wrapping needed */
        w = strlen(msg) + 2;
        x = (SCREEN_WIDTH / 2) - (strlen(msg) / 2) - 1;
        h = 3; 
        y = (SCREEN_HEIGHT / 2) - 1;
        draw_solid_box(x,y,w,h,bg);
        draw_colorstr(x+1,y+1,msg,fg,bg);
    }
}

char draw_cmenu(SList *menu, uint8_t fg, uint8_t bg, uint8_t boxcolor) {
    /* Draws a message box containing a prompt, with a list of options.
     * The slist passed to draw_menu (menu) should contain the following in
     * order:
     * - A prompt
     * - A message at the bottom of the window. Optional if the string in the
     *   slist node is "\0"
     * - A list of characters to represent the option choices, for example,
     *   "abcd" will show each option as "[a] opt 1 [b] opt 2" etc. Optional if
     *   the string in the slist node is "\0", and if it isn't passed in than
     *   the options will be numbered sequentially.
     * - A list of options to be displayed to the user. 
     * The fg/bg colors are used for the text and the box around the menu. */

    int x,y,w,h,cx,cy,i;
    SList *slit = NULL, *slprompt = NULL, *slinstr = NULL;
    // First three item in menu are the prompt, instr, and optitems
    char *prompt = slist_get_node(menu, 0)->data;
    char *instr = slist_get_node(menu, 1)->data;
    char *optitems = slist_get_node(menu,2)->data;
    SList *options = slist_get_node(menu,3);
    char numstr[5] = "[1] ";
    char result = '\0';
    if(instr[0] == '\0') instr = NULL;
    if(optitems[0] == '\0') optitems = NULL;
    numstr[4] = '\0';
    w = (SCREEN_WIDTH * 2) / 3; // width is 2/3 screen width (66%)

    // Linewrap the prompt string
    slprompt = slist_linewrap(prompt,w);
    
    // Height is number of options + 2 for the border top/btm + number of lines
    // in prompt + 2 for space after prompt and before instr
    h = slist_count(slprompt) + 3 + slist_count(options); 
    if(instr) h += 1;

    // Linewrap the instr string
    if(instr) {
        slinstr = slist_linewrap(instr, w);
        h += slist_count(slinstr);
    }

    // x/y are centered on the screen
    x = (SCREEN_WIDTH / 2) - (w / 2) - 1;
    y = (SCREEN_HEIGHT / 2) - (h / 2);
    cx = x + 1;
    cy = y + 1;

    // Draw a box around the menu area
    draw_box(x-1,y-1,w+2,h+2,boxcolor,bg);

    // Draw the prompt
    slit = slprompt;
    while(slit) {
        // Bold the foreground color if possible
        i = fg + 8;
        if(i > 15) i = fg;
        draw_colorstr(cx,cy,slit->data,i,bg);
        cy++;
        slit = slit->next;
    }
    destroy_slist(&slprompt);
    cy++;

    // Draw the options
    slit = options;
    if(optitems) {
        i = 0;
    } else {
        i='1';
    }
    while(slit) {
        draw_colorstr(cx+4,cy,slit->data,fg,bg);
        // "[1] " = 4 chars, 5 with null
        if(optitems) {
            numstr[1] = optitems[i];    
        } else {
            numstr[1] = i;
        }
        draw_colorstr(cx,cy,numstr,fg,bg);
        i++;
        cy++;
        if(i>'9') break; // Limited to nine options currently
        if(optitems) {
            if(i > strlen(optitems)) i = 0; // Shouldn't happen, but if it do
        }
        slit = slit->next;
    }

    // If instructions, draw them
    if(slinstr) {
        cy++;
        slit = slinstr;
        while(slit) {
            draw_colorstr(cx,cy,slit->data,fg,bg);
            cy++;
            slit = slit->next;
        }
        destroy_slist(&slinstr);
    }

    // Draw on the screen
    draw_screen(g_screenbuf);

    // Wait for input
    while(true) {
        result = get_input();
        if(result != '\0') break;
    }

    // Returns screen to whatever it was before drawing the menu
    // (makes the screen flicker during new player creation, so disabling)
    //engine_draw();

    // Return input
    return result;
}

char draw_menu_basic(SList *menu) {
    return draw_cmenu(menu, WHITE, BLACK, WHITE);
}

char draw_menu(SList *menu, uint8_t fg, uint8_t bg) {
    return draw_cmenu(menu, fg, bg, fg);
}

char draw_menu_nobox(SList *menu, uint8_t fg, uint8_t bg) {
    return draw_cmenu(menu, fg ,bg, bg);
}
