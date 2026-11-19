#include <stdio.h> 
#include <stdlib.h>  
#include <time.h>   
#include <math.h>
#include <string.h>
#include <stdint.h>   // contiene il tipo: uint32_t (Unsigned Integer a 32 bit)
#include <unistd.h>   // Serve per la funzione getpid() su Linux
#include <omp.h>      // header per OpenMP

#define N_battaglie 10000000   // Numero di simulazioni MC
#define Amax 100
#define Bmax 100

// algoritmo di George Marsaglia
static inline uint32_t xorshift32(uint32_t *state) {
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}


struct ARMATE {
    int armate_A;
    int armate_B;
};


int min(int a, int b) {   // funzione che da minimo tra due valori
    if (a < b) {
        return a;
    } 
    else {
        return b;
    }
}

int max(int a, int b) {   // funzione che da massimo tra due valori
    if (a > b) {
        return a;
    } 
    else {
        return b;
    }
}

void esito_N_lanci(int N, int *esiti, uint32_t *rng_state) {

    for (int i=0; i<N; i++) {
        esiti[i] = (int)((xorshift32(rng_state) % 6) + 1);
    }
}


void N_massimi(int N, int *array, int dim_array, int *massimi) {   // trova gli N valori maggiori di un array (ordinati in modo decrescente)

    memset(massimi, 0, N * sizeof(int)); // elementi allocati di massimi messi = 0

    for (int i = 0; i < dim_array; i++) {

        for (int j = 0; j < N; j++) {

            if( array[i] >= massimi[j] ) {   //ho tovato un nuovo j-esimo massimo

                for (int k = N-1; k > j; k--) {
                    massimi[k] = massimi[k-1];  // scalo i massimi minori
                }
                massimi[j] = array[i];  // inserisco nuovo massimo
                break;   // esco da ciclo su j
            }
        }
    }
}


void scontro(struct ARMATE *armate, int *esiti_A, int *esiti_B, int *massimi_A, int *massimi_B, uint32_t *rng_state) {
    // esito di uno scontro (A attaccante, B difensore)

    int n_tiri_A = min( (*armate).armate_A -1, 3 );
    int n_tiri_B = min( (*armate).armate_B, 3 );

    esito_N_lanci(n_tiri_A, esiti_A, rng_state);
    esito_N_lanci(n_tiri_B, esiti_B, rng_state);

    int n_confronti = min(n_tiri_A, n_tiri_B);

    N_massimi( n_confronti, esiti_A, n_tiri_A, massimi_A);
    N_massimi( n_confronti, esiti_B, n_tiri_B, massimi_B);

    for (int i=0; i<n_confronti; i++) {

        if ( massimi_A[i] > massimi_B[i] ) {  // confronto dado per dado
            (*armate).armate_B--;  // distrutta un'armata di B
        }
        else {
            (*armate).armate_A--;  // distrutta un'armata di A
        }
    }
}


void simulazioni(int A0, int B0, int *n_scontri, uint32_t *rng_state, double *output_vittorie, double *output_media, double *output_var) {

    int vittorie_A = 0;
    double n_scontri_media = 0;

    memset(n_scontri, 0, N_battaglie * sizeof(int));

    struct ARMATE armate;
    int esiti_A[3], esiti_B[3];
    int massimi_A[3], massimi_B[3];


    for (int sim=0; sim<N_battaglie; sim++) {
        
        armate.armate_A = A0;
        armate.armate_B = B0;

        while ( ( armate.armate_A > 1 ) && ( armate.armate_B > 0) ) {
            scontro(&armate, esiti_A, esiti_B, massimi_A, massimi_B, rng_state);
            n_scontri[sim]++;
            n_scontri_media++;
        }

        if ( armate.armate_B == 0 ) {
            vittorie_A++;
        }
    }

    n_scontri_media = n_scontri_media/N_battaglie;

    // calcolo della varianza sul numero di scontri di una battaglia
    double var = 0;
    for (int sim=0; sim<N_battaglie; sim++) {
        var = var + ( (n_scontri_media-n_scontri[sim]) * (n_scontri_media-n_scontri[sim]) );
    }
    var = var/(N_battaglie-1);

    //output (salvati su argometi della funzione)
    *output_vittorie = (double)vittorie_A / N_battaglie;
    *output_media = n_scontri_media;
    *output_var = var;    // varianza del singolo valore n_scontri_i
}


