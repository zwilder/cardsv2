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

void help_menu(void) {
    int xo = (g_screenW / 2) - (SCREEN_WIDTH / 2);
    int yo = (g_screenH / 2) - (SCREEN_HEIGHT / 2);
    char ch = '\0';
    bool loop = true;

    while(loop) {
        // Clear the screen
        scr_clear(); 
        fill_screen_blank(g_screenbuf);

        // Draw the title
        pt_card_title((SCREEN_WIDTH / 2)-12+xo, yo+1, "Help");

        // Print some help
        scr_pt_clr(xo + 2, yo + 6, WHITE, BLACK, " The cards are interacted with by pressing a key on the keyboard");
        scr_pt_clr(xo + 2, yo + 7, WHITE, BLACK, "corresponding to each card \"pile\" (tableau, deck, foundation, cell, etc).");
        scr_pt_clr(xo + 2, yo + 9, WHITE, BLACK, " The first button pressed selects the card to move, and the second button");
        scr_pt_clr(xo + 2, yo + 10, WHITE, BLACK, "pressed indicates where the user would like to move the card.");
        scr_pt_clr(xo + 2, yo + 12, WHITE, BLACK, " \'Escape\' pauses the game, opening up a menu where the user can start");
        scr_pt_clr(xo + 2, yo + 13, WHITE, BLACK, "a new game, return to the main menu, or change card color settings.");
        scr_pt_clr(xo + 4, yo + 15, WHITE, BLACK, "[a] - Klondike Rules");
        scr_pt_clr(xo + 4, yo + 16, WHITE, BLACK, "[b] - Penguin Rules");
        scr_pt_clr(xo + 4, yo + 17, WHITE, BLACK, "[c] - Cribbage Rules");
        scr_pt_clr(xo + 4, yo + 18, WHITE, BLACK, "[?] - About Cards");
        scr_pt_clr(xo + (SCREEN_WIDTH/2) - 15 , yo + 23, WHITE, BLACK, 
                "[Press any other key to return]");

        // Draw the screen
        draw_screen(g_screenbuf);

        // Wait for a keypress to return
        ch = kb_get_bl_char();
        switch(ch) {
            case 'a': help_klondike(); break;
            case 'b': help_penguin(); break;
            case '?': help_about(); break;
            default: loop = false; break;
        }
    }
}

void help_about(void) {
    int xo = (g_screenW / 2) - (SCREEN_WIDTH / 2);
    int yo = (g_screenH / 2) - (SCREEN_HEIGHT / 2);

    // Clear the screen
    scr_clear(); 
    fill_screen_blank(g_screenbuf);

    // Draw the title
    pt_card_title((SCREEN_WIDTH / 2)-10+xo, yo+1, "About");

    scr_pt_clr(xo + 1, yo +6, WHITE, BLACK, 
        "Cards!");
    scr_pt_clr(xo + 1, yo +7, WHITE, BLACK, 
        "\u00A9 Zach Wilder 2024, <https://github.com/zwilder/cardsv2>"); 

    scr_pt_clr(xo + 1, yo +9, WHITE, BLACK, 
        " Cards is free software: you can redistribute it and/or modify it under the");
    scr_pt_clr(xo + 1, yo +10, WHITE, BLACK, 
        "terms of the GNU General Public License as published by the Free Software");
    scr_pt_clr(xo + 1, yo +11, WHITE, BLACK, 
        "Foundation, either version 3 of the License, or (at your option) any later");
    scr_pt_clr(xo + 1, yo +12, WHITE, BLACK, 
        "version.");
 
    scr_pt_clr(xo + 1, yo +14, WHITE, BLACK, 
        " Cards is distributed in the hope that it will be useful, but WITHOUT ANY");
    scr_pt_clr(xo + 1, yo +15, WHITE, BLACK, 
        "WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS");
    scr_pt_clr(xo + 1, yo +16, WHITE, BLACK, 
        "FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more");
    scr_pt_clr(xo + 1, yo +17, WHITE, BLACK, 
        "details.");
 
    scr_pt_clr(xo + 1, yo +19, WHITE, BLACK, 
        " You should have received a copy of the GNU General Public License along");
    scr_pt_clr(xo + 1, yo +20, WHITE, BLACK, 
        "with Cards. If not, see <http://www.gnu.org/licenses/>.");

    scr_pt_clr(xo + 1, yo +22, WHITE, BLACK, 
        "Local files at $HOME/.zwsl/cards.bin");
    scr_pt_clr(xo + (SCREEN_WIDTH/2) - 12 , yo + 23, WHITE, BLACK, 
            "[Press any key to return]");
    // Draw the screen
    draw_screen(g_screenbuf);

    // Wait for a keypress to return
    kb_get_bl_char();
}

