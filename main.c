/*
 * main.c
 * 
 * nucleo do interpretador
 * 
 * Algoritmo implementado pelos
 * alunos do curso de ciência da
 * computação da Unipampa
 * 
 * Public Domain
 */

#include <main.h>

/* mostra uma mensagem de erro e sai do programa */

void erro (const gchar * mensagem)
{
    GtkWidget * dialogo;
    dialogo = gtk_message_dialog_new    (GTK_WINDOW (janela), 0, GTK_MESSAGE_ERROR
                                        , GTK_BUTTONS_CLOSE, "Erro: %s.", mensagem);
    gtk_dialog_run (GTK_DIALOG (dialogo));
    gtk_widget_destroy (dialogo);
    erro_de_sintaxe = true;
}

/* destroi a lista de linhas do gráfico */

void destroi_linha (struct ponto * linha)
{
    struct ponto * aux = NULL;
    while (linha != NULL) {
        if (aux != NULL)
            free (aux);
        aux = linha;
        if (linha->anterior == NULL) {
            free (linha);
            linha = NULL;
        } else
            linha = linha->anterior;
    }
}

void destroi_lista ()
{
    struct no * aux = NULL;
    while (lista != NULL) {
        if (aux != NULL) {
            destroi_linha (aux->linha);
            free (aux);
        }
        aux = lista;
        if (lista->anterior == NULL) {
            destroi_linha (lista->linha);
            free (lista);
            lista = NULL;
        } else
            lista = lista->anterior;
    }
}

/* faz uma nova lista de linhas */

void nova_lista ()
{
    if (lista != NULL)
        destroi_lista ();
    lista = malloc (sizeof (struct no));
    lista->linha = NULL;
    lista->anterior = NULL;
}

/*
 * coloca um ponto (x, y) na lista de linhas para desenho de linhas no gráfico
 * se o ponto for indefinido como por exemplo tg pi/2 um novo nó será colocado
 * na lista para que essa nova linha não seja seguimento da linha anterior
 */

void desenha_linha (double x, double y)
{
    if (!ponto_indefinido) {
        struct ponto * anterior;
        anterior = lista->linha;
        lista->linha = malloc (sizeof (struct ponto));
        lista->linha->x = x * 25.0 + 320.0;
        lista->linha->y = 240.0 - y * 25.0;
        lista->linha->anterior = anterior;
    } else if ((lista->linha != NULL) && funcao_impropria) {
        struct no * anterior;
        anterior = lista;
        lista = malloc (sizeof (struct no));
        lista->linha = NULL;
        lista->anterior = anterior;
    }
}

/*
 * função callback do botão de cálculo
 * recebe uma expressão e um intervalo
 * e calcula a área e o arco em [a, b]
 */

void expressao ()
{
    GtkWidget * dialogo;
    const gchar * expr;
    const gchar * ia;
    const gchar * ib;
    double x, y, y0;
    double n = 1000;
    double area = 0.0;
    double arco = 0.0;
    double dx = 1/n;
#define mostra_erro(m)  \
{                       \
    erro (m);           \
    goto final;         \
}
    ia = gtk_entry_get_text (GTK_ENTRY (na));
    ib = gtk_entry_get_text (GTK_ENTRY (nb));
    expr = gtk_entry_get_text (GTK_ENTRY (funcao));
    erro_de_sintaxe = false;
    usa_variaveis = false;
    funcao_impropria = false;
    ponto_indefinido = false;
    double a = precisao (parse (ia, 0.0) * n);
    if (ponto_indefinido)
        mostra_erro ("Expressão A inválida");
    if (erro_de_sintaxe)
        mostra_erro ("Erro de sintaxe na expressão A");
    if ((math_errhandling & MATH_ERRNO) && errno != 0)
        mostra_erro ("Overflow na expressão A");
    double b = precisao (parse (ib, 0.0) * n);
    if (ponto_indefinido)
        mostra_erro ("Expressão B inválida");
    if (erro_de_sintaxe)
        mostra_erro ("Erro de sintaxe na expressão B");
    if ((math_errhandling & MATH_ERRNO) && errno != 0)
        mostra_erro ("Overflow na expressão B");
    if (a >= b)
        mostra_erro ("A deve ser menor do que B");
    nova_lista ();
    usa_variaveis = true;
    /* realiza a somatória no intervalo [a, b] */
    for (long i = a; i <= b; i++) {
        x = i * dx;
        y = parse (expr, x);
        if (erro_de_sintaxe)
            mostra_erro ("Erro de sintaxe na função f(x)");
        if ((math_errhandling & MATH_ERRNO) && errno != 0)
            mostra_erro ("Overflow na função f(x)");
        if (!ponto_indefinido) {
            area += absoluto (y) * dx;
            if (lista->linha != NULL)
                arco += sqrt (pow (dx, 2.0) + pow (y-y0, 2.0));
            y0 = y;
        }
        desenha_linha (x, y);
        if (ponto_indefinido)
            ponto_indefinido = false;
    }
    /* desenha o gráfico e mostra os resultados */
    gtk_widget_queue_draw (grafico);
    const char * fmt =  "A área da função %s no intervalo [%lf, %lf] é: %lf\n"
                        "O comprimento da função no intervalo é: %lf";
    dialogo = gtk_message_dialog_new    (GTK_WINDOW (janela), 0, GTK_MESSAGE_INFO,
                                         GTK_BUTTONS_OK, fmt, funcao_impropria ? "impropria"
                                         : "não impropria", a * dx, b * dx, area, funcao_impropria ? INFINITY : arco);
    gtk_dialog_run (GTK_DIALOG (dialogo));
    gtk_widget_destroy (dialogo);
final:;
}

