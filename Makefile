all: ndef main.c
	clang -o main ndef.o main.c

ndef: ndef.c ndef.h
	clang -c ndef.c
