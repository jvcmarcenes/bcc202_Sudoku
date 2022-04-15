#ifndef SUDOKU
#define SUDOKU

typedef struct celula Celula;

void print_celula(Celula *c);
void print_celulas(Celula *c, int qtd);

typedef struct conflito Conflito;

void print_conflito(Conflito *c);
void print_conflitos(Conflito *c, int qtd);

typedef struct sudoku Sudoku;

Sudoku *novo_sudoku();

void free_sudoku();

int ler_sudoku(Sudoku *sudoku, char *arq);

Celula* celulas_vazias(Sudoku *sudoku, int *qtd_vazias);

void conflitos(Sudoku *sudoku, Conflito **conflitos, int *qtd_conflitos);

void print_vazia(Celula c, Sudoku *sudoku);

void print_vazias(Celula *c, int qtd, Sudoku *sudoku);

#endif
