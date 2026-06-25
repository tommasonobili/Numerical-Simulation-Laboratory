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
            rnd.SetRandom(seed, p1, p2);
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

double integranda(double x){
   return (M_PI/2)*cos((M_PI/2)*x);
}

void progressive_montecarlo_uniform_sampling(Random &rnd, int N_blocks,int throws_per_block, double progressive_integral_uniform_sampling[],
    double progressive_integral_uniform_sampling_error[]){

      double integral_uniform_sampling[N_blocks]={0.0};
      double integral_uniform_sampling_2[N_blocks]={0.0};
      double progressive_integral_uniform_sampling_2[N_blocks]={0.0};

      for(int i=0;i<N_blocks;i++){
         double sum=0;
         for(int j=0;j<throws_per_block;j++){
            sum+=integranda(rnd.Rannyu());         //unica cosa particolare di questo codice
         }
         integral_uniform_sampling[i]=sum/throws_per_block;
         integral_uniform_sampling_2[i]=pow(integral_uniform_sampling[i],2);
      }

      for(int i=0;i<N_blocks;i++){
         double sum=0;
         double sum_2=0;

         for(int j=0; j<i+1;j++){
            sum+=integral_uniform_sampling[j];
            sum_2+=integral_uniform_sampling_2[j];
         }
         progressive_integral_uniform_sampling[i]=sum/(i+1);
         progressive_integral_uniform_sampling_2[i]=sum_2/(i+1);         
         
         progressive_integral_uniform_sampling_error[i]=error(progressive_integral_uniform_sampling,progressive_integral_uniform_sampling_2,i);
      }

   }

void progressive_montecarlo_importance_sampling(Random &rnd, int N_blocks,int throws_per_block, double progressive_integral_importance_sampling[],
    double progressive_integral_importance_sampling_error[]){

      double integral_importance_sampling[N_blocks]={0.0};
      double integral_importance_sampling_2[N_blocks]={0.0};
      double progressive_integral_importance_sampling_2[N_blocks]={0.0};

      for(int i=0;i<N_blocks;i++){
         double sum=0;
         for(int j=0;j<throws_per_block;j++){
            double x=rnd.cos_importance_sampling();      //unica cosa particolare di questo codice
            sum+=(integranda(x)/(2.0*(1.0-x)));
         }
         integral_importance_sampling[i]=sum/throws_per_block;
         integral_importance_sampling_2[i]=pow(integral_importance_sampling[i],2);
      }

      for(int i=0;i<N_blocks;i++){
         double sum=0;
         double sum_2=0;

         for(int j=0; j<i+1;j++){
            sum+=integral_importance_sampling[j];
            sum_2+=integral_importance_sampling_2[j];
         }
         progressive_integral_importance_sampling[i]=sum/(i+1);
         progressive_integral_importance_sampling_2[i]=sum_2/(i+1);         
         
         progressive_integral_importance_sampling_error[i]=error(progressive_integral_importance_sampling,progressive_integral_importance_sampling_2,i);
      }

   }


int main(){

   Random rnd;
   initialize_random(rnd);

   int N_throws = 10000;    
   int N_blocks = 100;
   int throws_per_block = N_throws / N_blocks;


   double progressive_integral_uniform_sampling[N_blocks]={0.0};
   double progressive_integral_uniform_sampling_error[N_blocks]={0.0};

   progressive_montecarlo_uniform_sampling(rnd, N_blocks, throws_per_block, progressive_integral_uniform_sampling, progressive_integral_uniform_sampling_error);

   double progressive_integral_importance_sampling[N_blocks]={0.0};
   double progressive_integral_importance_sampling_error[N_blocks]={0.0};

   progressive_montecarlo_importance_sampling(rnd, N_blocks, throws_per_block, progressive_integral_importance_sampling, progressive_integral_importance_sampling_error);

   //stampa
   ofstream output("../Results/Esercizio_02.1_results.txt");

   output << "Progressive_integral_uniform_sampling" << ";"
          << "Progressive_integral_uniform_sampling_error" << ";"
          << "Progressive_integral_importance_sampling" << ";"
          << "Progressive_integral_importance_sampling_error" << endl;

 
   for(int i=0; i<N_blocks; i++){
      output << progressive_integral_uniform_sampling[i] << ";"
             << progressive_integral_uniform_sampling_error[i] << ";"
             << progressive_integral_importance_sampling[i] << ";"
             << progressive_integral_importance_sampling_error[i] << endl;
   }

   output.close();
   
   rnd.SaveSeed();

   return 0;
}