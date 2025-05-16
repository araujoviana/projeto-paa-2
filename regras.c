// Contém as regras especiais por fase

#include "tipos.h"

void regra_floresta(Item* items, int qtde_items) {
    for (int i = 0; i < qtde_items; i++)
    {
        if (items[i].tipo == MAGICO) {
            items[i].preco *= 2;
        }
    }
}

void regra_ruinas(Item* items, int qtde_items) {
    // TODO como impedir fracionamento?
    for (int i = 0; i < qtde_items; i++)
    {
        continue;
    }
}

void regra_montanhas(Item* items, int qtde_items) {
    for (int i = 0; i < qtde_items; i++)
    {
        if (items[i].tipo == SOBREVIVENCIA) {
            items[i].preco *= 0.8;
        }
    }
}

void regra_subterraneo(Item* items, int qtde_items) {
    // TODO regra diferente
    for (int i = 0; i < qtde_items; i++)
    {
        continue;
    }
}