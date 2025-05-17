// Matheus Gabriel Viana Araujo - 10420444
// Beatriz Bellini Prado Garcia - 10419741

// Referências --
// https://www.geeksforgeeks.org/memmove-in-cc/ -- Informações sobre memmove
// https://codingclutch.com/the-fractional-knapsack-problem-an-in-depth-guide/
// -- Guia sobre o algoritmo da mochila fracionária
// https://www.man7.org/linux/man-pages/man3/fseek.3.html -- Man page do fseek

#include "estruturas.h" // Contém os enums, structs e protótipos usados
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// Funções para o algoritmo da mochila fracionária

float mochila_fracionaria(Item *itens, int n, float capacidade,
                          float *resultado, int *tam_resultado, Regra regra) {
    // Inicializa o array de resultado com zeros
    for (int i = 0; i < n; i++) {
        resultado[i] = 0.0f;
    }

    // Cria uma cópia dos índices para preservar a ordem original
    int *indices = malloc(sizeof(int) * n);
    if (indices == NULL) {
        printf("Erro ao alocar memória para índices\n");
        return 0.0f;
    }

    for (int i = 0; i < n; i++) {
        indices[i] = i;
    }

    // Ordena os itens pelo valor/peso em ordem decrescente
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            float valor_peso_i = itens[indices[i]].valor_por_peso;
            float valor_peso_j = itens[indices[j]].valor_por_peso;

            if (valor_peso_i < valor_peso_j) {
                int tmp = indices[i];
                indices[i] = indices[j];
                indices[j] = tmp;
            }
        }
    }

    float cap_restante = capacidade;
    float total = 0.0f;
    *tam_resultado = 0;

    // Escolhe os itens em ordem decrescente de valor/peso
    for (int i = 0; i < n && cap_restante > 0; i++) {
        int idx = indices[i];

        // Para a fase de ruínas, itens tecnológicos não podem ser fracionados
        if (itens[idx].tipo == TECNOLOGICO && regra == TECNOLOGICOS_INTEIROS &&
            itens[idx].peso > cap_restante) {
            continue; // Pula este item se for tecnológico e não couber inteiro
        }

        if (itens[idx].peso <= cap_restante) {
            // Leva o item inteiro
            resultado[idx] = 1.0f;
            cap_restante -= itens[idx].peso;
            total += itens[idx].preco;
            (*tam_resultado)++;
        } else {
            // Leva fração do item (apenas se não for tecnológico na fase de
            // ruínas)
            if (!(itens[idx].tipo == TECNOLOGICO &&
                  regra == TECNOLOGICOS_INTEIROS)) {
                resultado[idx] = cap_restante / itens[idx].peso;
                total += resultado[idx] * itens[idx].preco;
                cap_restante = 0;
                (*tam_resultado)++;
            }
        }
    }

    free(indices);
    return total;
}

void aplicar_regra_floresta(Item *itens, int n) {
    for (int i = 0; i < n; i++) {
        if (itens[i].tipo == MAGICO) {
            itens[i].preco *= 2;
            // Atualiza a relação valor/peso
            itens[i].valor_por_peso = itens[i].preco / itens[i].peso;
        }
    }
}

void aplicar_regra_montanhas(Item *itens, int n) {
    for (int i = 0; i < n; i++) {
        if (itens[i].tipo == SOBREVIVENCIA) {
            itens[i].preco *= 0.8;
            // Atualiza a relação valor/peso
            itens[i].valor_por_peso = itens[i].preco / itens[i].peso;
        }
    }
}

void aplicar_regra_templo(Item *itens, int *n) {
    if (*n <= 3)
        return; // Não precisa filtrar se já temos 3 ou menos itens

    // Ordena os itens pelo valor/peso
    for (int i = 0; i < *n - 1; i++) {
        for (int j = i + 1; j < *n; j++) {
            if (itens[i].valor_por_peso < itens[j].valor_por_peso) {
                Item temp = itens[i];
                itens[i] = itens[j];
                itens[j] = temp;
            }
        }
    }

    // Libera a memória dos itens que serão descartados
    for (int i = 3; i < *n; i++) {
        if (itens[i].nome != NULL) {
            free(itens[i].nome);
            itens[i].nome = NULL;
        }
    }

    // Mantém apenas os três melhores
    *n = 3;
}

