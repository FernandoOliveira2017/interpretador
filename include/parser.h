/*
 * parser.h
 * 
 * parser header
 * 
 * Algoritmo implementado pelos
 * alunos do curso de ciência da
 * computação da Unipampa
 * 
 * Public Domain
 */

#ifndef __parser_h__
#define __parser_h__

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <token.h>

extern const gchar * expr;
extern bool erro_de_sintaxe;
extern bool usa_variaveis;
extern bool funcao_impropria;
extern bool ponto_indefinido;
extern gchar lookahead;
extern token tipo;
extern gchar identificador [];
extern double numero;

extern token lexan ();
extern void erro (const gchar * mensagem);
extern void novo_no ();

/* função recursiva do analisador sintático */

static double soma (double x);

#define eh_produto(a)               (a == multiplicacao || a == divisao)
#define eh_soma(a)                  (a == mais || a == menos)
#define eh_inteiro(a)               ((a)/((long) (a)) == 1.0)

#endif