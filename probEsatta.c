#include <stdio.h> 
#include <stdlib.h>  
#include <time.h>   
#include <math.h>
#include <string.h>
#include <stdint.h>   // contiene il tipo: uint32_t (Unsigned Integer a 32 bit)
#include <unistd.h>   // Serve per la funzione getpid() su Linux
#include <omp.h>      // header per OpenMP

#define Amax 1000
#define Bmax 1000

int max(int a, int b) {   // funzione che da massimo tra due valori
    if (a > b) {
        return a;
    } 
    else {
        return b;
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


// calcolo delle probabilita' di transizione
// scontri a 1 confronto di dadi:
static double p01_11 = 5.0/12.0;
static double p10_11 = 7.0/12.0;

static double p01_21 = 0;
static double p10_21;

static double p01_12 = 0;
static double p10_12;

static double p01_31 = 0;
static double p10_31;

static double p01_13 = 0;
static double p10_13;

// scontri a 2 confronti di dadi
static double p20_22 = 0;
static double p11_22 = 0;
static double p02_22;

static double p02_32 = 0;
static double p11_32 = 0;
static double p20_32;

static double p02_23 = 0;
static double p11_23 = 0;
static double p20_23;

// scontro a 3 confronti di dadi
static double p03_33 = 0;
static double p12_33 = 0;
static double p21_33 = 0;
static double p30_33;


void probabilita_transizione(){

    // probabilita' scontro 2vs1 (calcolo con casi favorevoli/totali)
    for (int a1 = 1; a1 <= 6; a1++){
        for (int a2 = 1; a2 <= 6; a2++){
            for (int b1 = 1; b1 <= 6; b1++){
                if (max(a1,a2) > b1){
                    p01_21++;
                }
            }
        }
    }
    p01_21 /= (6*6*6);
    p10_21 = 1 - p01_21;

    // probabilita' scontro 1vs2
    for (int a1 = 1; a1 <= 6; a1++){
        for (int b1 = 1; b1 <= 6; b1++){
            for (int b2 = 1; b2 <= 6; b2++){
                if (a1 > max(b1,b2)){
                    p01_12++;
                }
            }
        }
    }
    p01_12 /= (6*6*6);
    p10_12 = 1 - p01_12;

    // probabilita' scontri 3vs1
    for (int a1 = 1; a1 <= 6; a1++){
        for (int a2 = 1; a2 <= 6; a2++){
            for (int a3 = 1; a3 <= 6; a3++){
                for (int b1 = 1; b1 <= 6; b1++){
                    int c = max(a1,a2);
                    if(max(c,a3) > b1){
                        p01_31++;
                    }
                }
            }
        }
    }
    p01_31 /= (6*6*6*6);
    p10_31 = 1 - p01_31;

    // probabilita' scontro 1vs3
    for (int a1 = 1; a1 <= 6; a1++){
        for (int b1 = 1; b1 <= 6; b1++){
            for (int b2 = 1; b2 <= 6; b2++){
                for (int b3 = 1; b3 <= 6; b3++){
                    int c = max(b1,b2);
                    if(a1 > max(c,b3)){
                        p01_13++;
                    }
                }
            }
        }
    }
    p01_13 /= (6*6*6*6);
    p10_13 = 1 - p01_13;

    // probabilita' scontro 2vs2
    int dadiA[3], dadiB[3], massimiA[3], massimiB[3];
    for (int a1 = 1; a1 <= 6; a1++){
        for (int a2 = 1; a2 <= 6; a2++){
            for (int b1 = 1; b1 <= 6; b1++){
                for (int b2 = 1; b2 <= 6; b2++){
                    dadiA[0] = a1; 
                    dadiA[1] = a2;
                    dadiB[0] = b1; 
                    dadiB[1] = b2;
                    N_massimi(2, dadiA, 2, massimiA);
                    N_massimi(2, dadiB, 2, massimiB); 
                    if((massimiA[0] > massimiB[0]) && (massimiA[1] > massimiB[1])){
                        p02_22++;
                    }
                    else if((massimiA[0] > massimiB[0]) || (massimiA[1] > massimiB[1])){
                        p11_22++;
                    }
                }
            }
        }
    }
    p02_22 /= (6*6*6*6);
    p11_22 /= (6*6*6*6);
    p20_22 = 1 - p02_22 - p11_22;

    // probabilita' scontro 3vs2
    for (int a1 = 1; a1 <= 6; a1++){
        for (int a2 = 1; a2 <= 6; a2++){
            for (int a3 = 1; a3 <= 6; a3++){
                for (int b1 = 1; b1 <= 6; b1++){
                    for (int b2 = 1; b2 <= 6; b2++){
                        dadiA[0] = a1; 
                        dadiA[1] = a2;
                        dadiA[2] = a3;
                        dadiB[0] = b1; 
                        dadiB[1] = b2;
                        N_massimi(2, dadiA, 3, massimiA);
                        N_massimi(2, dadiB, 2, massimiB);
                        if((massimiA[0] > massimiB[0]) && (massimiA[1] > massimiB[1])){
                            p02_32++;
                        }
                        else if((massimiA[0] > massimiB[0]) || (massimiA[1] > massimiB[1])){
                            p11_32++;
                        }
                    }
                }
            }
        }
    }
    p02_32 /= (6*6*6*6*6);
    p11_32 /= (6*6*6*6*6);
    p20_32 = 1 - p02_32 - p11_32;

    // probabilita' scontro 2vs3
    for (int a1 = 1; a1 <= 6; a1++){
        for (int a2 = 1; a2 <= 6; a2++){
            for (int b1 = 1; b1 <= 6; b1++){
                for (int b2 = 1; b2 <= 6; b2++){
                    for (int b3 = 1; b3 <= 6; b3++){
                        dadiA[0] = a1; 
                        dadiA[1] = a2;
                        dadiB[0] = b1; 
                        dadiB[1] = b2; 
                        dadiB[2] = b3;
                        N_massimi(2, dadiA, 2, massimiA);
                        N_massimi(2, dadiB, 3, massimiB);
                        if((massimiA[0] > massimiB[0]) && (massimiA[1] > massimiB[1])){
                            p02_23++;
                        }
                        else if((massimiA[0] > massimiB[0]) || (massimiA[1] > massimiB[1])){
                            p11_23++;
                        }
                    }
                }
            }
        }
    }
    p02_23 /= (6*6*6*6*6);
    p11_23 /= (6*6*6*6*6);
    p20_23 = 1 - p02_23 - p11_23;

    // probabilita' scontro 3vs3
    for (int a1 = 1; a1 <= 6; a1++){
        for (int a2 = 1; a2 <= 6; a2++){
            for (int a3 = 1; a3 <= 6; a3++){
                for (int b1 = 1; b1 <= 6; b1++){
                    for (int b2 = 1; b2 <= 6; b2++){
                        for (int b3 = 1; b3 <= 6; b3++){
                            dadiA[0] = a1; 
                            dadiA[1] = a2;
                            dadiA[2] = a3;
                            dadiB[0] = b1; 
                            dadiB[1] = b2;
                            dadiB[2] = b3;
                            N_massimi(3, dadiA, 3, massimiA);
                            N_massimi(3, dadiB, 3, massimiB);
                            int c=0;
                            for (int i =0; i < 3; i++){
                                if (massimiA[i] > massimiB[i]){
                                    c++;
                                }
                            }
                            if(c==3){
                                p03_33++;
                            }
                            else if(c==2){
                                p12_33++;
                            }
                            else if(c==1){
                                p21_33++;
                            }
                        }     
                    }
                }
            }
        }
    }
    p03_33 /= (6*6*6*6*6*6);
    p12_33 /= (6*6*6*6*6*6);
    p21_33 /= (6*6*6*6*6*6);
    p30_33 = 1 - p03_33 - p12_33 - p21_33;

    // output delle probabilità di transizione
    printf(
        "\n=== PROBABILITA' DI TRANSIZIONE ===\n"
        "1vs1 | p10: %.4f | p01: %.4f\n"
        "2vs1 | p10: %.4f | p01: %.4f\n"
        "1vs2 | p10: %.4f | p01: %.4f\n"
        "3vs1 | p10: %.4f | p01: %.4f\n"
        "1vs3 | p10: %.4f | p01: %.4f\n"
        "-----------------------------------\n"
        "2vs2 | p20: %.4f | p11: %.4f | p02: %.4f\n"
        "3vs2 | p20: %.4f | p11: %.4f | p02: %.4f\n"
        "2vs3 | p20: %.4f | p11: %.4f | p02: %.4f\n"
        "-----------------------------------\n"
        "3vs3 | p30: %.4f | p21: %.4f | p12: %.4f | p03: %.4f\n",
        p10_11, p01_11,
        p10_21, p01_21,
        p10_12, p01_12,
        p10_31, p01_31,
        p10_13, p01_13,
        p20_22, p11_22, p02_22,
        p20_32, p11_32, p02_32,
        p20_23, p11_23, p02_23,
        p30_33, p21_33, p12_33, p03_33
    );

}

// calcolo esatto della probabilita' e del numero medio di scontri con metodo ricorsivo
static double prob[Amax+1][Bmax+1];
static double Ns[Amax+1][Bmax+1];


void curve_probabilita(){
    double Ea = p30_33*3 + p21_33*2 + p12_33*1;
    double Eb = p21_33*1 + p12_33*2 + p03_33*3;

    double m_50;
    m_50 = Ea/Eb;

    double var_X = (3 - m_50*0)*(3 - m_50*0)*p30_33 + (2 - m_50*1)*(2 - m_50*1)*p21_33 
    + (1 -  m_50*2)*(1 - m_50*2)*p12_33 + (0 - m_50*3)*(0 - m_50*3)*p03_33;
    double sig_eff = sqrt(var_X/Eb);

    printf(
        "\n=== ARMATE MEDIE PERSE IN REGIME ASINTOTICO ===\n"
        "E[a] = %.4f\n"
        "E[b] = %.4f\n"
        "\n=== PENDENZA ASINTOTICA DELLA CURVA DI PROBABILITA' 50per100 ===\n"
        "m_50 = %.4f\n"
        "\n=== VARIANZA DELLE ARMATE PERSE A SFAVORE DELL'ATTACCO NEL SINGOLO SCONTRO ===\n"
        "Var[a - m_50 * b] = %.4f\n"
        "sig_eff = sig[a - m_50 * b] / sqrt(E[b]) = %.4f\n",
        Ea, Eb,
        m_50,
        var_X, sig_eff
    );
}

void stati_assorbenti(){
    for (int a=2; a<=Amax; a++){
        prob[a][0] = 1;   // stati vittoria A
        Ns[a][0] = 0;     // stati di fine scontro
    }
    for (int b=1; b<=Bmax; b++){
        prob[1][b] = 0;   // stati sconfitta A
        Ns[1][b] = 0;     // stati di fine scontro
    }
}

void probabilita_ricorsiva(int a, int b){
    if((a >= 4) && (b >= 3)){   // confronto a 3 dadi
        prob[a][b] = p03_33*prob[a][b-3] + p12_33*prob[a-1][b-2] + p21_33*prob[a-2][b-1] + p30_33*prob[a-3][b];
        Ns[a][b] = 1 + p03_33*Ns[a][b-3] + p12_33*Ns[a-1][b-2] + p21_33*Ns[a-2][b-1] + p30_33*Ns[a-3][b];
    }
    else if ((a >= 3) && (b >= 2)){   // confronti a 2 dadi
        if (a >= 4){
            prob[a][b] = p02_32*prob[a][b-2] + p11_32*prob[a-1][b-1] + p20_32*prob[a-2][b];
            Ns[a][b] = 1 + p02_32*Ns[a][b-2] + p11_32*Ns[a-1][b-1] + p20_32*Ns[a-2][b];
        }
        else if (b >= 3){
            prob[a][b] = p02_23*prob[a][b-2] + p11_23*prob[a-1][b-1] + p20_23*prob[a-2][b];
            Ns[a][b] = 1 + p02_23*Ns[a][b-2] + p11_23*Ns[a-1][b-1] + p20_23*Ns[a-2][b];
        }
        else{
            prob[a][b] = p02_22*prob[a][b-2] + p11_22*prob[a-1][b-1] + p20_22*prob[a-2][b];
            Ns[a][b] = 1 + p02_22*Ns[a][b-2] + p11_22*Ns[a-1][b-1] + p20_22*Ns[a-2][b];
        }
    }
    else{   // confronti a 1 dado
        if(a>=4){
            prob[a][b] = p01_31*prob[a][b-1] + p10_31*prob[a-1][b];
            Ns[a][b] = 1 + p01_31*Ns[a][b-1] + p10_31*Ns[a-1][b];
        }
        else if (b>=3){
            prob[a][b] = p01_13*prob[a][b-1] + p10_13*prob[a-1][b];
            Ns[a][b] = 1 + p01_13*Ns[a][b-1] + p10_13*Ns[a-1][b];
        }
        else if (a==3){
            prob[a][b] = p01_21*prob[a][b-1] + p10_21*prob[a-1][b];
            Ns[a][b] = 1 + p01_21*Ns[a][b-1] + p10_21*Ns[a-1][b];
        }
        else if (b==2){
            prob[a][b] = p01_12*prob[a][b-1] + p10_12*prob[a-1][b];
            Ns[a][b] = 1 + p01_12*Ns[a][b-1] + p10_12*Ns[a-1][b];
        }
        else{
            prob[a][b] = p01_11*prob[a][b-1] + p10_11*prob[a-1][b];
            Ns[a][b] = 1 + p01_11*Ns[a][b-1] + p10_11*Ns[a-1][b];
        }
    }
}



int main(){

    probabilita_transizione();

    curve_probabilita();

    stati_assorbenti();

    for (int a=2; a<=Amax; a++){
        for (int b=1; b<=Bmax; b++){
            probabilita_ricorsiva(a,b);
        }
    }

    FILE *file = fopen("probEsatta.dat", "w");
    if (file == NULL) {
        printf("\nErrore nell'apertura del file\n");
        return 1;
    }
    
    fprintf(file, "%d %d\n", Amax, Bmax);  // prima riga del file con valori #define

    for (int a = 2; a <= Amax; a++) {
        for (int b = 1; b <= Bmax; b++) {
            fprintf(file, "%d %d %lf %lf\n", a, b, prob[a][b], Ns[a][b]);
        }
    }
    fclose(file);
}