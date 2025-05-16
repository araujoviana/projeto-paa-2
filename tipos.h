#ifndef TIPOS_H
#define TIPOS_H

enum Fase { FLORESTA, RUINAS, MONTANHAS, TEMPLO };

enum Tipo { NORMAL, SOBREVIVENCIA, TECNOLOGICO, MAGICO };

typedef struct {
    char *nome;
    float peso;
    float preco;
    enum Tipo tipo;
} Item;

#endif