/**
 * Escreve os resultados no terminal e no arquivo de saída.
 */

void escrever_resultado_fase(FILE *arquivo, FaseJogo *fase, float *resultado,
                             float valor_total) {
    fprintf(arquivo, "--- FASE: %s ---\n", fase->nome);
    printf("--- FASE: %s ---\n", fase->nome);

    fprintf(arquivo, "Capacidade da mochila: %.2f kg\n", fase->capacidade);
    printf("Capacidade da mochila: %.2f kg\n", fase->capacidade);

    switch (fase->regra) {
    case MAGICOS_VALOR_DOBRADO:
        fprintf(arquivo, "Regra aplicada: Itens mágicos com valor dobrado\n");
        printf("Regra aplicada: Itens mágicos com valor dobrado\n");
        break;
    case TECNOLOGICOS_INTEIROS:
        fprintf(
            arquivo,
            "Regra aplicada: Itens tecnologicos não podem ser fracionados\n");
        printf(
            "Regra aplicada: Itens tecnologicos não podem ser fracionados\n");
        break;
    case SOBREVIVENCIA_DESVALORIZADA:
        fprintf(
            arquivo,
            "Regra aplicada: Itens de sobrevivencia perdem 20%% do valor\n");
        printf("Regra aplicada: Itens de sobrevivencia perdem 20%% do valor\n");
        break;
    case TRES_MELHORES_VALOR_PESO:
        fprintf(arquivo, "Regra aplicada: Apenas os tres itens com maior "
                         "valor/peso podem ser escolhidos\n");
        printf("Regra aplicada: Apenas os tres itens com maior valor/peso "
               "podem ser escolhidos\n");
        break;
    }

    for (int i = 0; i < fase->num_itens; i++) {
        if (resultado[i] > 0.0001) {
            if (resultado[i] >= 0.999) {
                fprintf(arquivo, "Pegou (inteiro) %s (%.2fkg, R$ %.2f)\n",
                        fase->itens[i].nome, fase->itens[i].peso,
                        fase->itens[i].preco);
                printf("Pegou (inteiro) %s (%.2fkg, R$ %.2f)\n",
                       fase->itens[i].nome, fase->itens[i].peso,
                       fase->itens[i].preco);
            } else {
                float peso_fracao = resultado[i] * fase->itens[i].peso;
                float valor_fracao = resultado[i] * fase->itens[i].preco;
                fprintf(arquivo, "Pegou (fracionado) %s (%.2fkg, R$ %.2f)\n",
                        fase->itens[i].nome, peso_fracao, valor_fracao);
                printf("Pegou (fracionado) %s (%.2fkg, R$ %.2f)\n",
                       fase->itens[i].nome, peso_fracao, valor_fracao);
            }
        }
    }

    fprintf(arquivo, "Lucro da fase: R$ %.2f\n\n", valor_total);
    printf("Lucro da fase: R$ %.2f\n\n", valor_total);
}

float processar_fase(FaseJogo *fase, FILE *arquivo) {
    // Aplica as regras específicas da fase
    switch (fase->regra) {
    case MAGICOS_VALOR_DOBRADO:
        aplicar_regra_floresta(fase->itens, fase->num_itens);
        break;
    case SOBREVIVENCIA_DESVALORIZADA:
        aplicar_regra_montanhas(fase->itens, fase->num_itens);
        break;
    case TRES_MELHORES_VALOR_PESO:
        aplicar_regra_templo(fase->itens, &(fase->num_itens));
        break;
    default:
        break;
    }

    // Aloca array para resultados
    float *resultado = (float *)malloc(fase->num_itens * sizeof(float));
    if (resultado == NULL) {
        printf("Erro ao alocar memória para resultado\n");
        return 0.0f;
    }

    int tam_resultado = 0;
    float valor_total = 0.0f;

    // Executa o algoritmo guloso
    valor_total =
        mochila_fracionaria(fase->itens, fase->num_itens, fase->capacidade,
                            resultado, &tam_resultado, fase->regra);

    // Escreve os resultados no arquivo
    escrever_resultado_fase(arquivo, fase, resultado, valor_total);

    free(resultado);
    return valor_total;
}

