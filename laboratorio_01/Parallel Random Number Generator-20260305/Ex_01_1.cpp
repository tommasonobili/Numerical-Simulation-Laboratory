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

double error(double media[], double media2[], int n){
   if (n==0){
      return 0.0;
   } else {
      return sqrt((media2[n] - pow(media[n],2))/n);
   }
}

void progressive_mean_and_variance_function(Random &rnd, double progressive_mean[], double progressive_error[],double progressive_variance[], double progressive_variance_error[], int N_blocks, int throws_per_block){

   //creo i dati(medie) random "scorrelati"
   double data[N_blocks]={0.0};
   double data_2[N_blocks]={0.0};
   double progressive_mean_2[N_blocks]={0.0};

   //creo i dati(varianze)"
   double variance[N_blocks]={0.0};
   double variance_2[N_blocks]={0.0};
   double progressive_variance_2[N_blocks]={0.0};

   for(int i=0;i<N_blocks;i++){
      
      double sum = 0.0;
      double block_throws[throws_per_block] = {0.0};   // salvo i lanci del blocco per poter calcolare dopo la varianza rispetto alla media del blocco

      double sum_var = 0.0;
 
      for(int j=0;j<throws_per_block;j++){
         block_throws[j] = rnd.Rannyu();
         sum += block_throws[j];
      }

      data[i] = sum/throws_per_block;   // media del blocco i-esimo
      data_2[i]= data[i]*data[i];

      for(int j=0;j<throws_per_block;j++){
         sum_var+=pow((block_throws[j]-data[i]),2);   // somma degli scarti quadratici dalla media del blocco
      }

      variance[i]=sum_var/(throws_per_block-1);  //tolto grado di libertà dovuto al calcolo della media
      variance_2[i]=pow(variance[i],2);
   }

   //faccio progressive mean dei miei dati(medie) all'aumentare di essi (con relativo errore)
   for(int i=0;i<N_blocks;i++){

      double sum=0;
      double sum_2=0;

      double sum_var=0.0;
      double sum_var_2=0.0;

      for(int j=0;j<i+1;j++){
         sum+=data[j];
         sum_2+=data_2[j];

         sum_var+=variance[j];
         sum_var_2+=variance_2[j];
      }

      progressive_mean[i]=sum/(i+1);
      progressive_mean_2[i]=sum_2/(i+1);

      progressive_error[i]=error(progressive_mean,progressive_mean_2,i);

      progressive_variance[i]=sum_var/(i+1);   // media progressiva delle varianze di blocco
      progressive_variance_2[i]=sum_var_2/(i+1);
      
      progressive_variance_error[i]=error(progressive_variance,progressive_variance_2,i);

   }
}

//100 test separati di chi^2 (100 esperimenti diversi)
void riprovo_chi_2_function(Random &rnd, double chi_2[], int N_bins, int n_throws, int N_tests){

   double expected = double(n_throws)/N_bins;

   for(int i=0; i<N_tests; i++){
      double count[N_bins] = {0.0};
      double sum = 0.0;

      for(int j=0; j<n_throws; j++){
         double x = rnd.Rannyu();
         int bin = int(x*N_bins);

         if(bin == N_bins){ //se per caso esce proprio 1 (quasi impossibile)
            bin = N_bins-1;}

         count[bin] += 1.0;
      }

      for(int k=0; k<N_bins; k++){
         sum += pow(count[k]-expected,2);
      }

      chi_2[i]=sum/expected;
   }
}

int main(){

   Random rnd;
   initialize_random(rnd);
   
   int N_throws = 10000;
   int N_blocks = 100;
   int throws_per_block = N_throws/N_blocks;   // numero di lanci per ciascun blocco

   double progressive_mean[N_blocks] = {0.0};
   double progressive_error[N_blocks] = {0.0};

   double progressive_variance[N_blocks] = {0.0};
   double progressive_variance_error[N_blocks] = {0.0};

   progressive_mean_and_variance_function(rnd, progressive_mean,progressive_error, progressive_variance,progressive_variance_error,N_blocks,throws_per_block);

   int N_bins = 100;
   int n_throws = 10000;
   int N_tests = 100;

   double chi_2[N_tests]={0.0};

   riprovo_chi_2_function(rnd,chi_2,N_bins,n_throws, N_tests);


   ofstream output("../Results/Esercizio_01.1_results.txt");

   output << "Progressive_mean" << ";"
          << "Progressive_error" << ";" 
          << "Progressive_variance" << ";"
          << "Progressive_variance_error" << endl;

   for (int i = 0; i < N_blocks; i++) {
      output << progressive_mean[i] << ";"
             << progressive_error[i] << ";"
             << progressive_variance[i] << ";"
             << progressive_variance_error[i] << endl;
   }

   ofstream output_chi2("../Results/Esercizio_01.1_chi2_results.txt");

   output_chi2 << "Chi_2" << endl;

   for(int i = 0; i < N_tests; i++){
      output_chi2 << chi_2[i] << endl;
   }

   output_chi2.close();


   output.close();

   rnd.SaveSeed();

   return 0;
}