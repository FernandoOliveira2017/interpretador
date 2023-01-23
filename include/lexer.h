/*
 * lexer.h
 * 
 * lexer header
 * 
 * Algoritmo implementado pelos
 * alunos do curso de ciência da
 * computação da Unipampa
 * 
 * Public Domain
 */

#ifndef __lex_h__
#define __lex_h__

#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <token.h>

#define identificador_max 64

/* variáveis do analisador léxico */

token tipo;
gchar identificador [identificador_max];
double numero;
const gchar * expr;
gchar lookahead;

token lexan ();

extern void erro (const gchar * mensagem);

#define eh_espaco(a)            a == U' ' || a == U'\t' || a == U'\n'
#define eh_digito(a)            a >= U'0' && a <= U'9'
#define eh_letra(a)             a >= U'a' && a <= U'z' || a >= U'A' && a <= U'Z'

typedef struct
{
    token tipo;
    gchar * palavra;
} palavra_chave;

/* tabela de palavras reservadas */

palavra_chave reservado [6][5] =
{
#define palavra_chave(a, b) {a, b}
#include <palavra_chave.def>
#undef palavra_chave
};

#endif