TipoItem str_to_tipo_item(const char *str) {
    if (strcmp(str, "magico") == 0) {
        return MAGICO;
    } else if (strcmp(str, "sobrevivencia") == 0) {
        return SOBREVIVENCIA;
    } else if (strcmp(str, "tecnologico") == 0) {
        return TECNOLOGICO;
    } else {
        return NORMAL;
    }
}

Regra str_to_regra(const char *str) {
    if (strcmp(str, "MAGICOS_VALOR_DOBRADO") == 0) {
        return MAGICOS_VALOR_DOBRADO;
    } else if (strcmp(str, "TECNOLOGICOS_INTEIROS") == 0) {
        return TECNOLOGICOS_INTEIROS;
    } else if (strcmp(str, "SOBREVIVENCIA_DESVALORIZADA") == 0) {
        return SOBREVIVENCIA_DESVALORIZADA;
    } else if (strcmp(str, "TRES_MELHORES_VALOR_PESO") == 0) {
        return TRES_MELHORES_VALOR_PESO;
    } else {
        return MAGICOS_VALOR_DOBRADO; // Default
    }
}

/**
 * Remove espaços em branco do início e fim de uma string.
 */
void trim(char *str) {
    if (str == NULL)
        return;

    char *start = str;
    while (isspace(*start))
        start++;

    if (*start == 0) {
        *str = 0;
        return;
    }

    char *end = str + strlen(str) - 1;
    while (end > start && isspace(*end))
        end--;

    *(end + 1) = 0;

    if (start > str) {
        memmove(str, start, strlen(start) + 1);
    }
}

/**
 * Lê uma fase do jogo a partir do arquivo de entrada.
 */
FaseJogo *ler_fase(FILE *arquivo, int *eof) {
    FaseJogo *fase = (FaseJogo *)malloc(sizeof(FaseJogo));
    if (fase == NULL) {
        printf("Erro ao alocar memória para fase\n");
        return NULL;
    }

    fase->nome = NULL;
    fase->capacidade = 0.0f;
    fase->regra = MAGICOS_VALOR_DOBRADO;
    fase->itens = NULL;
    fase->num_itens = 0;

    char linha[256];
    int lendo_itens = 0;
    int capacidade_itens = 10; // Capacidade inicial do array de itens

    fase->itens = (Item *)malloc(capacidade_itens * sizeof(Item));
    if (fase->itens == NULL) {
        printf("Erro ao alocar memória para itens\n");
        free(fase);
        return NULL;
    }

    // Inicializa os nomes dos itens como NULL para facilitar a limpeza em caso
    // de erro
    for (int i = 0; i < capacidade_itens; i++) {
        fase->itens[i].nome = NULL;
    }

    while (fgets(linha, sizeof(linha), arquivo)) {
        // Remove quebra de linha
        linha[strcspn(linha, "\r\n")] = 0;

        // Ignora linhas vazias
        if (strlen(linha) == 0) {
            continue;
        }

        // Verifica se é o início de uma nova fase
        if (strncmp(linha, "FASE:", 5) == 0) {
            // Se já estamos lendo itens de uma fase, esta é uma nova fase
            if (lendo_itens && fase->nome != NULL) {
                // Volta o ponteiro do arquivo para ler esta linha novamente
                fseek(arquivo, -strlen(linha) - 2, SEEK_CUR);
                *eof = 0;
                return fase;
            }

            // Nome da fase
            char *nome_fase = linha + 5;
            trim(nome_fase);
            fase->nome = strdup(nome_fase);
            lendo_itens = 1;
        } else if (strncmp(linha, "CAPACIDADE:", 11) == 0) {
            // Capacidade da mochila
            sscanf(linha + 11, "%f", &(fase->capacidade));
        } else if (strncmp(linha, "REGRA:", 6) == 0) {
            // Regra especial
            char regra_str[50];
            sscanf(linha + 6, "%s", regra_str);
            fase->regra = str_to_regra(regra_str);
        } else if (strncmp(linha, "ITEM:", 5) == 0) {
            // Parseia um item
            char nome[100];
            float peso, preco;
            char tipo_str[50];

            // Divide a linha em partes
            char *token = strtok(linha + 5, ",");
            if (token == NULL)
                continue;
            strncpy(nome, token, sizeof(nome));
            trim(nome);

            token = strtok(NULL, ",");
            if (token == NULL)
                continue;
            peso = atof(token);

            token = strtok(NULL, ",");
            if (token == NULL)
                continue;
            preco = atof(token);

            token = strtok(NULL, ",");
            if (token == NULL)
                continue;
            strncpy(tipo_str, token, sizeof(tipo_str));
            trim(tipo_str);

            // Verifica se precisa aumentar a capacidade do array de itens
            if (fase->num_itens >= capacidade_itens) {
                int new_capacidade = capacidade_itens * 2;
                Item *new_itens =
                    (Item *)realloc(fase->itens, new_capacidade * sizeof(Item));
                if (new_itens == NULL) {
                    printf("Erro ao realocar memória para itens\n");
                    liberar_fase(fase);
                    return NULL;
                }
                fase->itens = new_itens;

                // Inicializa os novos espaços com NULL
                for (int i = capacidade_itens; i < new_capacidade; i++) {
                    fase->itens[i].nome = NULL;
                }

                capacidade_itens = new_capacidade;
            }

            // Adiciona o item ao array
            fase->itens[fase->num_itens].nome = strdup(nome);
            fase->itens[fase->num_itens].peso = peso;
            fase->itens[fase->num_itens].preco = preco;
            fase->itens[fase->num_itens].tipo = str_to_tipo_item(tipo_str);
            fase->itens[fase->num_itens].valor_por_peso = preco / peso;
            fase->num_itens++;
        }
    }

    // Chegou ao final do arquivo
    *eof = 1;
    return fase;
}

