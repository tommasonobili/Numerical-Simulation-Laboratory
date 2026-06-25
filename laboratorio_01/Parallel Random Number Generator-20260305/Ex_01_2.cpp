#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "random.h"

using namespace std;

void initialize_random(Random &rnd){
   int seed[4];
   int p1, p2;

   ifstream Primes("Primes");
   if (Primes.is_open()){
      Primes >> p1 >> p2;
   } else cerr << "PROBLEM: Unable to open Primes" << endl;
   Primes.close();

   ifstream input("seed.in");
   string property;
   if (input.is_open()){
      while (!input.eof()){
         input >> property;
         if(property == "RANDOMSEED"){
            input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
            rnd.SetRandom(seed, p1, p2);   // inizializza il generatore con i semi letti da file
         }
      }
      input.close();
   } else cerr << "PROBLEM: Unable to open seed.in" << endl;
}

void generate_distribution_averages(Random &rnd, double sum_uniform[], double sum_exponential[],double sum_lorentzian[], int n_throws){
    int index = 0;
    int N[4];
    N[0]=1;
    N[1]=2;
    N[2]=10;
    N[3]=100;

    for(int k=0;k<4;k++){
        for(int j=0;j<n_throws;j++){
            double sum_un=0.0;
            double sum_exp=0.0;
            double sum_lor=0.0;
            for(int i=0;i<N[k];i++){
                sum_un+=rnd.Rannyu();
                sum_exp+=rnd.Exp(1.0);  // lambda = 1.0 for exponential distribution
                sum_lor+=rnd.Lorentz(0.0,1.0);  // mu = 0.0 and gamma = 1.0 for Lorentzian distribution
            }
            sum_uniform[index] = sum_un/N[k];
            sum_exponential[index] = sum_exp/N[k];
            sum_lorentzian[index] = sum_lor/N[k];
            index++;
        }
    }
}


int main(){

    Random rnd;
    initialize_random(rnd);
   
    int n_throws= 10000;

    double sum_uniform[4*n_throws];  
    double sum_exponential[4*n_throws];
    double sum_lorentzian[4*n_throws];
    generate_distribution_averages(rnd,sum_uniform,sum_exponential,sum_lorentzian,n_throws);

       

ofstream output("../Results/Esercizio_01.2_results.txt");

output  << "Sum_uniform_1" << ";"
        << "Sum_uniform_2" << ";"
        << "Sum_uniform_10" << ";"
        << "Sum_uniform_100" << ";"
        << "Sum_exponential_1" << ";"
        << "Sum_exponential_2" << ";"
        << "Sum_exponential_10" << ";"
        << "Sum_exponential_100" << ";"
        << "Sum_lorentzian_1" << ";"
        << "Sum_lorentzian_2" << ";"
        << "Sum_lorentzian_10" << ";"
        << "Sum_lorentzian_100" << endl;   


for (int i = 0; i < n_throws; i++) {
    output << sum_uniform[i] << ";"
           << sum_uniform[i + n_throws] << ";"
           << sum_uniform[i + 2*n_throws] << ";"
           << sum_uniform[i + 3*n_throws] << ";"
           << sum_exponential[i] << ";"
           << sum_exponential[i + n_throws] << ";"
           << sum_exponential[i + 2*n_throws] << ";"
           << sum_exponential[i + 3*n_throws] << ";"
           << sum_lorentzian[i] << ";"
           << sum_lorentzian[i + n_throws] << ";"
           << sum_lorentzian[i + 2*n_throws] << ";"
           << sum_lorentzian[i + 3*n_throws] << endl;
}


    output.close();
    rnd.SaveSeed();

    return 0;
}