void help_klondike(void) {
    /*
     * Klondike Rules (Courtesy of Wikipedia) 
     */
    int xo = (g_screenW / 2) - (SCREEN_WIDTH / 2);
    int yo = (g_screenH / 2) - (SCREEN_HEIGHT / 2);

    // Clear the screen
    scr_clear(); 
    fill_screen_blank(g_screenbuf);

    // Draw the title
    pt_card_title((SCREEN_WIDTH / 2)-16+xo, yo+1, "Klondike");

    scr_pt_clr(xo + 1, yo + 6, WHITE, BLACK, 
          " After shuffling, a tableau of seven fanned piles of cards is laid from left");
    scr_pt_clr(xo + 1, yo + 7, WHITE, BLACK, 
          "to right. From left to right, each pile contains one more card than the last.");
    scr_pt_clr(xo + 1, yo + 8, WHITE, BLACK, 
          "The first and left-most pile contains a single upturned card, the second");
    scr_pt_clr(xo + 1, yo + 9, WHITE, BLACK, 
          "pile contains two cards, and so forth. The topmost card of each pile is turned");
    scr_pt_clr(xo + 1, yo + 10, WHITE, BLACK, 
          "face up. The remaining cards form the stock and are placed facedown at the");
    scr_pt_clr(xo + 1, yo + 11, WHITE, BLACK, 
          "upper left of the layout.");

    scr_pt_clr(xo + 1, yo + 13, WHITE, BLACK, 
          " The four foundations (light rectangles in the upper right of the game)");
    scr_pt_clr(xo + 1, yo + 14, WHITE, BLACK, 
          "are built up by suit from Ace (low in this game) to King, and the tableau");
    scr_pt_clr(xo + 1, yo + 15, WHITE, BLACK, 
          "piles can be built down by alternate colors. Every face-up card in a partial");
    scr_pt_clr(xo + 1, yo + 16, WHITE, BLACK, 
          "pile, or a complete pile, can be moved, as a unit, to another tableau pile on");
    scr_pt_clr(xo + 1, yo + 17, WHITE, BLACK, 
          "the basis of its highest card. Any empty piles can be filled with a King,");
    scr_pt_clr(xo + 1, yo + 18, WHITE, BLACK, 
          "or a pile of cards with a King. The aim of the game is to build up four stacks");
    scr_pt_clr(xo + 1, yo + 19, WHITE, BLACK, 
          "of cards starting with Ace and ending with King, all of the same suit, on one");
    scr_pt_clr(xo + 1, yo + 20, WHITE, BLACK, 
          "of the four foundations, at which time the player would have won.");
    scr_pt_clr(xo + (SCREEN_WIDTH/2) - 12 , yo + 23, WHITE, BLACK, 
            "[Press any key to return]");
  
    // Draw the screen
    draw_screen(g_screenbuf);

    // Wait for a keypress to return
    kb_get_bl_char();
}

void help_penguin(void) {
    /*
     * Penguin Rules Summary (Courtesy of Wikipedia)
     */
    int xo = (g_screenW / 2) - (SCREEN_WIDTH / 2);
    int yo = (g_screenH / 2) - (SCREEN_HEIGHT / 2);

    // Clear the screen
    scr_clear(); 
    fill_screen_blank(g_screenbuf);

    // Draw the title
    pt_card_title((SCREEN_WIDTH / 2)-14+xo, yo+1, "Penguin");
    scr_pt_clr(xo + 1, yo + 6, WHITE, BLACK, 
        " Cards are dealt from left to right into seven tableaus, each with seven cards.");
    scr_pt_clr(xo + 1, yo + 7, WHITE, BLACK, 
        "The first card dealt is called the \"beak\". When the other three cards with the");
    scr_pt_clr(xo + 1, yo + 8, WHITE, BLACK, 
        "same rank appear, they are immediately placed on the foundations, and the next");
    scr_pt_clr(xo + 1, yo + 9, WHITE, BLACK, 
        "dealt card takes it's place in the tableau.");
 
    scr_pt_clr(xo + 1, yo + 11, WHITE, BLACK, 
        " Object of the game is to build the foundations up in suit to the card that is");
    scr_pt_clr(xo + 1, yo + 12, WHITE, BLACK, 
        "a rank lower than the beak.");
 
    scr_pt_clr(xo + 1, yo + 14, WHITE, BLACK, 
        " Cards on each tableau are built down by suit. Cards are moved one at a time,");
    scr_pt_clr(xo + 1, yo + 15, WHITE, BLACK, 
        "unless a suit sequence of cards is formed, which can be moved as a unit. When");
    scr_pt_clr(xo + 1, yo + 16, WHITE, BLACK, 
        "an empty column occurs in the tableau, only a card of the rank directly under");
    scr_pt_clr(xo + 1, yo + 17, WHITE, BLACK, 
        "the beak can be placed on it.");
 
    scr_pt_clr(xo + 1, yo + 19, WHITE, BLACK, 
        " There are seven cells which can be used to store a single card to be played");
    scr_pt_clr(xo + 1, yo + 20, WHITE, BLACK, 
        "later. These cells are called the \"flipper\".");
 
    scr_pt_clr(xo + 1, yo + 22, WHITE, BLACK, 
        " The game is won when all cards are built onto the foundations.");
    scr_pt_clr(xo + (SCREEN_WIDTH/2) - 12 , yo + 23, WHITE, BLACK, 
            "[Press any key to return]");
    // Draw the screen
    draw_screen(g_screenbuf);

    // Wait for a keypress to return
    kb_get_bl_char();
}
