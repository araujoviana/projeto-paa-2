//Apresentação do código: https://www.youtube.com/watch?v=280VpE_OrAA

// Matheus Gabriel Viana Araujo - 10420444
// Beatriz Bellini Prado Garcia - 10419741

#include <stdio.h> // Necessário pro FILE

// Estruturas de dados e definições base

typedef enum { NORMAL, MAGICO, SOBREVIVENCIA, TECNOLOGICO } TipoItem;

typedef enum { FLORESTA, RUINAS, MONTANHAS, TEMPLO } Fase;

// Regras especiais
typedef enum {
    MAGICOS_VALOR_DOBRADO,
    TECNOLOGICOS_INTEIROS,
    SOBREVIVENCIA_DESVALORIZADA,
    TRES_MELHORES_VALOR_PESO
} Regra;

// Item das fases
typedef struct {
    char *nome;
    float peso;
    float preco;
    TipoItem tipo;
    float valor_por_peso;
} Item;

// Fase do jogo
typedef struct {
    char *nome;
    float capacidade;
    Regra regra;
    Item *itens;
    int num_itens;
} FaseJogo;

// Protótipos de funções
void liberar_fase(FaseJogo *fase);
float mochila_fracionaria(Item *itens, int n, float capacidade,
                          float *resultado, int *tam_resultado, Regra regra);
void aplicar_regra_floresta(Item *itens, int n);
void aplicar_regra_montanhas(Item *itens, int n);
void aplicar_regra_templo(Item *itens, int *n);
void escrever_resultado_fase(FILE *arquivo, FaseJogo *fase, float *resultado,
                             float valor_total);
float processar_fase(FaseJogo *fase, FILE *arquivo);
TipoItem str_to_tipo_item(const char *str);
Regra str_to_regra(const char *str);
void trim(char *str);
FaseJogo *ler_fase(FILE *arquivo, int *eof);
void liberar_fase(FaseJogo *fase);
