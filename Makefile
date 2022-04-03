all:
	gcc sudoku.c -c -Wall
	gcc main.c -c -Wall
	gcc *.o -o sudoku
run:
	make all
	./sudoku sud1.txt
