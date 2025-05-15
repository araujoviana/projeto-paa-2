// Matheus Gabriel Viana Araujo - 10420444
// Beatriz Bellini Prado Garcia - 10419741

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum Fase { FLORESTA, RUINAS, MONTANHAS, TEMPLO };

enum Tipo { NORMAL, SOBREVIVENCIA, TECNOLOGICO, MAGICO };

typedef struct {
    char *nome;
    float peso;
    float preco;
    enum Tipo tipo;
} Item;

void ler_arquivo(FILE *fp, Item **items, enum Fase *fase) {
    char linha[256];
    int capacidade = 10;
    int qtd = 0;
    *items = malloc(capacidade * sizeof(Item));

    while (fgets(linha, sizeof(linha), fp)) {
        // Remove quebra de linha
        linha[strcspn(linha, "\r\n")] = 0;

        // Verifica linha de fase
        if (strncmp(linha, "FASE:", 5) == 0) {
            if (strstr(linha, "Floresta"))
                *fase = FLORESTA;
            else if (strstr(linha, "Ruinas"))
                *fase = RUINAS;
            else if (strstr(linha, "Montanhas"))
                *fase = MONTANHAS;
            else if (strstr(linha, "Templo"))
                *fase = TEMPLO;
        }

        // Verifica linha de item
        if (strncmp(linha, "ITEM:", 5) == 0) {
            char *dados = linha + 5;
            while (dados && *dados) {
                char nome[100], tipo_str[32];
                float peso, preco;

                // Pega só uma entrada de item
                int lidos = sscanf(dados, " %[^,], %f, %f, %s", nome, &peso,
                                   &preco, tipo_str);
                if (lidos < 4)
                    break;

                // Adiciona ao vetor
                if (qtd >= capacidade) {
                    capacidade *= 2;
                    *items = realloc(*items, capacidade * sizeof(Item));
                }

                (*items)[qtd].nome = strdup(nome);
                (*items)[qtd].peso = peso;
                (*items)[qtd].preco = preco;

                if (strcmp(tipo_str, "normal") == 0)
                    (*items)[qtd].tipo = NORMAL;
                else if (strcmp(tipo_str, "magico") == 0)
                    (*items)[qtd].tipo = MAGICO;
                else if (strcmp(tipo_str, "sobrevivencia") == 0)
                    (*items)[qtd].tipo = SOBREVIVENCIA;
                else if (strcmp(tipo_str, "tecnologico") == 0)
                    (*items)[qtd].tipo = TECNOLOGICO;

                qtd++;

                // Move para o próximo "ITEM:" se estiver grudado
                dados = strstr(dados + 1, "ITEM:");
                if (dados)
                    dados += 5;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    // Verificação do número de argumentos
    if (argc != 3) {
        printf("Erro: código espera 2 argumentos: um arquivo de entrada e um "
               "de saida.\n");
        return 1;
    }

    // Ponteiro do arquivo de entrada
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    Item *itens = NULL;
    enum Fase fase;

    ler_arquivo(fp, &itens, &fase);

    for (int i = 0; i < ...; i++) {
        printf("Item: %s, Peso: %.2f, Preço: %.2f, Tipo: %d\n", itens[i].nome,
               itens[i].peso, itens[i].preco, itens[i].tipo);
    }

    fclose(fp); // Fecha o arquivo de entrada

    return 0;
}
