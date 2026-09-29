Jeu de Taquin for OS/2  Version 1.0
=====================================

Overview
--------
Jeu de Taquin is the classic 15-tile sliding puzzle (also known as the
15-puzzle or the Taquin puzzle). It was originally programmed by Charles
Petzold and published in PC Magazine, Vol. 8 No. 12 & 13, January 1989.

This version is a 32-bit OS/2 Presentation Manager port for ArcaOS and
OS/2 Warp 4.52, built with OpenWatcom 2.0.


How to Play
-----------
The puzzle consists of 15 numbered tiles arranged in a 4x4 grid, with
one blank space. The object is to slide the tiles into numerical order:

  1  2  3  4
  5  6  7  8
  9 10 11 12
 13 14 15  _

Click any tile in the same row or column as the blank space to slide
that entire row or column.


Controls
--------
Arrow keys    Slide tiles (same as clicking on the adjacent tile)
Ctrl+N        New Game (normal order)
Ctrl+X        Exit the application
Ctrl+F        Toggle Frame Controls (borderless mode)


Menu
----
Game > New Game           Start a new puzzle in normal order
Game > Inverted Reset     Start with the last two tiles swapped
Game > Scramble           Randomly scramble the tiles
Game > Exit               Close the application

Options > Language        Switch the interface language
Options > Frame Controls  Hide or show the title bar and menu
Options > Save settings   Save language preference on exit

Help > About              Show version and author information


File List
---------
  doc\Readme.txt      This file
  doc\Changelog.txt   Version history
  doc\LICENSE.txt     License information
  bin\taquin.exe      The game executable


Disclaimer
----------
This port is provided as-is, for educational purposes. It is based on
example source code published in PC Magazine in 1989.


Author
------
Original source: Charles Petzold, PC Magazine, January 1989
OS/2 32-bit port: OS2World community, 2026
