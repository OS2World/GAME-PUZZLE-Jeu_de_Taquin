/*--------------------
   LANG.H -- language support for Jeu de Taquin
  --------------------*/

#ifndef LANG_H
#define LANG_H

#define LANG_EN    0
#define LANG_ES    1
#define LANG_NL    2
#define LANG_DE    3
#define LANG_FR    4
#define LANG_IT    5
#define LANG_COUNT 6

enum {
    STR_MENU_GAME=0,
    STR_MENU_NEW_NORMAL,
    STR_MENU_NEW_INVERT,
    STR_MENU_SCRAMBLE,
    STR_MENU_EXIT,
    STR_MENU_OPTIONS,
    STR_MENU_LANGUAGE,
    STR_MENU_FRAME,
    STR_MENU_SAVEONEXIT,
    STR_MENU_HELP,
    STR_MENU_ABOUT,
    STR_TIME_IDLE,
    STR_TIME_RUNNING,
    STR_TIME_SOLVED,
    STR_COUNT
};

extern int current_lang;
extern const char *lang_strings[LANG_COUNT][STR_COUNT];
#define tr(id) ((char*)lang_strings[current_lang][(id)])

#endif
