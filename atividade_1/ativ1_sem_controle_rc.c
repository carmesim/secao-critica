#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

// Variáveis compartilhadas - Manna-Pnueli
int SOMA = 0;
int respond = 0;
int request = 0;


void cliente(int i);
void servidor();

int main (int argc, char** argv){

    int i, n_threads = 3;
    struct timeval start, end;
    srand(time(NULL));

    // recebe número de threads por linha de comando
    if(argc > 1 && atoi(argv[1]) >= 3){
        n_threads = atoi(argv[1]);
    }
    printf("Manna-Pnueli com %d Threads\n", n_threads);

    // inicia contagem do tempo
    gettimeofday(&start, NULL);

    // laço com a criação das threads
#pragma omp parallel \
    default(none) \
    shared (n_threads) \
    private(i) \
    num_threads(n_threads)
{
#pragma omp for
    for (i = 0; i < n_threads; i++){
        if(i == 0){
            servidor();
        }else{
            cliente(i);
        }
    }
}

    // finaliza contagem do tempo
    gettimeofday(&end, NULL);
    double time_taken_sec;
    time_taken_sec = (end.tv_sec - start.tv_sec) * 1e6;
    time_taken_sec = (time_taken_sec + (end.tv_usec - start.tv_usec)) * 1e-6;
    printf("SOMA = %d\nTempo: %f segundos\n", SOMA, time_taken_sec);

    return 0;
}

void servidor(){
    //Servidor
    printf("Servidor Executando\n");
    fflush(stdout);
    while (1){
        while(request == 0); // await request != 0
        respond = request;
        while(respond != 0); // await request == 0 
        request = 0;
    }
}

void cliente(int i){
    // Cliente i
    printf("Cliente %d Executando\n", i);
    fflush(stdout);
    while (1){
        //seção não crítica
        // OBS: Pré-Protocolo desativado -> Não há controle da RC
        /*while(respond != i){
            request = i;
        }*/
        //seção crítica
        int local = SOMA;
        sleep(rand()%2);
        SOMA = local+1;
        printf("Cliente %d diz: SOMA = %d\n", i, SOMA);
        fflush(stdout);
        //fim da seção crítica
        respond = 0;
    }
}
