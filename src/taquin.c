/*---------------------------------------------------------
   TAQUIN.C -- Jeu de Taquin for OS/2 Presentation Manager
               (c) 1989, Ziff Communications Co.
               PC Magazine * Charles Petzold, January 1989
               32-bit PM port, OS2World 2026
  ---------------------------------------------------------*/

static const char bldlevel[] =
    "@#Charles Petzold:1.1#@##1## 28 Sep 2026 00:00:00      "
    "ARCAOS:::0::::@@Jeu de Taquin - 15-tile sliding puzzle for OS/2 PM\r\n\x1a";

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "taquin.h"
#include "lang.h"

#define NUMROWS        4
#define NUMCOLS        4
#define SCRAMBLEREP  100
#define SQUARESIZE    67
#define BARSIZE       15

/* --- language strings -------------------------------------------- */
int current_lang = LANG_EN;

const char *lang_strings[LANG_COUNT][STR_COUNT] = {
    /* English */
    { "~Game", "~New Game\tCtrl+N", "~Inverted Reset", "~Scramble",
      "E~xit\tCtrl+X", "~Options", "~Language",
      "~Frame Controls\tCtrl+F", "~Save settings on exit",
      "~Help", "~About...",
      "Time: --", "Time: %d:%02d", "Solved in %d:%02d" },
    /* Spanish */
    { "~Juego", "~Nueva Partida\tCtrl+N", "~Reinicio Invertido", "~Mezclar",
      "~Salir\tCtrl+X", "~Opciones", "~Idioma",
      "~Controles Marco\tCtrl+F", "~Guardar al salir",
      "~Ayuda", "~About...",
      "Tiempo: --", "Tiempo: %d:%02d", "Resuelto en %d:%02d" },
    /* Dutch */
    { "~Spel", "~Nieuw Spel\tCtrl+N", "~Omgekeerde Reset", "~Schudden",
      "A~fsluiten\tCtrl+X", "~Opties", "~Taal",
      "~Kaderopties\tCtrl+F", "~Instellingen opslaan",
      "~Help", "~About...",
      "Tijd: --", "Tijd: %d:%02d", "Opgelost in %d:%02d" },
    /* German */
    { "~Spiel", "~Neues Spiel\tCtrl+N", "~Invertierter Reset", "~Mischen",
      "~Beenden\tCtrl+X", "~Optionen", "~Sprache",
      "~Rahmen\tCtrl+F", "~Einstellungen speichern",
      "~Hilfe", "~About...",
      "Zeit: --", "Zeit: %d:%02d", "Geloest in %d:%02d" },
    /* French */
    { "~Jeu", "~Nouveau Jeu\tCtrl+N", "~Reinit. Inversee", "~Melanger",
      "~Quitter\tCtrl+X", "~Options", "~Langue",
      "~Cadre\tCtrl+F", "~Sauver a la sortie",
      "~Aide", "~About...",
      "Temps: --", "Temps: %d:%02d", "Resolu en %d:%02d" },
    /* Italian */
    { "~Gioco", "~Nuovo Gioco\tCtrl+N", "~Reset Invertito", "~Mescolare",
      "E~sci\tCtrl+X", "~Opzioni", "~Lingua",
      "~Cornice\tCtrl+F", "~Salva alla chiusura",
      "~Aiuto", "~About...",
      "Tempo: --", "Tempo: %d:%02d", "Risolto in %d:%02d" }
};

/* --- global window handles --------------------------------------- */
static HWND hwndFrame    = NULLHANDLE;
static HWND hwndTitleBar = NULLHANDLE;
static HWND hwndSysMenu  = NULLHANDLE;
static HWND hwndMinMax   = NULLHANDLE;
static HWND hwndMenuBar  = NULLHANDLE;
static HWND hwndObject   = NULLHANDLE;

/* --- global state ----------------------------------------------- */
static BOOL bSaveOnExit  = FALSE;
static BOOL bFrameHidden = FALSE;

