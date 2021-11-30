#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define N 2048
#define itera_max 2000
#define cores 8

int grid [N][N];
int new_grid[N][N];

int cont = 0;


typedef struct {
	// How many rows in this matrix
	size_t data_rows;
	// How many columns in this matrix
	size_t data_cols;
} matrix_info_t;


typedef struct {
	matrix_info_t matrix_info;
	size_t row_to_count;
} thread_args_t;



void* counter_thread(void * args){
	uint64_t *ret = malloc(1*sizeof(uint64_t));
	thread_args_t info = *(thread_args_t *) args;

	uint32_t row = info.row_to_count;

	// How many columns in this matrix
	size_t max_col = info.matrix_info.data_cols;

	uint64_t sum = 0;
	for(uint64_t col = 0; col < max_col; col++) {
		sum += grid[row][col];
	}


	*ret = sum;
	pthread_exit(ret);
}

int async_count(matrix_info_t info, size_t row, uint8_t threads_to_spawn) {
	pthread_t * thread_ids = malloc(threads_to_spawn * sizeof(pthread_t));
	thread_args_t * args = malloc(threads_to_spawn * sizeof(thread_args_t));
	uint64_t sum = 0;

	for(uint8_t i = 0; i < threads_to_spawn; i++) {
		args[i].matrix_info = info;
		args[i].row_to_count = row + i;
		pthread_create(&thread_ids[i], NULL, counter_thread, (void *) &args[i]);
	}

	for(uint8_t i = 0; i < threads_to_spawn; i++) {
		int * retval;
		pthread_join(thread_ids[i], (void*) &retval);
		sum += *retval;
	}

	free(thread_ids);
	free(args);

	return sum;
}



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

#pragma omp parallel for collapse(2)
	//passar a nova geração para atual
	for (i = 0; i < N; i++){
		for (j = 0; j < N; j++){
			grid[i][j] = new_grid[i][j];
		}
	}
}


int count_LiveCells(){

	int i = 0;
	uint64_t sum = 0;
	
		for(i = 0; i < N; i += cores) {
		uint8_t threads_to_spawn;
		matrix_info_t info;
		info.data_cols = N;
		info.data_rows = N;

		if(i + cores > N) {
			threads_to_spawn = N - i;
		} else {
			threads_to_spawn = cores;
		}

		sum += async_count(info, i, threads_to_spawn);
	}

	return sum;
}



int main (int argc, char **argv){

	int i, j;
	int vida;
	int cont;
	
	time_t start, end;

	inicia_grids_zero();

	geracao_inicial();

	//start = omp_get_wtime ();
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

        struct timespec tstart={0,0}, tend={0,0};
        clock_gettime(CLOCK_MONOTONIC, &tstart);
	cont = count_LiveCells();	

        clock_gettime(CLOCK_MONOTONIC, &tend);

	printf("VIVOS: %d\n", cont);
	printf("CORES: %d\n", cores);
        printf("TEMPO: %.5f segundos\n",
           ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) -
           ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));

	return 0;
}
