# makefile.wat -- Jeu de Taquin for ArcaOS/OpenWatcom 2.0

CC     = wcc386
RC     = wrc
LINK   = wlink

CFLAGS = -bt=os2 -mf -5 -fpi -Oaxt -W3 -ze -d0 -i=src -i=$(OS2TK)\h

TARGET = bin\taquin.exe

all: $(TARGET)

bin\taquin.obj: src\taquin.c src\taquin.h src\lang.h
	$(CC) $(CFLAGS) -fo=bin\taquin.obj src\taquin.c

bin\taquin.res: src\taquin.rc src\taquin.h src\taquin.ico
	$(RC) -r -i=src -i=$(OS2TK)\h -fo=bin\taquin.res src\taquin.rc

$(TARGET): bin\taquin.obj bin\taquin.res
	$(LINK) system os2v2_pm name $(TARGET) file bin\taquin.obj option stack=65536 option heap=4096 option map=bin\taquin.map
	$(RC) -q bin\taquin.res $(TARGET)

clean:
	-del bin\taquin.obj
	-del bin\taquin.res
	-del bin\taquin.exe
	-del bin\taquin.map
