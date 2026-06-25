#include <iostream>
#include <fstream>
#include <string>
#include "random.h"
#include <cmath>
#include <algorithm>

using namespace std;

double mu=1.0;
double sigma=0.5;

void initialize_random(Random &rnd) {
    int seed[4];
    int p1, p2;

    ifstream Primes("Primes");
    if (Primes.is_open()) {
        Primes >> p1 >> p2;
    } else {
        cerr << "PROBLEM: Unable to open Primes" << endl;
    }
    Primes.close();

    ifstream input("seed.in");
    string property;

    if (input.is_open()) {
        while (input >> property) {
            if (property == "RANDOMSEED") {
                input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
                rnd.SetRandom(seed, p1, p2);
            }
        }
        input.close();
    } else {
        cerr << "PROBLEM: Unable to open seed.in" << endl;
    }
}

double error(double media[], double media2[], int n){
   if (n==0){
      return 0.0;
   } else {
      return sqrt((media2[n] - pow(media[n],2))/n);
   }
}

// Calcola il potenziale valutato in X
double potential(double x) {
    return pow(x, 4) - 2.5 * x * x;
}

// Calcola l'argomento dell'integrale che sta accanto a p(x). Somma termine cinetico e potenziale.  (H|PHI>)/PHI>)
double Energy(double x, double mu, double sigma) {

    double s2 = sigma * sigma;
    double s4 = s2 * s2;

    double g1 = exp(-pow(x - mu, 2) / (2.0 * s2));
    double g2 = exp(-pow(x + mu, 2) / (2.0 * s2));

    double psi_x = g1 + g2;

    // Calcolo di d2Psi / dx2
    double psi_second = g1 * (pow(x - mu, 2) / s4 - 1.0 / s2)+ g2 * (pow(x + mu, 2) / s4 - 1.0 / s2);

    double kinetic = -0.5 * psi_second / psi_x;

    return kinetic + potential(x);
}

//modulo quadro funzione d'onda trial
double probability_distribution_squared_modulus(double x, double mu, double sigma){

    double modulus_2 = pow(
    exp(-pow(x-mu,2)/(2*pow(sigma,2))) +
    exp(-pow(x+mu,2)/(2*pow(sigma,2))),
    2
);
    return modulus_2;
}

//Accettazione
bool Acceptance(Random &rnd, double x, double x_new, int &accepted_per_block){
    double A=min(1.0,probability_distribution_squared_modulus(x_new, mu, sigma)/probability_distribution_squared_modulus(x, mu, sigma));      //T, ovvero lo step, è simmetrico. Si elide nella frazione

    double random = rnd.Rannyu();

    if(random<A){
        accepted_per_block++;
        return true;
    }
    else{
        return false;
    }

}

//metropolis 
double metropolis(Random &rnd, double &x,int &accepted_per_block){
    //mi deve ritornare H estratto dalla distribuzione di probabilità "trial probability distribution function"
    
    double x_new=x+rnd.Rannyu(-1.2,1.2);

    if(Acceptance(rnd, x,x_new, accepted_per_block)){ //x_new è estratto secondo distrib di probabilità definita dal modulo della funzione d'onda

        x=x_new;

        return Energy(x, mu, sigma);
    }

    else{

        return Energy(x, mu, sigma);

    }
    
}

//genero H da distribuzione di probabilità
void H_mean_blocks(Random &rnd, int N_blocks, int throws_per_block, double progressive_H[], double progressive_H_error[]){

    double H[N_blocks];
    double H_2[N_blocks];

    double progressive_H_2[N_blocks];

    double x=0.1;


    // //apro file per salvare posizioni per tracciare traiettoria
    // ofstream trajectory("../trajectories/trajectory_uniform_100.dat");
    // trajectory << "x y z\n";

    int accepted_total = 0;

    //equilibriamento iniziale
    for(int i=0; i<1000; i++){
        int dummy = 0;
        metropolis(rnd, x, dummy);
    }

    //valori per ogni blocco
    for(int i=0; i<N_blocks;i++){
        double sum=0;

        int accepted_per_block=0;

        for(int j=0;j<throws_per_block;j++){
            sum+=metropolis(rnd,x,accepted_per_block);
            // if(j % 1000 == 0){
            //     trajectory << x << " " << y << " " << z << "\n";
            // }
        }

        H[i]=sum/throws_per_block;
        H_2[i]=H[i]*H[i];

        //cout << "Acceptance rate: " << double(accepted_per_block)/throws_per_block << endl;
        accepted_total+=accepted_per_block;
    }

    // trajectory.close();

    cout << "Mean  acceptance rate: " << double(accepted_total)/(N_blocks*throws_per_block) << endl;

    //media progressiva
    for(int i=0;i<N_blocks;i++){
        double sum=0;
        double sum_2=0;

        for(int j=0;j<i+1;j++){
            sum+=H[j];
            sum_2+=H_2[j];
        }

        progressive_H[i]=sum/(i+1);
        progressive_H_2[i]=sum_2/(i+1);

        progressive_H_error[i]=error(progressive_H,progressive_H_2,i);

    }
}





int main() {
    Random rnd;
    initialize_random(rnd);

   int N_throws = 10000;
   int N_blocks = 100;
   int throws_per_block = N_throws/N_blocks;   // numero di lanci per ciascun blocco

//-----------------------------------------------FUNDAMENTAL UNIFORM-------------------------------------------------------------------

    double progressive_H[N_blocks]={0.0};
    double progressive_H_error[N_blocks]={0.0};

    H_mean_blocks(rnd, N_blocks, throws_per_block, progressive_H, progressive_H_error);

//-----------------------------------------------OUTPUT------------------------------------------------------------------------------

    ofstream output("../Esercizio_08.1_results.txt");

    output << "Progressive_ground_state_Energy" << ";"
            << "Progressive_ground_state_Energy_error" << endl;



    for (int i = 0; i < N_blocks; i++) {
        output  << progressive_H[i] << ";"
                << progressive_H_error[i] << endl;
    }


    output.close();

    rnd.SaveSeed();
    return 0;
}