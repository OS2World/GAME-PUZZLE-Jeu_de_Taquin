@echo off
rem compile.cmd -- Build Jeu de Taquin for ArcaOS
rem Requires: OpenWatcom 2.0, OS/2 Toolkit 4.5

if not exist bin md bin
echo Building Jeu de Taquin...
wmake -f makefile.os2 -a 2>&1 | tee -a bin\compile.log
echo Done. See bin\compile.log