/* --- settings --------------------------------------------------- */
#define CFG_FILE "taquin.cfg"

typedef struct {
    int saveonexit;
    int detaillevel;
    int current_lang;
} TAQUINCFG;

static void load_settings(void) {
    FILE *f;
    TAQUINCFG cfg = { 1, 0, LANG_EN };
    f = fopen(CFG_FILE, "rb");
    if (f) {
        fread(&cfg, sizeof(cfg), 1, f);
        fclose(f);
    }
    bSaveOnExit  = (cfg.saveonexit != 0);
    current_lang = cfg.current_lang;
    if (current_lang < 0 || current_lang >= LANG_COUNT) current_lang = LANG_EN;
}

static void save_settings(void) {
    FILE *f;
    TAQUINCFG cfg;
    cfg.saveonexit   = bSaveOnExit ? 1 : 0;
    cfg.detaillevel  = 0;
    cfg.current_lang = current_lang;
    f = fopen(CFG_FILE, "wb");
    if (f) {
        fwrite(&cfg, sizeof(cfg), 1, f);
        fclose(f);
    }
}

/* --- language runtime ------------------------------------------- */
static void set_language(int lang) {
    HWND hMenu, hSubGame, hSubOpt, hSubLang, hSubHelp;
    MENUITEM mi;
    int i;

    current_lang = lang;
    hMenu = WinWindowFromID(hwndFrame, FID_MENU);
    if (hMenu == NULLHANDLE) return;

    memset(&mi, 0, sizeof(mi));

    WinSendMsg(hMenu, MM_QUERYITEM,
               MPFROM2SHORT(IDM_SUBMENU_GAME, TRUE), MPFROMP(&mi));
    hSubGame = mi.hwndSubMenu;

    memset(&mi, 0, sizeof(mi));
    WinSendMsg(hMenu, MM_QUERYITEM,
               MPFROM2SHORT(IDM_SUBMENU_OPTIONS, TRUE), MPFROMP(&mi));
    hSubOpt = mi.hwndSubMenu;

    memset(&mi, 0, sizeof(mi));
    WinSendMsg(hMenu, MM_QUERYITEM,
               MPFROM2SHORT(IDM_SUBMENU_LANG, TRUE), MPFROMP(&mi));
    hSubLang = mi.hwndSubMenu;

    memset(&mi, 0, sizeof(mi));
    WinSendMsg(hMenu, MM_QUERYITEM,
               MPFROM2SHORT(IDM_SUBMENU_HELP, TRUE), MPFROMP(&mi));
    hSubHelp = mi.hwndSubMenu;

    /* Update top-level menu items */
    WinSendMsg(hMenu, MM_SETITEMTEXT,
               MPFROMSHORT(IDM_SUBMENU_GAME),    MPFROMP(tr(STR_MENU_GAME)));
    WinSendMsg(hMenu, MM_SETITEMTEXT,
               MPFROMSHORT(IDM_SUBMENU_OPTIONS), MPFROMP(tr(STR_MENU_OPTIONS)));
    WinSendMsg(hMenu, MM_SETITEMTEXT,
               MPFROMSHORT(IDM_SUBMENU_HELP),    MPFROMP(tr(STR_MENU_HELP)));

    /* Update Game submenu */
    if (hSubGame) {
        WinSendMsg(hSubGame, MM_SETITEMTEXT,
                   MPFROMSHORT(IDM_NORMAL),   MPFROMP(tr(STR_MENU_NEW_NORMAL)));
        WinSendMsg(hSubGame, MM_SETITEMTEXT,
                   MPFROMSHORT(IDM_INVERT),   MPFROMP(tr(STR_MENU_NEW_INVERT)));
        WinSendMsg(hSubGame, MM_SETITEMTEXT,
                   MPFROMSHORT(IDM_SCRAMBLE), MPFROMP(tr(STR_MENU_SCRAMBLE)));
        WinSendMsg(hSubGame, MM_SETITEMTEXT,
                   MPFROMSHORT(IDM_EXIT),     MPFROMP(tr(STR_MENU_EXIT)));
    }

    /* Update Options submenu */
    if (hSubOpt) {
        WinSendMsg(hSubOpt, MM_SETITEMTEXT,
                   MPFROMSHORT(IDM_SUBMENU_LANG), MPFROMP(tr(STR_MENU_LANGUAGE)));
        WinSendMsg(hSubOpt, MM_SETITEMTEXT,
                   MPFROMSHORT(IDM_FRAME),        MPFROMP(tr(STR_MENU_FRAME)));
        WinSendMsg(hSubOpt, MM_SETITEMTEXT,
                   MPFROMSHORT(IDM_SAVEONEXIT),   MPFROMP(tr(STR_MENU_SAVEONEXIT)));
    }

    /* Update Help submenu */
    if (hSubHelp) {
        WinSendMsg(hSubHelp, MM_SETITEMTEXT,
                   MPFROMSHORT(IDM_ABOUT), MPFROMP(tr(STR_MENU_ABOUT)));
    }

    /* Checkmark the selected language */
    if (hSubLang) {
        for (i = 0; i < LANG_COUNT; i++)
            WinCheckMenuItem(hSubLang, IDM_LANG_EN + i, (i == lang));
    }

    /* Checkmark Save settings on exit */
    if (hSubOpt) {
        WinCheckMenuItem(hSubOpt, IDM_SAVEONEXIT, bSaveOnExit);
        WinCheckMenuItem(hSubOpt, IDM_FRAME,      bFrameHidden);
    }

    /* Refresh the status bar so its text switches to the new language */
    {
        HWND hwndCl = WinWindowFromID(hwndFrame, FID_CLIENT);
        if (hwndCl != NULLHANDLE)
            WinInvalidateRect(hwndCl, NULL, FALSE);
    }
}