// Matrici globali per salvare i risultati di ogni coppia (a, b)
static double matrix_vittorie[Amax+1][Bmax+1];
static double matrix_media[Amax+1][Bmax+1];
static double matrix_var[Amax+1][Bmax+1];


int main(int argc, char *argv[]) {

    double inizio = omp_get_wtime();  //salva tempo inizio run

    if (argc != 2) { // se non ci sono due parametri inseriti da terminale (il nome dell'eseguibile conta come primo parametro)
        printf("Errore: specifica il nome del file di output!\n");
        printf("Uso corretto: %s <nome_file.dat>\n", argv[0]);
        return 1;
    }
    // prende il nome del file dal secondo argomento
    char *nome_file = argv[1];

    FILE *file = fopen(nome_file, "w");
    if (file == NULL) {
        printf("\nErrore nell'apertura del file\n");
        return 1;
    }

    // inizializzazione del seme in modo che ogni run abbia un seed diverso
    // utilizzo time + PID (il codice identificativo assegnato a ogni processo)
    // operatore ^ in C rappresenta lo XOR bit a bit, mischia i due valori
    uint32_t base_seed = (uint32_t)(time(NULL) ^ getpid());


    // variabili per visualizzare la percentuale di completamento della run
    int totali = (Amax - 1) * Bmax; 
    int completati = 0;
    int passo_1_percento = totali/100;

    // parallelizzazione
    #pragma omp parallel
    {
        // Ogni thread alloca il proprio buffer n_scontri privato
        int *n_scontri = malloc(N_battaglie * sizeof(int));

        // Parallelizzazione dei due cicli annidati
        #pragma omp for collapse(2) schedule(dynamic)
        for (int a = 2; a <= Amax; a++) {
            for (int b = 1; b <= Bmax; b++) {
                
                // Generazione di un seed unico per la coppia (a, b)
                uint32_t rng_state = base_seed ^ ((a * Bmax + b) * 2654435761u);   // con costante di Knuth, u=unsigned

                // Esecuzione simulazione salvando i dati nella matrice in memoria
                simulazioni(a, b, n_scontri, &rng_state, &matrix_vittorie[a][b], &matrix_media[a][b], &matrix_var[a][b]);

                // Visualizzazione della percentuale di completamento della run
                int c=0;     // c e' variabile privata per ogni thread (completati e' globale)
                #pragma omp atomic capture
                c = ++completati;
                if (c % passo_1_percento == 0 || c == totali) {
                    #pragma omp critical   // un thread alla volta puo' entrare qui
                    {
                        printf("\rAvanzamento: %3d%%", (c * 100) / totali);
                        fflush(stdout);
                    }
                }
            }
        }
        // liberazione della memoria del singolo thread
        free(n_scontri);
    }

    // output su file a fine simulazione
    fprintf(file, "%d %d %d\n", N_battaglie, Amax, Bmax);  // prima riga del file con valori #define
    double std;
    for (int a = 2; a <= Amax; a++) {
        for (int b = 1; b <= Bmax; b++) {
            std = sqrt(matrix_vittorie[a][b] * (1-matrix_vittorie[a][b]) / N_battaglie);
            fprintf(file, "%d %d  %lf %lf %lf %lf\n", a, b, matrix_vittorie[a][b], std, matrix_media[a][b], sqrt(matrix_var[a][b] / N_battaglie));
            //    A    B    pA_mc    sig_pA_mc    n_scontri_media    sig_n_scontri_media
        }
    }
    fclose(file);

    // Calcolo del tempo reale trascorso
    double tempo_secondi = omp_get_wtime() - inizio;
    int ore = (int)tempo_secondi / 3600;
    int min = (int)(tempo_secondi - 3600 * ore) / 60;
    int sec = (int)tempo_secondi - 3600 * ore - 60 * min;
    printf("Durata della run: %d h ; %d m ; %d s\n", ore, min, sec);

    return 0;
}
