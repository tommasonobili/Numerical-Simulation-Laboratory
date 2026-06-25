#include <iostream>
#include <fstream>
#include <string>
#include "random.h"
#include <cmath>
#include <algorithm>

using namespace std;

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

double GBM(double t, double S0, double mu, double sigma, double w) {
    return S0 * exp((mu - 0.5 * sigma * sigma) * t + sigma * w);
}

void monte_carlo_direct_function(Random &rnd, double progressive_direct_call[], double progressive_direct_call_error[], double progressive_direct_put[],
    double progressive_direct_put_error[],int N_blocks, int throws_per_block, double T, double K, double S_0, double r, double sigma){

    double direct_call[N_blocks];
    double direct_call_2[N_blocks];
    double direct_put[N_blocks];
    double direct_put_2[N_blocks];

    double progressive_direct_call_2[N_blocks];
    double progressive_direct_put_2[N_blocks];

    for(int i=0; i< N_blocks;i++){

        double sum_call=0.0;
        double sum_put=0.0;

        for(int j=0;j<throws_per_block;j++){

            double w = rnd.Gauss(0,sqrt(T));
            
            double ST = GBM(T,S_0,r,sigma,w);

            double payoff_call = exp(-r * T) * max(0.0, ST - K);
            sum_call+=payoff_call;

            double payoff_put = exp(-r * T) * max(0.0, K - ST);
            sum_put+=payoff_put;
        }

        direct_call[i]=(sum_call/throws_per_block);
        direct_call_2[i]=direct_call[i]*direct_call[i];

        direct_put[i]=(sum_put/throws_per_block);
        direct_put_2[i]=direct_put[i]*direct_put[i];
    }

    for(int i=0;i<N_blocks;i++){

        double sum_call=0;
        double sum_call_2=0;
        double sum_put=0.0;
        double sum_put_2=0.0;

        for(int j=0;j<i+1;j++){
            sum_call+=direct_call[j];
            sum_call_2+=direct_call_2[j];

            sum_put+=direct_put[j];
            sum_put_2+=direct_put_2[j];
        }

        progressive_direct_call[i]=sum_call/(i+1);
        progressive_direct_call_2[i]=sum_call_2/(i+1);
        progressive_direct_call_error[i]=error(progressive_direct_call,progressive_direct_call_2,i);

        progressive_direct_put[i]=sum_put/(i+1);
        progressive_direct_put_2[i]=sum_put_2/(i+1);
        progressive_direct_put_error[i]=error(progressive_direct_put,progressive_direct_put_2,i);

    }
}

void monte_carlo_discrete_function(Random &rnd, double progressive_discrete_call[], double progressive_discrete_call_error[], double progressive_discrete_put[],
    double progressive_discrete_put_error[],int N_blocks, int throws_per_block, int n_steps, double T, double K, double S_0, double r, double sigma){

    double dt=T/n_steps;
    
    double discrete_call[N_blocks];
    double discrete_call_2[N_blocks];
    double discrete_put[N_blocks];
    double discrete_put_2[N_blocks];

    double progressive_discrete_call_2[N_blocks];
    double progressive_discrete_put_2[N_blocks];   

    for(int i=0; i< N_blocks;i++){

        double sum_call=0.0;
        double sum_put=0.0;

        for(int j=0;j<throws_per_block;j++){

            //discrete propagation up to time T  (iterative method)
            double ST = S_0;
            for (int t = 0; t < n_steps; t++) {
                double Z = rnd.Gauss(0.0, 1.0);
                ST = GBM(dt, ST, r, sigma, Z * sqrt(dt));
            }

            double payoff_call = exp(-r * T) * max(0.0, ST - K);
            sum_call+=payoff_call;

            double payoff_put = exp(-r * T) * max(0.0, K - ST);
            sum_put+=payoff_put;
        }

        discrete_call[i]=(sum_call/throws_per_block);
        discrete_call_2[i]=discrete_call[i]*discrete_call[i];

        discrete_put[i]=(sum_put/throws_per_block);
        discrete_put_2[i]=discrete_put[i]*discrete_put[i];
    }

    for(int i=0;i<N_blocks;i++){

        double sum_call=0;
        double sum_call_2=0;
        double sum_put=0.0;
        double sum_put_2=0.0;

        for(int j=0;j<i+1;j++){
            sum_call+=discrete_call[j];
            sum_call_2+=discrete_call_2[j];

            sum_put+=discrete_put[j];
            sum_put_2+=discrete_put_2[j];
        }

        progressive_discrete_call[i]=sum_call/(i+1);
        progressive_discrete_call_2[i]=sum_call_2/(i+1);
        progressive_discrete_call_error[i]=error(progressive_discrete_call,progressive_discrete_call_2,i);

        progressive_discrete_put[i]=sum_put/(i+1);
        progressive_discrete_put_2[i]=sum_put_2/(i+1);
        progressive_discrete_put_error[i]=error(progressive_discrete_put,progressive_discrete_put_2,i);

    }        
    }


int main() {
    Random rnd;
    initialize_random(rnd);

   int N_throws = 10000;
   int N_blocks = 100;
   int throws_per_block = N_throws/N_blocks;   // numero di lanci per ciascun blocco

    double T = 1.0;
    double S_0 = 100.0;
    double K = 100.0;
    double r = 0.1;
    double sigma = 0.25;

    double progressive_direct_call[N_blocks]={0.0};
    double progressive_direct_call_error[N_blocks]={0.0};
    double progressive_direct_put[N_blocks]={0.0};
    double progressive_direct_put_error[N_blocks]={0.0};

    double progressive_discrete_call[N_blocks]={0.0};
    double progressive_discrete_call_error[N_blocks]={0.0};
    double progressive_discrete_put[N_blocks]={0.0};
    double progressive_discrete_put_error[N_blocks]={0.0};

    //direct method
    monte_carlo_direct_function(rnd, progressive_direct_call, progressive_direct_call_error, progressive_direct_put,
    progressive_direct_put_error,N_blocks, throws_per_block, T, K, S_0, r, sigma);

    //discrete method
    int n_steps = 100;

    monte_carlo_discrete_function(rnd, progressive_discrete_call, progressive_discrete_call_error, progressive_discrete_put,
    progressive_discrete_put_error,N_blocks, throws_per_block, n_steps, T, K, S_0, r, sigma);    


    ofstream output("../Results/Esercizio_03.1_results.txt");

    output << "Progressive_direct_call" << ";"
            << "Progressive_direct_call_error" << ";" 
            << "Progressive_direct_put" << ";"
            << "Progressive_direct_put_error" << ";"

            << "Progressive_discrete_call" << ";"
            << "Progressive_discrete_call_error" << ";"
            << "Progressive_discrete_put" << ";"
            << "Progressive_discrete_put_error" << endl;




    for (int i = 0; i < N_blocks; i++) {
        output  << progressive_direct_call[i] << ";"
                << progressive_direct_call_error[i] << ";"
                << progressive_direct_put[i] << ";"
                << progressive_direct_put_error[i] << ";"

                << progressive_discrete_call[i] << ";"
                << progressive_discrete_call_error[i] << ";"
                << progressive_discrete_put[i] << ";"
                << progressive_discrete_put_error[i] << endl;
    }


   output.close();

    rnd.SaveSeed();
    return 0;
}