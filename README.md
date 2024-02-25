# Cards!

![Title](screenshots/Title.png)

A deck of cards simulator. I designed this with the goal of making the framework
to add any game played with a deck of cards easy to implement. This should run
in most any modern terminal simulator that supports 256 colors (xterm, GNOME
terminal, etc). Nothing but standard C libraries are used (no NCurses
required!), and the program can be built with "make optimized". 

The games included (and mostly completed) so far are:
- Klondike
![Klondike](screenshots/Klondike.png)
- Penguin
![Penguin](screenshots/Penguin.png)

I'm planning on adding a couple "multiplayer" games (particularly Cribbage),
where the computer plays the second player... Eventually.

Additionally, the card colors are customizable in a settigns screen, where
(using the VI keys, 'hjkl') the user can choose from any of the supported 256
colors.
![Color Picker](screenshots/ColorPicker.png)
