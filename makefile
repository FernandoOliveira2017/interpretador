## makefile
##
## Algoritmo implementado pelos
## alunos do curso de ciência da
## computação da Unipampa
## Public Domain

cc = gcc
bibliotecas = -lm `pkg-config --libs --cflags gtk+-3.0`
bandeiras = -I include $(bibliotecas)

interpretador: lexer.c parser.c main.c
	$(cc) $^ -o $@ $(bandeiras)
	echo interpretador compilado com sucesso.