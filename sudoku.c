
#include<stdio.h>
#include<stdlib.h>
#include "sudoku.h"

// Struct para definir uma posição no tabuleiro
struct celula {
	int col;
	int lin;
};

// Imprime uma celula no terminal, no formato (linha, coluna)
void print_celula(Celula *c) {
	printf("(%d,%d)", c->lin + 1, c->col + 1);
}

int celula_igual(Celula *c1, Celula *c2) {
	return c1->lin == c2->lin && c1->col == c2->col;
}

// Struct para definir conflitos entre duas celulas
struct conflito {
	int tipo; // 0: linha; 1: coluna; 2: região
	int n; // numero da linha/coluna/região, de 0 a 8
	Celula c1;
	Celula c2;
};

// Imprime um conflito no terminal
void print_conflito(Conflito *c) {
	switch (c->tipo) {
		case 0: printf("Linha "); break;
		case 1: printf("Coluna "); break;
		case 2: printf("Regiao "); break;
	}
	printf("%d:  ", c->n + 1);
	print_celula(&c->c1);
	printf(" e ");
	print_celula(&c->c2);
}

void mostra_conflitos(Conflito *c, int qtd) {
	if (qtd == 0) return;

	printf("Alguma coisa deu errado...  Invalidos:\n");
	for (int i = 0; i < qtd; i++) {
		print_conflito(c + i);
		printf("\n");
	}
}

int conflito_igual(Conflito *c1, Conflito *c2) {
	return c1->tipo == c2->tipo && c1->n == c2->n && (
		(celula_igual(&c1->c1, &c2->c1) && celula_igual(&c1->c2, &c2->c2)) ||
		(celula_igual(&c1->c1, &c2->c2) && celula_igual(&c1->c2, &c2->c1)));
}

// função de comparação entre dois conflitos, usada no qsort
int conflito_comp(const void *a, const void *b) {
	Conflito *c1 = (Conflito *)a;
	Conflito *c2 = (Conflito *)b;
	return c1->tipo == c2->tipo ? c1->n - c2->n : c1->tipo - c2->tipo;
}

// Struct para definir um tabuleiro de sudoku
struct sudoku {
	int tabuleiro[9][9];
};

// Aloca um novo tabuleiro de sudoku
Sudoku *novo_sudoku() {
	return malloc(sizeof(Sudoku));
}

// Libera espaço alocado para um tabuleiro de sudoku
void free_sudoku(Sudoku **sudoku) {
	free(*sudoku);
	*sudoku = NULL;
}

// Lê os numeros do tabuleiro de sudoku de um arquivo
// retorna 1 caso haja falha na leitura do arquivo
// retorna 0 se não ouve falhas
int ler_sudoku(Sudoku *sudoku, char *arq_nome) {
	FILE *arq = fopen(arq_nome, "r");

	// se arq for NULL, quer dizer que ouve falha na leitura do arquivo, e portanto retornamos o codigo de erro 1
	if (arq == NULL) return 1;

	for (int lin = 0; lin < 9; lin++) {
		for (int col = 0; col < 9; col++) {
			fscanf(arq, "%d", &sudoku->tabuleiro[col][lin]);
		}
	}

	fclose(arq);

	return 0;
}

// Encontra todas as celulas vazias do tabuleiro e as retorna por referencia pelo vetor "vazias"
// o vetor é alocado dentro da função, e portanto deve ser desalocado por quem chamou a função
// "qtd_vazias" é uma referencia a um int usado para denominar o tamanho do vetor
void celulas_vazias(Sudoku *sudoku, Celula **vazias, int *qtd_vazias) {
	// primeiro contamos quantas celulas vazias existem no tabuleiro, para saber qual tamanho alocar para o vetor de celulas
	*qtd_vazias = 0;
	for (int lin = 0; lin < 9; lin++) {
		for (int col = 0; col < 9; col++) {
			if (sudoku->tabuleiro[col][lin] == 0) (*qtd_vazias)++;
		}
	}

	*vazias = malloc(*qtd_vazias * sizeof(Celula));

	// populamos o vetor com as celulas vazias
	int i = 0;
	for (int lin = 0; lin < 9; lin++) {
		for (int col = 0; col < 9; col++) {
			if (sudoku->tabuleiro[col][lin] == 0) {
				(*vazias)[i++] = (Celula){ col, lin };
			}
		}
	}
}

