
#include<stdio.h>
#include<stdlib.h>
#include "sudoku.h"

/*

1 - alocar tabuleiro - J
2 - encontrar celulas vazias - J
3 - encontrar celulas invalidas - J
4 - se tiver celulas invalidas, imprimir celulas invalidas - A 
5 - se não tiver celulas vazias, imprimir que o jogo esta completo - P
6 - senao, encontrar possiveis valores pras celulas vazias - P

*/

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Uso: %s <arquivo_tabuleiro>\n", argv[0]);
		return 0;
	}

	int res;

	Sudoku *sudoku = novo_sudoku();
	res = ler_sudoku(sudoku, argv[1]);

	if (res == 1) {
		printf("Falha ao tentar ler arquivo %s\n", argv[1]);
		return 0;
	}

	Conflito *conf;
	int qtd_conflitos;
	conflitos(sudoku, &conf, &qtd_conflitos);

	if (qtd_conflitos > 0) {
		mostra_conflitos(conf, qtd_conflitos);
	} else {
		Celula *vazias;
		int qtd_vazias;
		celulas_vazias(sudoku, &vazias, &qtd_vazias);

		if (qtd_vazias > 0) {
			mostra_sugestoes(vazias, qtd_vazias, sudoku);
		} else {
			printf("Jogo completo. Voce ganhou!\n");
		}

		free(vazias);
	}

	free(conf);
	free_sudoku(&sudoku);

	return 0;
}