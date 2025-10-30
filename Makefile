CC = gcc
CFLAGS = -lsqlite3
SOURCES = src/main.c

bibCLI: src/main.c src/bible.h src/bibdb.h
	$(CC) $(SOURCES) -o bibCLI $(CFLAGS)

clean:
	rm bibCLI