// encontra todos os conflitos em uma linha com uma celula
void conflitos_linha(Sudoku *sudoku, int lin, Celula *c, Conflito **conflitos, int *qtd_conflitos) {
	*qtd_conflitos = 0;
	for (int col = 0; col < 9; col++) {
		if (col == c->col) continue;
		if (sudoku->tabuleiro[col][lin] == sudoku->tabuleiro[c->col][lin]) (*qtd_conflitos)++;
	}

	*conflitos = malloc(*qtd_conflitos * sizeof(Conflito));

	int i = 0;
	for (int col = 0; col < 9; col++) {
		if (col == c->col) continue;
		if (sudoku->tabuleiro[col][lin] == sudoku->tabuleiro[c->col][lin]) {
			(*conflitos)[i++] = (Conflito) { 0, lin, *c, (Celula) { col, lin } };
		}
	}
}

// encontra todos os conflitos em uma coluna com uma celula
void conflitos_coluna(Sudoku *sudoku, int col, Celula *c, Conflito **conflitos, int *qtd_conflitos) {
	*qtd_conflitos = 0;
	for (int lin = 0; lin < 9; lin++) {
		if (lin == c->lin) continue;
		if (sudoku->tabuleiro[col][lin] == sudoku->tabuleiro[col][c->lin]) (*qtd_conflitos)++;
	}

	*conflitos = malloc(*qtd_conflitos * sizeof(Conflito));

	int i = 0;
	for (int lin = 0; lin < 9; lin++) {
		if (lin == c->lin) continue;
		if (sudoku->tabuleiro[col][lin] == sudoku->tabuleiro[col][c->lin]) {
			(*conflitos)[i++] = (Conflito) { 1, col, *c, (Celula) { col, lin } };
		}
	}
}

// encontra todos os conflitos em uma região com uma celula
void conflitos_regiao(Sudoku *sudoku, int lin_c, int col_c, Celula *c, Conflito **conflitos, int *qtd_conflitos) {

	*qtd_conflitos = 0;
	for (int lin = lin_c; lin < lin_c + 3; lin++) {
		for (int col = col_c; col < col_c + 3; col++) {
			if (lin == c->lin && col == c->col) continue;
			if (sudoku->tabuleiro[col][lin] == sudoku->tabuleiro[c->col][c->lin]) (*qtd_conflitos)++;
		}
	}

	*conflitos = malloc(*qtd_conflitos * sizeof(Conflito));

	int i = 0;
	for (int lin = lin_c; lin < lin_c + 3; lin++) {
		for (int col = col_c; col < col_c + 3; col++) {
			if (lin == c->lin && col == c->col) continue;
			if (sudoku->tabuleiro[col][lin] == sudoku->tabuleiro[c->col][c->lin]) {
				(*conflitos)[i++] = (Conflito) { 2, lin_c / 3 * 3 + col_c / 3, *c, (Celula) { col, lin } };
			}
		}
	}
}

// encontra todos os conflitos com uma celula
void conflitos_celula(Sudoku *sudoku, Celula *c, Conflito **conflitos, int *qtd_conflitos) {
	Conflito *conflitos_lin;
	int qtd_conf_lin;
	conflitos_linha(sudoku, c->lin, c, &conflitos_lin, &qtd_conf_lin);

	Conflito *conflitos_col;
	int qtd_conf_col;
	conflitos_coluna(sudoku, c->col, c, &conflitos_col, &qtd_conf_col);

	Conflito *conflitos_reg;
	int qtd_conf_reg;
	conflitos_regiao(sudoku, c->lin / 3 * 3, c->col / 3 * 3, c, &conflitos_reg, &qtd_conf_reg);

	(*qtd_conflitos) = qtd_conf_lin + qtd_conf_col + qtd_conf_reg;

	(*conflitos) = malloc(*qtd_conflitos * sizeof(Conflito));

	int i = 0;
	for (int j = 0; j < qtd_conf_lin; j++) (*conflitos)[i++] = conflitos_lin[j];
	for (int j = 0; j < qtd_conf_col; j++) (*conflitos)[i++] = conflitos_col[j];
	for (int j = 0; j < qtd_conf_reg; j++) (*conflitos)[i++] = conflitos_reg[j];
}

