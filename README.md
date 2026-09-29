# Jeu de Taquin for OS/2

Version 1.1 — 32-bit PM port for ArcaOS / OS/2 Warp 4.52

The classic 15-tile sliding puzzle, originally programmed by Charles Petzold
and published in PC Magazine Vol. 8 No. 12 & 13, January 1989.

![JeuDeTaquin](/doc/JeuDeTaquin.png)

## License

Public Domain / Educational Use  
Original source: (c) 1989, Ziff Communications Co.  
OS/2 32-bit port: (c) 2026, OS2World

## Build

Requires OpenWatcom 2.0 and OS/2 Toolkit 4.5 on ArcaOS.

```
compile-wat.cmd
```

Output: `bin\taquin.exe`

## Features

- 4×4 sliding tile puzzle (15 numbered tiles + 1 blank)
- Click or use arrow keys to slide tiles
- Game > New Game (Ctrl+N), Inverted Reset, Scramble
- 6 interface languages: English, Spanish, Dutch, German, French, Italian
- Frame Controls (Ctrl+F) for borderless mode
- Bottom status bar with elapsed timer; stops when solved
- Settings persistence (taquin.cfg); save-on-exit enabled by default

## Changes from Original

- Ported from 16-bit OS/2 1.x PM to 32-bit PM (OpenWatcom 2.0)
- Restructured menus: Game / Options / Help
- Added keyboard shortcuts: Ctrl+N, Ctrl+X, Ctrl+F
- Added 6-language support with runtime switching
- Added settings persistence
- Added Frame Controls (borderless mode)
- About dialog updated to OS2World standard format

## Authors

- Charles Petzold — original source, PC Magazine 1989
- OS2World community — 32-bit port, 2026