/* funcao callback para desenho do gráfico */

gboolean desenha (GtkWidget * grafico, cairo_t * cr, gpointer dados)
{
    char num [4];
    
    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
    cairo_set_line_width (cr, 1.0);
    cairo_move_to (cr, 0.0, 240.0);
    cairo_line_to (cr, 640.0, 240.0);
    cairo_stroke (cr);
    cairo_move_to (cr, 320.0, 0.0);
    cairo_line_to (cr, 320.0, 480.0);
    cairo_stroke (cr);
    cairo_set_source_rgb (cr, 0.0, 0.0, 1.0);
    cairo_select_font_face (cr, "Liberation Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size (cr, 10);
    for (int x = -((int) 320.0/25.0); x <= ((int) 320.0/25.0); x++) {
        sprintf (num, "%d", x);
        cairo_move_to (cr, x * 25.0 + 313.0, 249.0);
        cairo_show_text (cr, num);
    }
    for (int y = -((int) 240.0/25.0); y <= ((int) 240.0/25.0); y++) {
        sprintf (num, "%d", y);
        cairo_move_to (cr, y < 0 ? 310.0 : 313.0, 249.0 - y * 25.0);
        cairo_show_text (cr, num);
    }
    cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
    cairo_set_line_width (cr, 3.0);
    for (struct no * i = lista; i != NULL; i = i->anterior)
        for (struct ponto * j = i->linha; j != NULL; j = j->anterior)
            if (j->anterior != NULL) {
                cairo_move_to (cr, j->x, j->y);
                cairo_line_to (cr, j->anterior->x, j->anterior->y);
                cairo_stroke (cr);
            }
    
    return FALSE;
}

/* sai do programa */

void destroi ()
{
    gtk_main_quit ();
}

/* função principal do programa */

int main (int argc, char * argv [])
{
    GtkWidget * botao;
    GtkWidget * caixa;
    GtkWidget * caixa2;
    GtkWidget * caixa3;
    
    gtk_init (&argc, &argv);
    janela = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (janela), "Trabalho de Cálculo II");
    g_signal_connect (janela, "destroy", G_CALLBACK (destroi), NULL);
    gtk_window_set_default_size (GTK_WINDOW (janela), 640, 480);
    gtk_window_set_position (GTK_WINDOW (janela), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable (GTK_WINDOW (janela), FALSE);
    caixa = gtk_box_new (FALSE, 0);
    gtk_orientable_set_orientation (GTK_ORIENTABLE (caixa), GTK_ORIENTATION_VERTICAL);
    gtk_container_add (GTK_CONTAINER (janela), caixa);
    caixa2 = gtk_box_new (FALSE, 0);
    gtk_orientable_set_orientation (GTK_ORIENTABLE (caixa2), GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start (GTK_BOX (caixa), caixa2, TRUE, TRUE, 0);
    caixa3 = gtk_box_new (FALSE, 0);
    gtk_orientable_set_orientation (GTK_ORIENTABLE (caixa3), GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start (GTK_BOX (caixa), caixa3, TRUE, TRUE, 0);
    na = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (na), 100);
    gtk_entry_set_placeholder_text (GTK_ENTRY (na), "digite uma expressão a");
    gtk_box_pack_start (GTK_BOX (caixa2), na, TRUE, TRUE, 0);
    nb = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (nb), 100);
    gtk_entry_set_placeholder_text (GTK_ENTRY (nb), "digite uma expressão b");
    gtk_box_pack_start (GTK_BOX (caixa2), nb, TRUE, TRUE, 0);
    funcao = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (funcao), 100);
    gtk_entry_set_placeholder_text (GTK_ENTRY (funcao), "digite uma função f(x) para calcular a integral em [a, b]");
    gtk_box_pack_start (GTK_BOX (caixa3), funcao, TRUE, TRUE, 0);
    botao = gtk_button_new_with_label ("calcular");
    g_signal_connect (botao, "clicked", G_CALLBACK (expressao), caixa3);
    gtk_box_pack_start (GTK_BOX (caixa3), botao, TRUE, TRUE, 0);
    grafico = gtk_drawing_area_new ();
    gtk_box_pack_start (GTK_BOX (caixa), grafico, TRUE, TRUE, 0);
    gtk_widget_set_size_request (grafico, 640, 480);
    g_signal_connect (grafico, "draw", G_CALLBACK (desenha), NULL);
    gtk_widget_show_all (janela);
    gtk_main ();
    
    return 0;
}