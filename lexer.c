/*
 * lexer.c
 * 
 * analisador léxico
 * 
 * quebra a entrada e retorna os tokens
 * para que o parser possa analisá-los
 * 
 * Algoritmo implementado pelos
 * alunos do curso de ciência da
 * computação da Unipampa
 * 
 * Public Domain
 */

#include <lexer.h>

/* pega um caractere da entrada de texto */

static gchar pega_caractere ()
{
    gchar c = lookahead;
    
    if (lookahead)
        lookahead = * expr++;
    else
        return 0;
    
    return c;
}

static void pula_espacos ()
{
    do pega_caractere ();
    while (eh_espaco (lookahead));
}

/*
 * se o proximo caractere da entrada for o esperado
 * pega um novo caractere e retorna verdadeiro
 * senão retorna falso
 */

static bool proximo (gchar c)
{
    if (lookahead == c) {
        pega_caractere ();
        return true;
    } else
        return false;
}

static token pega_palavra (int i, size_t tamanho)
{
    for (int j = 0; j < tamanho; j++)
        if (!strcmp (identificador, reservado [i][j].palavra))
            return reservado [i][j].tipo;
    return simbolo;
}

/*
 * se a entrada for uma palavra-chave
 * retorna o token da palavra-chave
 * senão retorna o token simbolo
 */

static token pega_palavra_chave (size_t tamanho)
{
    switch (tamanho) {
        case 1:
            return pega_palavra (0, 1);
        case 2:
            return pega_palavra (1, 3);
        case 3:
            return pega_palavra (2, 4);
        case 4:
            return pega_palavra (3, 1);
        case 5:
            return pega_palavra (4, 2);
        case 6:
            return pega_palavra (5, 4);
        default:
            return simbolo;
    }
}

/*
 * se a entrada for um identificador
 * pega um símbolo ou uma palavra-chave
 */

static token pega_identificador ()
{
    unsigned int i = 0;
    
    memset (identificador, 0, identificador_max);
    do if (i < identificador_max)
        identificador [i++] = pega_caractere ();
    else
        erro ("Identificador muito longo");
    while (eh_letra (lookahead));
    
    return pega_palavra_chave (i);
}

/*
 * se a entrada for um número
 * retorna a parte decimal
 * ou a parte inteira do número
 */

static double pega_decimal (bool decimal)
{
    double numero = 0;
    unsigned int i = 1;
    
    if (!(eh_digito (lookahead)))
        erro ("Número inválido");
    do {
        numero = numero * 10 + pega_caractere () - '0';
        i = decimal ? i * 10 : 1;
    } while (eh_digito (lookahead));
    
    return numero / i;
}

static token pega_numero ()
{
    numero = 0;
    numero = pega_decimal (false);
    if (proximo ('.'))
        numero += pega_decimal (true);
    return numeros;
}

/* função principal do analisador léxico */

token lexan ()
{
loop:
    if (eh_espaco (lookahead)) {
        pula_espacos ();
        goto loop;
    } else if (eh_digito (lookahead))
        return pega_numero ();
    else if (eh_letra (lookahead))
        return pega_identificador ();
    else if (proximo ('('))
        return abre_parentese;
    else if (proximo ('|'))
        return modulo;
    else if (proximo ('^'))
        return exponenciacao;
    else if (proximo ('*'))
        return multiplicacao;
    else if (proximo ('/'))
        return divisao;
    else if (proximo ('+'))
        return mais;
    else if (proximo ('-'))
        return menos;
    else if (proximo (')'))
        return fecha_parentese;
    else if (proximo ('\0'))
        return fim;
    else
        erro ("Caractere desconhecido");
}