/* --- frame controls --------------------------------------------- */
static void toggle_frame_controls(void) {
    SWP  swp;
    HWND hwndCl;

    bFrameHidden = !bFrameHidden;
    if (bFrameHidden) {
        WinSetParent(hwndTitleBar, hwndObject, FALSE);
        WinSetParent(hwndSysMenu,  hwndObject, FALSE);
        WinSetParent(hwndMinMax,   hwndObject, FALSE);
        WinSetParent(hwndMenuBar,  hwndObject, FALSE);
    } else {
        WinSetParent(hwndTitleBar, hwndFrame, FALSE);
        WinSetParent(hwndSysMenu,  hwndFrame, FALSE);
        WinSetParent(hwndMinMax,   hwndFrame, FALSE);
        WinSetParent(hwndMenuBar,  hwndFrame, FALSE);
    }
    /* Tell the frame which controls changed so it knows what to reformat */
    WinSendMsg(hwndFrame, WM_UPDATEFRAME,
               (MPARAM)(FCF_TITLEBAR|FCF_SYSMENU|FCF_MINMAX|FCF_MENU), (MPARAM)0);
    /* Resize to current size to trigger WM_FORMATFRAME and reposition client */
    WinQueryWindowPos(hwndFrame, &swp);
    WinSetWindowPos(hwndFrame, NULLHANDLE, swp.x, swp.y, swp.cx, swp.cy,
                    SWP_SIZE | SWP_MOVE);
    /* Repaint frame and client */
    WinInvalidateRect(hwndFrame, NULL, TRUE);
    WinUpdateWindow(hwndFrame);
    hwndCl = WinWindowFromID(hwndFrame, FID_CLIENT);
    if (hwndCl != NULLHANDLE) {
        WinInvalidateRect(hwndCl, NULL, TRUE);
        WinUpdateWindow(hwndCl);
    }
    /* Update checkmark when menu is visible */
    if (!bFrameHidden)
        WinCheckMenuItem(hwndMenuBar, IDM_FRAME, FALSE);
}

