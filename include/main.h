/*
 * main.h
 * 
 * main header
 * 
 * Algoritmo implementado pelos
 * alunos do curso de ciência da
 * computação da Unipampa
 * 
 * Public Domain
 */

#ifndef __main_h__
#define __main_h__

#include <gtk/gtk.h>
#include <cairo.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <errno.h>

/* componentes da interface gráfica */

GtkWidget * janela;
GtkWidget * na, * nb;
GtkWidget * funcao;
GtkWidget * grafico;

bool erro_de_sintaxe = false;
bool usa_variaveis = false;
bool funcao_impropria = false;
bool ponto_indefinido = false;

/* declaração da lista de linhas do gráfico */

struct ponto
{
    double x;
    double y;
    struct ponto * anterior;
};

struct no
{
    struct ponto * linha;
    struct no * anterior;
} * lista = NULL;

extern double absoluto (double eval);
extern double precisao (double op);
extern double parse (const gchar * expr, double x);

#endif