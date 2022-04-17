
#ifndef SUDOKU
#define SUDOKU

typedef struct celula Celula;

void print_celula(Celula *c);

typedef struct conflito Conflito;

void print_conflito(Conflito *c);
void mostra_conflitos(Conflito *c, int qtd);

typedef struct sudoku Sudoku;

Sudoku *novo_sudoku();

void free_sudoku(Sudoku **sudoku);

int ler_sudoku(Sudoku *sudoku, char *arq);

void celulas_vazias(Sudoku *sudoku, Celula **vazias, int *qtd_vazias);

void conflitos(Sudoku *sudoku, Conflito **conflitos, int *qtd_conflitos);

void mostra_sugestoes(Celula *c, int qtd, Sudoku *sudoku);

#endif