/* --- window procedures forward declarations --------------------- */
MRESULT EXPENTRY ClientWndProc (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY AboutDlgProc  (HWND, ULONG, MPARAM, MPARAM);

/* --- main ------------------------------------------------------- */
int main(void)
     {
     static CHAR  szClientClass[] = "Taquin";
     static ULONG flFrameFlags = FCF_SYSMENU  | FCF_TITLEBAR  |
                                 FCF_BORDER   | FCF_MINBUTTON |
                                 FCF_MENU     | FCF_ICON      |
                                 FCF_TASKLIST | FCF_ACCELTABLE;
     HAB          hab;
     HMQ          hmq;
     HWND         hwndClient;
     QMSG         qmsg;

     hab = WinInitialize(0);
     hmq = WinCreateMsgQueue(hab, 0);
     WinRegisterClass(hab, szClientClass, ClientWndProc, 0L, 0);

     /* Create invisible; WM_CREATE will size and show it */
     hwndFrame = WinCreateStdWindow(HWND_DESKTOP, 0L,
                                    &flFrameFlags, szClientClass, "Jeu de Taquin",
                                    0L, NULLHANDLE, ID_RESOURCE, &hwndClient);

     /* Capture frame control handles */
     hwndTitleBar = WinWindowFromID(hwndFrame, FID_TITLEBAR);
     hwndSysMenu  = WinWindowFromID(hwndFrame, FID_SYSMENU);
     hwndMinMax   = WinWindowFromID(hwndFrame, FID_MINMAX);
     hwndMenuBar  = WinWindowFromID(hwndFrame, FID_MENU);

     /* Parking window for Frame Controls */
     hwndObject = WinCreateWindow(HWND_OBJECT, WC_FRAME, "",
                     0L, 0,0,0,0, NULLHANDLE, HWND_TOP, 0, NULL, NULL);

     /* Load settings, apply language */
     load_settings();
     set_language(current_lang);

     while (WinGetMsg(hab, &qmsg, NULLHANDLE, 0, 0))
          WinDispatchMsg(hab, &qmsg);

     if (bSaveOnExit) save_settings();

     if (hwndObject != NULLHANDLE)
          WinDestroyWindow(hwndObject);
     WinDestroyWindow(hwndFrame);
     WinDestroyMsgQueue(hmq);
     WinTerminate(hab);
     return 0;
     }

/* --- ClientWndProc ---------------------------------------------- */
MRESULT EXPENTRY ClientWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
     {
     static SHORT asPuzzle[NUMROWS][NUMCOLS],
                  sBlankRow, sBlankCol, cxSquare, cySquare, barHeight;
     static int   iTimerSecs;
     static BOOL  bTimerRunning, bSolved;
     CHAR         szNum[5], szTimer[32];
     HPS          hps;
     HWND         hwndFr;
     POINTL       ptl;
     RECTL        rcl, rclInvalid, rclIntersect, rclBar;
     SHORT        sRow, sCol, sMouseRow, sMouseCol, i;
     SIZEL        sizl;

     switch (msg)
          {
          case WM_CREATE:
               hps = WinGetPS(hwnd);
               sizl.cx = sizl.cy = 0;
               GpiSetPS(hps, &sizl, PU_LOENGLISH);
               ptl.x = SQUARESIZE;
               ptl.y = SQUARESIZE;
               GpiConvert(hps, CVTC_PAGE, CVTC_DEVICE, 1L, &ptl);
               cxSquare = (SHORT)ptl.x;
               cySquare = (SHORT)ptl.y;
               ptl.x = 0;
               ptl.y = BARSIZE;
               GpiConvert(hps, CVTC_PAGE, CVTC_DEVICE, 1L, &ptl);
               barHeight = (SHORT)ptl.y;
               WinReleasePS(hps);

               iTimerSecs   = 0;
               bTimerRunning = FALSE;
               bSolved      = FALSE;

               rcl.xLeft   = (WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN) -
                               NUMCOLS * cxSquare) / 2;
               rcl.yBottom = (WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN) -
                               (NUMROWS * cySquare + barHeight)) / 2;
               rcl.xRight  = rcl.xLeft   + NUMCOLS * cxSquare;
               rcl.yTop    = rcl.yBottom + NUMROWS * cySquare + barHeight;

               hwndFr = WinQueryWindow(hwnd, QW_PARENT);
               WinCalcFrameRect(hwndFr, &rcl, FALSE);
               WinSetWindowPos(hwndFr, NULL,
                               (SHORT)rcl.xLeft, (SHORT)rcl.yBottom,
                               (SHORT)(rcl.xRight - rcl.xLeft),
                               (SHORT)(rcl.yTop - rcl.yBottom),
                               SWP_MOVE | SWP_SIZE | SWP_ACTIVATE | SWP_SHOW);

               WinSendMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDM_NORMAL), (MPARAM)0);
               return 0;

          case WM_PAINT:
               hps = WinBeginPaint(hwnd, NULLHANDLE, &rclInvalid);
               WinFillRect(hps, &rclInvalid, CLR_BLACK);
               /* Status bar */
               rclBar.xLeft = 0;           rclBar.yBottom = 0;
               rclBar.xRight = NUMCOLS * cxSquare; rclBar.yTop = barHeight;
               WinFillRect(hps, &rclBar, CLR_DARKGRAY);
               if (bSolved)
                    sprintf(szTimer, tr(STR_TIME_SOLVED),
                            iTimerSecs / 60, iTimerSecs % 60);
               else if (bTimerRunning || iTimerSecs > 0)
                    sprintf(szTimer, tr(STR_TIME_RUNNING),
                            iTimerSecs / 60, iTimerSecs % 60);
               else
                    strcpy(szTimer, tr(STR_TIME_IDLE));
               WinDrawText(hps, -1, szTimer, &rclBar,
                           CLR_WHITE, CLR_DARKGRAY, DT_CENTER | DT_VCENTER);
               /* Tiles */
               for (sRow = NUMROWS - 1; sRow >= 0; sRow--)
                    for (sCol = 0; sCol < NUMCOLS; sCol++)
                         {
                         rcl.xLeft   = cxSquare * sCol;
                         rcl.yBottom = cySquare * sRow + barHeight;
                         rcl.xRight  = rcl.xLeft   + cxSquare;
                         rcl.yTop    = rcl.yBottom + cySquare;

                         if (!WinIntersectRect(NULLHANDLE, &rclIntersect,
                                               &rcl, &rclInvalid))
                              continue;

                         if (sRow == sBlankRow && sCol == sBlankCol)
                              WinFillRect(hps, &rcl, CLR_BLACK);
                         else
                              {
                              WinDrawBorder(hps, &rcl, 5, 5,
                                            CLR_PALEGRAY, CLR_DARKGRAY,
                                            DB_STANDARD | DB_INTERIOR);
                              WinDrawBorder(hps, &rcl, 2, 2,
                                            CLR_BLACK, 0L, DB_STANDARD);
                              WinDrawText(hps, -1,
                                     itoa(asPuzzle[sRow][sCol], szNum, 10),
                                          &rcl, CLR_WHITE, CLR_DARKGRAY,
                                          DT_CENTER | DT_VCENTER);
                              }
                         }
               WinEndPaint(hps);
               return 0;

          case WM_TIMER:
               if (SHORT1FROMMP(mp1) == ID_TIMER) {
                    iTimerSecs++;
                    rclBar.xLeft = 0; rclBar.yBottom = 0;
                    rclBar.xRight = NUMCOLS * cxSquare; rclBar.yTop = barHeight;
                    WinInvalidateRect(hwnd, &rclBar, FALSE);
               }
               return 0;

          case WM_BUTTON1DOWN:
               sMouseCol = MOUSEMSG(&msg)->x / cxSquare;
               sMouseRow = (MOUSEMSG(&msg)->y - barHeight) / cySquare;

               if ( sMouseRow < 0         || sMouseCol < 0          ||
                    sMouseRow >= NUMROWS  || sMouseCol >= NUMCOLS   ||
                   (sMouseRow != sBlankRow && sMouseCol != sBlankCol)||
                   (sMouseRow == sBlankRow && sMouseCol == sBlankCol))
                         break;

               if (sMouseRow == sBlankRow)
                    {
                    if (sMouseCol < sBlankCol)
                         for (sCol = sBlankCol; sCol > sMouseCol; sCol--)
                              asPuzzle[sBlankRow][sCol] =
                                   asPuzzle[sBlankRow][sCol - 1];
                    else
                         for (sCol = sBlankCol; sCol < sMouseCol; sCol++)
                              asPuzzle[sBlankRow][sCol] =
                                   asPuzzle[sBlankRow][sCol + 1];
                    }
               else
                    {
                    if (sMouseRow < sBlankRow)
                         for (sRow = sBlankRow; sRow > sMouseRow; sRow--)
                              asPuzzle[sRow][sBlankCol] =
                                   asPuzzle[sRow - 1][sBlankCol];
                    else
                         for (sRow = sBlankRow; sRow < sMouseRow; sRow++)
                              asPuzzle[sRow][sBlankCol] =
                                   asPuzzle[sRow + 1][sBlankCol];
                    }

               rcl.xLeft   = cxSquare *  min(sMouseCol, sBlankCol);
               rcl.yBottom = cySquare *  min(sMouseRow, sBlankRow) + barHeight;
               rcl.xRight  = cxSquare * (max(sMouseCol, sBlankCol) + 1);
               rcl.yTop    = cySquare * (max(sMouseRow, sBlankRow) + 1) + barHeight;

               sBlankRow = sMouseRow;
               sBlankCol = sMouseCol;
               asPuzzle[sBlankRow][sBlankCol] = 0;

               WinInvalidateRect(hwnd, &rcl, FALSE);

               /* Check solved */
               if (bTimerRunning) {
                    BOOL bOk = TRUE;
                    for (sRow = 0; sRow < NUMROWS && bOk; sRow++)
                         for (sCol = 0; sCol < NUMCOLS && bOk; sCol++) {
                              SHORT exp = (SHORT)((sRow == 0 && sCol == NUMCOLS-1) ? 0 :
                                          sCol + 1 + NUMCOLS * (NUMROWS - sRow - 1));
                              if (asPuzzle[sRow][sCol] != exp) bOk = FALSE;
                         }
                    if (bOk) {
                         WinStopTimer(WinQueryAnchorBlock(hwnd), hwnd, ID_TIMER);
                         bTimerRunning = FALSE;
                         bSolved = TRUE;
                         rclBar.xLeft = 0; rclBar.yBottom = 0;
                         rclBar.xRight = NUMCOLS * cxSquare; rclBar.yTop = barHeight;
                         WinInvalidateRect(hwnd, &rclBar, FALSE);
                    }
               }
               break;

          case WM_CHAR:
               if (!(CHARMSG(&msg)->fs & KC_VIRTUALKEY) ||
                     CHARMSG(&msg)->fs & KC_KEYUP)
                         return 0;

               sMouseCol = sBlankCol;
               sMouseRow = sBlankRow;

               switch (CHARMSG(&msg)->vkey)
                    {
                    case VK_LEFT:   sMouseCol++;  break;
                    case VK_RIGHT:  sMouseCol--;  break;
                    case VK_UP:     sMouseRow--;  break;
                    case VK_DOWN:   sMouseRow++;  break;
                    default:        return 0;
                    }
               WinSendMsg(hwnd, WM_BUTTON1DOWN,
                          MPFROM2SHORT(sMouseCol * cxSquare,
                                       sMouseRow * cySquare + barHeight), (MPARAM)0);
               return 0;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case IDM_NORMAL:
                    case IDM_INVERT:
                         for (sRow = 0; sRow < NUMROWS; sRow++)
                              for (sCol = 0; sCol < NUMCOLS; sCol++)
                                   asPuzzle[sRow][sCol] = sCol + 1 +
                                        NUMCOLS * (NUMROWS - sRow - 1);

                         if (COMMANDMSG(&msg)->cmd == IDM_INVERT)
                              {
                              asPuzzle[0][NUMCOLS-2] = NUMCOLS * NUMROWS - 2;
                              asPuzzle[0][NUMCOLS-3] = NUMCOLS * NUMROWS - 1;
                              }
                         asPuzzle[sBlankRow = 0][sBlankCol = NUMCOLS - 1] = 0;
                         if (bTimerRunning)
                              WinStopTimer(WinQueryAnchorBlock(hwnd), hwnd, ID_TIMER);
                         iTimerSecs = 0; bSolved = FALSE; bTimerRunning = TRUE;
                         WinStartTimer(WinQueryAnchorBlock(hwnd), hwnd, ID_TIMER, 1000);
                         WinInvalidateRect(hwnd, NULL, FALSE);
                         return 0;

                    case IDM_SCRAMBLE:
                         WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(
                                       HWND_DESKTOP, SPTR_WAIT, FALSE));
                         srand((int)WinGetCurrentTime(NULLHANDLE));
                         for (i = 0; i < SCRAMBLEREP; i++)
                              {
                              WinSendMsg(hwnd, WM_BUTTON1DOWN,
                                   MPFROM2SHORT(rand() % NUMCOLS * cxSquare,
                                        sBlankRow * cySquare + barHeight), (MPARAM)0);
                              WinUpdateWindow(hwnd);
                              WinSendMsg(hwnd, WM_BUTTON1DOWN,
                                   MPFROM2SHORT(sBlankCol * cxSquare,
                                        rand() % NUMROWS * cySquare + barHeight), (MPARAM)0);
                              WinUpdateWindow(hwnd);
                              }
                         WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(
                                       HWND_DESKTOP, SPTR_ARROW, FALSE));
                         if (bTimerRunning)
                              WinStopTimer(WinQueryAnchorBlock(hwnd), hwnd, ID_TIMER);
                         iTimerSecs = 0; bSolved = FALSE; bTimerRunning = TRUE;
                         WinStartTimer(WinQueryAnchorBlock(hwnd), hwnd, ID_TIMER, 1000);
                         return 0;

                    case IDM_ABOUT:
                         WinDlgBox(HWND_DESKTOP, hwnd, AboutDlgProc,
                                   NULLHANDLE, IDD_ABOUT, (MPARAM)0);
                         return 0;

                    case IDM_EXIT:
                         WinPostMsg(hwnd, WM_QUIT, 0L, 0L);
                         return 0;

                    case IDM_FRAME:
                         toggle_frame_controls();
                         return 0;

                    case IDM_SAVEONEXIT:
                         bSaveOnExit = !bSaveOnExit;
                         WinCheckMenuItem(hwndMenuBar, IDM_SAVEONEXIT, bSaveOnExit);
                         return 0;

                    case IDM_LANG_EN:
                    case IDM_LANG_ES:
                    case IDM_LANG_NL:
                    case IDM_LANG_DE:
                    case IDM_LANG_FR:
                    case IDM_LANG_IT:
                         set_language(COMMANDMSG(&msg)->cmd - IDM_LANG_EN);
                         return 0;
                    }
               break;

          case WM_DESTROY:
               if (bTimerRunning)
                    WinStopTimer(WinQueryAnchorBlock(hwnd), hwnd, ID_TIMER);
               return 0;
          }
     return WinDefWindowProc(hwnd, msg, mp1, mp2);
     }

/* --- AboutDlgProc ----------------------------------------------- */
MRESULT EXPENTRY AboutDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
     {
     switch (msg)
          {
          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case DID_OK:
                    case DID_CANCEL:
                         WinDismissDlg(hwnd, TRUE);
                         return 0L;
                    }
               break;
          }
     return WinDefDlgProc(hwnd, msg, mp1, mp2);
     }
