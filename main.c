// Matheus Gabriel Viana Araujo - 10420444
// Beatriz Bellini Prado Garcia - 10419741

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tipos.h"

// Protótipo de parsing de arquivo
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

// PROTOTIPO DE MOCHILA FRACIONARIA
/**
 * Implementa o algoritmo da mochila fracionária usando uma abordagem baseada em
 * programação dinâmica. Esta função implementa uma estratégia híbrida que
 * combina elementos de programação dinâmica com a natureza contínua da mochila
 * fracionária.
 *
 * @param items Array de itens disponíveis
 * @param n Número de itens
 * @param capacidade Capacidade máxima da mochila
 * @param resultado Array para armazenar a fração de cada item escolhido (deve
 * ser pré-alocado)
 * @return Valor total obtido
 */
float mochila_fracionaria_pd(Item *items, int n, float capacidade,
                             float *resultado) {
    if (items == NULL || resultado == NULL || n <= 0 || capacidade <= 0) {
        return 0.0f;
    }

    // Inicializa o array de resultado com zeros
    for (int i = 0; i < n; i++) {
        resultado[i] = 0.0f;
    }

    // Cria uma tabela para armazenar o valor máximo para cada capacidade
    // Para uma abordagem contínua, usamos discretização da capacidade
    int max_cap = (int)(capacidade * 100) + 1; // Precisão de 0.01
    float *dp = (float *)malloc(max_cap * sizeof(float));
    if (dp == NULL) {
        printf("Erro ao alocar memória para tabela de programação dinâmica\n");
        return 0.0f;
    }

    // Inicializa a tabela com zeros
    for (int i = 0; i < max_cap; i++) {
        dp[i] = 0.0f;
    }

    // Para cada item, calcula o melhor valor para cada capacidade
    for (int i = 0; i < n; i++) {
        // Calcula valor por unidade de peso
        float valor_por_peso = items[i].preco / items[i].peso;

        // Para cada capacidade possível (de maior para menor para evitar
        // contagem duplicada)
        for (int w = max_cap - 1; w >= 0; w--) {
            float cap_atual = w / 100.0f;

            // Quanto deste item podemos levar com a capacidade disponível
            float peso_maximo =
                (items[i].peso > cap_atual) ? cap_atual : items[i].peso;

            if (peso_maximo > 0) {
                float novo_valor = dp[w] + (valor_por_peso * peso_maximo);
                int nova_cap = (int)((cap_atual - peso_maximo) * 100);

                if (nova_cap >= 0 && dp[nova_cap] + novo_valor > dp[w]) {
                    float fracao = peso_maximo / items[i].peso;
                    resultado[i] = fracao;
                    dp[w] = dp[nova_cap] + (items[i].preco * fracao);
                }
            }
        }
    }

    // Valor máximo está na última posição da tabela
    float valor_total = dp[max_cap - 1];
    free(dp);

    // Reconstrução da solução para garantir os valores corretos no array
    // resultado
    float cap_restante = capacidade;
    memset(resultado, 0, n * sizeof(float));

    // Ordena os itens por valor/peso decrescente (abordagem gulosa)
    typedef struct {
        float valor_por_peso;
        int indice;
    } ItemOrdenado;

    ItemOrdenado *itens_ordenados =
        (ItemOrdenado *)malloc(n * sizeof(ItemOrdenado));
    if (itens_ordenados == NULL) {
        printf("Erro ao alocar memória para ordenação\n");
        return valor_total;
    }

    for (int i = 0; i < n; i++) {
        itens_ordenados[i].valor_por_peso = items[i].preco / items[i].peso;
        itens_ordenados[i].indice = i;
    }

    // Ordenação por valor/peso decrescente
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (itens_ordenados[j].valor_por_peso <
                itens_ordenados[j + 1].valor_por_peso) {
                ItemOrdenado temp = itens_ordenados[j];
                itens_ordenados[j] = itens_ordenados[j + 1];
                itens_ordenados[j + 1] = temp;
            }
        }
    }

    // Reconstrução da solução
    for (int i = 0; i < n && cap_restante > 0; i++) {
        int idx = itens_ordenados[i].indice;
        if (items[idx].peso <= cap_restante) {
            resultado[idx] = 1.0f; // Pega o item inteiro
            cap_restante -= items[idx].peso;
        } else if (cap_restante > 0) {
            resultado[idx] =
                cap_restante / items[idx].peso; // Pega fração do item
            cap_restante = 0;
        }
    }

    free(itens_ordenados);
    return valor_total;
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

    fclose(fp); // Fecha o arquivo de entrada

    return 0;
}
