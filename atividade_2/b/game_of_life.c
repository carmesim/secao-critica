#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 2048
#define itera_max 2000
#define cores 8

int grid [N][N];
int new_grid[N][N];

void inicia_grids_zero(){
	int i, j;
	//iniciando com zero
#pragma omp parallel for collapse(2)
	for (i = 0; i < N; i++){
		for (j = 0; j < N; j++){
			grid[i][j] = 0;
			new_grid[i][j] = 0;
		}
	}
}

void geracao_inicial(){
	//GLIDER
	int lin = 1, col = 1;
	grid[lin  ][col+1] = 1;
	grid[lin+1][col+2] = 1;
	grid[lin+2][col  ] = 1;
	grid[lin+2][col+1] = 1;
	grid[lin+2][col+2] = 1;
	 
	//R-pentomino
	lin =10; col = 30;
	grid[lin  ][col+1] = 1;
	grid[lin  ][col+2] = 1;
	grid[lin+1][col  ] = 1;
	grid[lin+1][col+1] = 1;
	grid[lin+2][col+1] = 1;
	
}

int getNeighbors(int table[N][N], int i, int j){
	int numAliveNeighbors = 0;

	// Up
	if(i != 0){
		if(table[i - 1][j] == 1){
			numAliveNeighbors++;
		}
	}else{
		if(table[N - 1][j] == 1){
			numAliveNeighbors++;
		}
	}

	// Down
	if(table[(i + 1)%N][j] == 1){
		numAliveNeighbors++;
	}

	// Left
	if(j != 0){
		if(table[i][j - 1] == 1){
			numAliveNeighbors++;
		}
	}else{
		if(table[i][N - 1] == 1){
			numAliveNeighbors++;
		}
	}

	// Right
	if(table[i][(j + 1)%N] == 1){
		numAliveNeighbors++;
	}

	// Upper-Right Corner
	if((i == 0) && (j == N - 1)){
		if(table[N - 1][0] == 1){
			numAliveNeighbors++;
		}
	}else{
		// i!=0 || j != n-1
		if(i == 0){
			// já sabemos que j != N - 1
			if(table[N - 1][j + 1] == 1){
				numAliveNeighbors++;
			}
		}else{// i != 0
			if(j == N - 1){
				if(table[i - 1][0] == 1){
					numAliveNeighbors++;
				}
			}else{
				if(table[i - 1][j + 1] == 1){
					numAliveNeighbors++;
				}
			}
		}
	}

	// Lower-Right Corner
	if(table[(i + 1)%N][(j + 1)%N] == 1){
		numAliveNeighbors++;
	}

	// Upper-Left Corner
	if((i == 0) && (j == 0)){
		 if(table[N - 1][N - 1] == 1){
			numAliveNeighbors++;
		}
	}else{
		// i!=0 || j != 0
		if(i == 0){
			// já sabemos que j != 0
			if(table[N - 1][j -1] == 1){
				numAliveNeighbors++;
			}
		}else{// i != 0
			if(j == 0){
				if(table[i - 1][N - 1] == 1){
					numAliveNeighbors++;
				}
			}else{
				if(table[i - 1][j - 1] == 1){
					numAliveNeighbors++;
				}
			}
		}
	}


	// Lower-Left Corner
	if((i == N - 1) && (j == 0)){
		 if(table[0][N - 1] == 1){
			numAliveNeighbors++;
		}
	}else{
		// i!=n-1 || j != 0
		if(i == N - 1){
			// já sabemos que j != 0
			if(table[0][j - 1] == 1){
				numAliveNeighbors++;
			}
		}else{// i != n-1
			if(j == 0){
				if(table[i + 1][N - 1] == 1){
					numAliveNeighbors++;
				}
			}else{
				if(table[i + 1][j - 1] == 1){
					numAliveNeighbors++;
				}
			}
		}
	}

	return numAliveNeighbors;
}


void game_of_life(){

	int i;
	int j;

#pragma omp parallel for collapse(2)
	for (i = 0; i < N; i++){
		for (j = 0; j < N; j++){
			//aplicar as regras do jogo da vida

			//celulas vivas com menos de 2 vizinhas vivas morrem
			if(grid[i][j] == 1 && getNeighbors(grid, i, j) < 2){
				new_grid[i][j] = 0;
			}

			//célula viva com 2 ou 3 vizinhos deve permanecer viva para a próxima geração
			else if (grid[i][j] == 1 && getNeighbors(grid, i, j) == 2 || getNeighbors(grid, i, j) == 3){
				new_grid[i][j] = 1;
			}

			//célula viva com 4 ou mais vizinhos morre por superpopulação
			else if (grid[i][j] == 1 && getNeighbors(grid, i, j) >= 4){
				new_grid[i][j] = 0;
			}

			//morta com exatamente 3 vizinhos deve se tornar viva
			else if (grid[i][j] == 0 && getNeighbors(grid, i, j) == 3){
				new_grid[i][j] = 1;
			}
		}
	}

	//passar a nova geração para atual
#pragma omp parallel for collapse(2)
	for (i = 0; i < N; i++){
		for (j = 0; j < N; j++){
			grid[i][j] = new_grid[i][j];
		}
	}
}


int count_LiveCells(){
	int i;
	int j;
	int cont = 0;

#pragma omp parallel for collapse (2) reduction(+ : cont)
	
		for (i = 0; i < N; i++){
			for (j = 0; j < N; j++){
				if (grid[i][j] == 1){
					cont++;
				}
			}
		}
	
	return cont;
}

int main (){

	int i, j;
	int var;
	int vida;
	int cont = 0;
	double start;
	double end;

	omp_set_num_threads(cores);

	inicia_grids_zero();

	geracao_inicial();

	
	for (vida = 0; vida < itera_max; vida++){
		/*
		for (i = 0; i < N; i++){
			for (j = 0; j < N; j++){

				if (grid[i][j] == 1){
					printf("\033[1;31m");
					printf("%d", grid[i][j]);
					printf("\033[0m");
				}
				else{
					printf("%d", grid[i][j]);
				}
			}
			printf("\n");
		}*/
		//printf("VIVOS: %d\n", count_LiveCells());
		game_of_life();
		//getchar(); //para fazer o for esperar por um enter
	}


	start = omp_get_wtime ();

	cont = count_LiveCells ();

	end = omp_get_wtime();


	printf("VIVOS: %d\n", cont);
	printf("CORES: %d\n", cores);
	printf("TEMPO: %f\n", end - start);
	


/*
	for (i = 0; i < N; i++){
		for (j = 0; j < N; j++){
			printf("%d", grid[i][j]);
		}
		printf("\n");
	}
*/





	return 0;
}
