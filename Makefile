CFLAGS=-g -Wall
LIBS= -lncurses
PREFIX?=/usr/local

all:
	$(CC) $(CFLAGS)  main.c -o  InterControl $(LIBS)

install: all 
	install -d $(DESTDIR)/$(PREFIX)/bin/
	install InterControl $(DESTDIR)/$(PREFIX)/bin

clean:
	rm -f InterControl