// remove conflitos duplicados do vetor de conflitos
void remove_duplicados(Conflito **conflitos, int *qtd_conflitos) {
	Conflito *conf = malloc(*qtd_conflitos * sizeof(Conflito));
	int i = 0;
	for (int j = 0; j < *qtd_conflitos; j++) {
		int repetido = 0;
		for (int k = 0; k < i; k++) {
			if (conflito_igual((*conflitos) + j, conf + k)) {
				repetido = 1;
				break;
			}
		}

		if (!repetido) conf[i++] = (*conflitos)[j];
	}

	*conflitos = realloc(*conflitos, i * sizeof(Conflito));
	*conflitos = conf;
	*qtd_conflitos = i;
}

// encontra todos os conflitos no tabuleiro
void conflitos(Sudoku *sudoku, Conflito **conflitos, int *qtd_conflitos) {
	Conflito *conf[9][9];
	int qtd_conf[9][9];

	for (int lin = 0; lin < 9; lin++) {
		for (int col = 0; col < 9; col++) {
			qtd_conf[col][lin] = 0;
			if (sudoku->tabuleiro[col][lin] == 0) continue;
			Celula c = (Celula){ col, lin };
			conflitos_celula(sudoku, &c, &conf[col][lin], &qtd_conf[col][lin]);
		}
	}

	*qtd_conflitos = 0;
	for (int lin = 0; lin < 9; lin++) {
		for (int col = 0; col < 9; col++) {
			(*qtd_conflitos) += qtd_conf[col][lin];
		}
	}

	*conflitos = malloc(*qtd_conflitos * sizeof(Conflito));

	int i = 0;
	for (int lin = 0; lin < 9; lin++) {
		for (int col = 0; col < 9; col++) {
			for (int j = 0; j < qtd_conf[col][lin]; j++) {
				(*conflitos)[i++] = conf[col][lin][j];
			}
		}
	}

	remove_duplicados(conflitos, qtd_conflitos);

	qsort(*conflitos, *qtd_conflitos, sizeof(Conflito), conflito_comp);
}

// encontra os possiveis valores para uma celula
void mostra_sugestao_celula(Celula *c, Sudoku *sudoku) {
	const int lin_c = c->lin / 3 * 3;
	const int col_c = c->col / 3 * 3;

	for (int i = 1; i <= 9; i++) {
		int ver = 0;

		// verifica a linha
		for (int j = 0; j < 9; j++)
			if (sudoku->tabuleiro[j][c->lin] == i) { ver = 1; break; } // se já houver aquele valor na linha

		// verifica a coluna
		for (int j = 0; (j < 9) && !ver; j++)
			if (sudoku->tabuleiro[c->col][j] == i) { ver = 1; break; } // se já houver aquele valor na coluna

		// verifica o quadrante
		for (int j = col_c; (j < col_c + 3) && !ver; j++)
			for (int k = lin_c; k < lin_c + 3; k++)
				if (sudoku->tabuleiro[j][k] == i) { ver = 1; break; } // se já houver aquele valor no quadrante

		if (!ver) printf("%d ", i); // verifica se a sugestões é valida e a imprime
	}
}

// imprime as sugestões para as celulas vazias
void mostra_sugestoes(Celula *c, int qtd, Sudoku *sudoku) {
	if (!qtd) return;

	printf("Voce esta no caminho certo. Sugestoes:\n");
	for (int i = 0; i < qtd; i++) {
		printf("(%d,%d):  ", c[i].lin + 1, c[i].col + 1);
		mostra_sugestao_celula(c + i, sudoku); // imprime as sugestões para a celula vazia
		printf("\n");
	}
}