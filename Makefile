CC=gcc
CFLAGS=-Wall
LDFLAGS=-lsodium

all: jp-decrypt

jp-decrypt: jp-decrypt.c base64.o
	$(CC) $(CFLAGS) jp-decrypt.c base64.o -lsodium -o jp-decrypt
