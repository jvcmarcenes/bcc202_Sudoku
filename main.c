/*
 * Artur Bermond Torres (21.1.4003)
 * João Vitor Costa Marcenes Vieira (21.1.4016)
 * Pedro Augusto Sousa Gonçalves (21.1.4015)
*/
#include<stdio.h>
#include<stdlib.h>
#include "sudoku.h"

int main(int argc, char *argv[]) {
    //Informa a formatação correta da execução no terminal, caso venha a estar errada
	if (argc != 2) {
		printf("Uso: %s <arquivo_tabuleiro>\n", argv[0]);
		return 0;
	}

    //Lê o arquivo de sudoku e nos diz caso tenha tido um erro na leitura
    int res;
	Sudoku *sudoku = novo_sudoku();
	res = ler_sudoku(sudoku, argv[1]);
	if (res == 1) {
		printf("Falha ao tentar ler arquivo %s\n", argv[1]);
		free_sudoku(&sudoku);
		return 0;
	}

    //Verifica se há conflitos (se o tabuleiro está incorreto)
	Conflito *conf;
	int qtd_conflitos;
	conflitos(sudoku, &conf, &qtd_conflitos);
	if (qtd_conflitos > 0) {
		mostra_conflitos(conf, qtd_conflitos);
	} else {
        //Verifica se há células vazias (se o tabuleiro está incompleto)
		Celula *vazias;
		int qtd_vazias;
		celulas_vazias(sudoku, &vazias, &qtd_vazias);
		if (qtd_vazias > 0) {
			mostra_sugestoes(vazias, qtd_vazias, sudoku);
		} else {
            //Caso os dois casos não forem verdade, o jogo está completo e correto.
			printf("Jogo completo. Voce ganhou!\n");
		}

		free(vazias);
	}

	free(conf);
	free_sudoku(&sudoku);

	return 0;
}