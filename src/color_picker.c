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

uint8_t color_picker_menu(const uint8_t initcolor, char *prompt) {
    /*
     * Ideally, this function will print a nice display of all 256 colors, and
     * the user can select a color by moving the cursor with the arrow keys and
     * hitting 'q' to return the color selected. A color will show up as
     * 'selected' by changing that cell's background color to the selected
     * color.
     *
     */
    // x and y offsets so that things are drawn in the right spot
    // Dont need these because I'm using only the buffer?
    //int xo = (g_screenW / 2) - (SCREEN_WIDTH / 2);
    //int yo = (g_screenH / 2) - (SCREEN_HEIGHT / 2);
    int x = 0, y = 0;
    uint8_t selcolor = initcolor;
    int i = 0, j = 0, k = 0;
    char ch = '\0';

    // String buffer
    char strbuf[80];

    while(ch != 'q') {
        clear_screen(g_screenbuf); // Clear the screen buffer
        //draw_hline(0,0,SCREEN_WIDTH,WHITE);
        if(prompt) {
            draw_colorstr((SCREEN_WIDTH/2)-(strlen(prompt)/2),y,
                    prompt,WHITE,BLACK);
            y++;
        }
        snprintf(strbuf, 79, "Current color: %d. Selected color: %d.",
                initcolor,selcolor);
        draw_str(x,y,strbuf);
        y+=2;
        x = 0;
        for(i = 1; i < 256; i ++) {
            j = get_screen_index(x,y);
            k = get_screen_index(x,y+1);
            g_screenbuf[k].ch = ' ';
            g_screenbuf[k].fg = i;
            g_screenbuf[k].bg = i;
            k = get_screen_index(x+1,y);
            g_screenbuf[k].ch = ' ';
            g_screenbuf[k].fg = i;
            g_screenbuf[k].bg = i;
            k = get_screen_index(x+1,y+1);
            g_screenbuf[k].ch = ' ';
            g_screenbuf[k].fg = i;
            g_screenbuf[k].bg = i;
            if(i == selcolor) {
                g_screenbuf[j].ch = 'X';
                g_screenbuf[j].fg = i;
                g_screenbuf[j].bg = BLACK;
            } else if (i == initcolor) {
                g_screenbuf[j].ch = 'o';
                g_screenbuf[j].bg = BLACK;
                g_screenbuf[j].fg = i;
            } else {
                g_screenbuf[j].ch = ' ';
                g_screenbuf[j].bg = i;
                g_screenbuf[j].fg = i;
            }
            
            x+=2;
            if((i % 32 == 0) && (i != 0)) { 
                y+=2;
                x=0;
            }
        }
        y+=2;
        x=0;
        snprintf(strbuf,79, "Press [q] to select and return.");
        draw_str(x,y,strbuf);
        draw_screen(g_screenbuf); // Draw the screen buffer
        ch = kb_get_bl_char();
        switch(ch) {
            case 'h': 
                //Left
                selcolor -= 1; 
                break;
            case 'j':
                //Down
                selcolor += 32;
                break;
            case 'k':
                //up
                selcolor -= 32;
                break;
            case 'l':
                //right
                selcolor += 1;
                break;
            case 'q':
            default: break;
        }
        x = 0;
        y = 0;
    }

    return selcolor; //Black
}