/**
 * Libera a memória alocada para uma fase.
 */
void liberar_fase(FaseJogo *fase) {
    if (fase == NULL)
        return;

    if (fase->nome != NULL) {
        free(fase->nome);
    }

    if (fase->itens != NULL) {
        for (int i = 0; i < fase->num_itens; i++) {
            if (fase->itens[i].nome != NULL) {
                free(fase->itens[i].nome);
            }
        }
        free(fase->itens);
    }

    free(fase);
}

int main(int argc, char *argv[]) {
    // Verificação do número de argumentos
    if (argc != 3) {
        printf("Erro: código espera 2 argumentos: um arquivo de entrada e um "
               "de saida.\n");
        printf("Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    FILE *arquivo_entrada = fopen(argv[1], "r");
    if (arquivo_entrada == NULL) {
        printf("Erro ao abrir o arquivo de entrada: %s\n", argv[1]);
        return 1;
    }

    FILE *arquivo_saida = fopen(argv[2], "w");
    if (arquivo_saida == NULL) {
        printf("Erro ao abrir o arquivo de saída: %s\n", argv[2]);
        fclose(arquivo_entrada);
        return 1;
    }

    float lucro_total = 0.0f;
    int eof = 0;

    // Lê cada fase do arquivo e processa
    while (!eof) {
        FaseJogo *fase = ler_fase(arquivo_entrada, &eof);
        if (fase != NULL && fase->nome != NULL) {
            // Processa a fase usando o algoritmo guloso
            float lucro_fase = processar_fase(fase, arquivo_saida);
            lucro_total += lucro_fase;

            // Libera a memória da fase
            liberar_fase(fase);
        } else if (fase != NULL) {
            liberar_fase(fase);
        }
    }

    // Escreve o lucro total no arquivo de saída
    fprintf(arquivo_saida, "Lucro total do jogo: R$ %.2f\n", lucro_total);

    // Fecha os arquivos
    fclose(arquivo_entrada);
    fclose(arquivo_saida);

    printf("Processamento concluído. Resultado gravado em %s\n", argv[2]);

    return 0;
}
