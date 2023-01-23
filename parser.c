/*
 * parser.c
 * 
 * analisador sintático descendente recursivo
 * 
 * aqui estão as funções correspondentes à cada
 * operação matemática, cada uma conforme a sua
 * ordem de prioridade na precedência de operadores
 * 
 * Algoritmo implementado pelos
 * alunos do curso de ciência da
 * computação da Unipampa
 * 
 * Public Domain
 */

#include <parser.h>

/* inicializa as variáveis do lexer */

static void inicia (const gchar * expressao)
{
    expr = expressao;
    lookahead = * expr++;
    tipo = lexan ();
}

/*
 * se o proximo token for o esperado
 * pega um novo token e retorna verdadeiro
 * senão retorna falso
 */

static bool proximo_token (token t)
{
    if (tipo == t) {
        tipo = lexan ();
        return true;
    } else
        return false;
}

/* calcula o valor absoluto de eval */

double absoluto (double eval)
{
    return eval < 0.0 ? (-eval) : eval;
}

/* define se a função é imprópria ou não */

static double impropria ()
{
    funcao_impropria = true;
    ponto_indefinido = true;
    return 0.0;
}

double precisao (double op)
{
    return ((long) (op * 1000.0))/1000.0;
}

static double pega_rad (double op)
{
    return op - precisao (((long) (op/M_2_PI)) * M_2_PI);
}

static double pega_pi_rad (double op)
{
    return pega_rad (op) == 0.0 || pega_rad (op) == 3.141 || pega_rad (op) == -3.141;
}

static double pega_pi_2_rad (double op)
{
    return pega_rad (op) == 1.570 || pega_rad (op) == -1.570 || pega_rad (op) == 4.712 || pega_rad (op) == -4.712;
}

static double pega_sen (double op)
{
    if (pega_pi_rad (op))
        return 0.0;
    else if (pega_pi_2_rad (op))
        return 1.0;
    else
        return precisao (sin (op));
}

static double pega_cos (double op)
{
    if (pega_pi_rad (op))
        return 1.0;
    else if (pega_pi_2_rad (op))
        return 0.0;
    else
        return precisao (cos (op));
}

/* pega uma variável, um número, um módulo ou uma expressão entre parênteses */

static double fator (double x)
{
    double eval;
    
    if (tipo == simbolo) {
        if (!usa_variaveis)
            erro ("Não use variáveis em expressões");
        else if (!strcmp (identificador, "x"))
            eval = x;
        else
            erro (strcat (identificador, " não é uma variável"));
        tipo = lexan ();
    } else if (tipo == numeros) {
        eval = numero;
        tipo = lexan ();
    } else if (proximo_token (numero_pi))
        eval = M_PI;
    else if (proximo_token (numero_de_euler))
        eval = M_E;
    else if (proximo_token (modulo)) {
        eval = absoluto (soma (x));
        if (!proximo_token (modulo))
            erro ("Módulo não foi fechado");
    } else if (proximo_token (abre_parentese)) {
        eval = soma (x);
        if (!proximo_token (fecha_parentese))
            erro ("Parêntese não foi fechado");
    }
    
    return eval;
}

/* pega um sinal - ou + antes de um fator */

static double unario (double x)
{
    if (proximo_token (mais))
        return fator (x);
    else if (proximo_token (menos))
        return -fator (x);
    else
        return fator (x);
}

/* pega uma função matemática */

static double funcao (double x)
{
    if (proximo_token (arco_cosseno)) {
        double op = funcao (x);
        if (op < -1.0 || op > 1.0)
            ponto_indefinido = true;
        return op >= -1.0 && op <= 1.0 ? acos (op) : 0.0;
    } else if (proximo_token (arco_seno)) {
        double op = funcao (x);
        if (op < -1.0 || op > 1.0)
            ponto_indefinido = true;
        return op >= -1.0 && op <= 1.0 ? asin (op) : 0.0;
    } else if (proximo_token (arco_tangente))
        return atan (funcao (x));
    else if (proximo_token (cosseno))
        return pega_cos (funcao (x));
    else if (proximo_token (cossecante)) {
        double op = pega_sen (funcao (x));
        return op ? 1.0/op : impropria ();
    } else if (proximo_token (cotangente)) {
        double op = funcao (x);
        double a = pega_cos (op);
        double b = pega_sen (op);
        return b ? a/b : impropria ();
    } else if (proximo_token (logaritmo_neperiano)) {
        double op = funcao (x);
        return op > 0.0 ? log (op) : impropria ();
    } else if (proximo_token (logaritmo_decimal)) {
        double op = funcao (x);
        return op > 0.0 ? log10 (op) : impropria ();
    } else if (proximo_token (secante)) {
        double op = pega_cos (funcao (x));
        return op ? 1.0/op : impropria ();
    } else if (proximo_token (seno))
        return pega_sen (funcao (x));
    else if (proximo_token (tangente)) {
        double op = funcao (x);
        double a = pega_sen (op);
        double b = pega_cos (op);
        return b ? a/b : impropria ();
    } else if (proximo_token (radiciacao_2)) {
        double op = funcao (x);
        if (op < 0.0)
            ponto_indefinido = true;
        return op >= 0.0 ? sqrt (op) : 0.0;
    } else if (proximo_token (radiciacao_3))
        return cbrt (funcao (x));
    else
        return unario (x);
}

/* pega uma pontência */

static double potencia (double x)
{
    double eval = funcao (x);
    
    while (proximo_token (exponenciacao)) {
            double op = funcao (x);
            if (eval == 0.0 && op <= 0.0)
                eval = impropria ();
            else if (eval < 0.0 && !(eh_inteiro (op))) {
                ponto_indefinido = true;
                eval = 0.0;
            } else
                eval = pow (eval, op);
    }
    
    return eval;
}

/* pega um produto */

static double produto (double x)
{
    double eval = potencia (x);
    
    while (eh_produto (tipo))
        if (proximo_token (multiplicacao))
            eval *= potencia (x);
        else if (proximo_token (divisao)) {
            double op = potencia (x);
            if (op)
                eval /= op;
            else
                return impropria ();
        }
    
    return eval;
}

/* pega uma soma */

static double soma (double x)
{
    double eval = produto (x);
    
    while (eh_soma (tipo))
        if (proximo_token (mais))
            eval += produto (x);
        else if (proximo_token (menos))
            eval -= produto (x);
    
    return eval;
}

/* funcao principal do analisador sintático */

double parse (const gchar * expressao, double x)
{
    double eval;
    
    inicia (expressao);
    errno = 0;
    eval = soma (x);
    if (!proximo_token (fim))
        erro_de_sintaxe = true;
    
    return eval;
}