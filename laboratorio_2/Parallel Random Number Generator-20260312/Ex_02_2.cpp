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

double error_matrix(double media[][101], double media2[][101], int i, int k){
   if (i == 0){
      return 0.0;
   } else {
      return sqrt((media2[i][k] - pow(media[i][k], 2)) / i);
   }
}

void discrete_step(Random &rnd, double &x, double &y, double &z){

   double p=rnd.Rannyu();
   if(p<(1./3.)){
      double l=rnd.Rannyu();
      if(l<1./2.){
         x+=1;
      } 
      else{
         x-=1;
      }
   }
   else if(p<(2./3.)){
      double l=rnd.Rannyu();
      if(l<1./2.){
         y+=1;
      } 
      else{
         y-=1;
      }
   }
   else{
      double l=rnd.Rannyu();
      if(l<1./2.){
         z+=1;
      } 
      else{
         z-=1;
      }
   }

   return ;
}

void continuous_step(Random &rnd, double &x, double &y, double &z){
//cambiare magari
   double phi=rnd.Rannyu(0,2*M_PI);
   double costheta=rnd.Rannyu(-1,1);
   double sintheta=sqrt(1-costheta*costheta);

   x+=sintheta*cos(phi);
   y+=sintheta*sin(phi);
   z+=costheta;

   return ;
}

void discrete_rw_function(Random &rnd, int N_blocks,int throws_per_block, int n_steps, double progressive_discrete_rw[][101], double progressive_discrete_rw_error[][101]){
   
   double discrete_rw[N_blocks][101]={0.0}; //valori di sqrt(<r^2>) per ogni blocco e passo (dati)
   double discrete_rw_2[N_blocks][101]={0.0};
   double progressive_discrete_rw_2[N_blocks][101]={0.0};

   for (int i = 0; i < N_blocks; i++) {
      for (int k = 0; k <= n_steps; k++) {
         double sum_r2 = 0.0;

         for (int j = 0; j < throws_per_block; j++) {
            double x = 0.0;
            double y = 0.0;
            double z = 0.0;

            for (int s = 0; s < k; s++) {
               discrete_step(rnd, x, y, z);
            }

            double r2 = x*x + y*y + z*z;
            sum_r2 += r2;
         }

         double mean_r2 = sum_r2 / throws_per_block;

         discrete_rw[i][k] = sqrt(mean_r2);
         discrete_rw_2[i][k] = discrete_rw[i][k] * discrete_rw[i][k];
      }
   }

   for (int k = 0; k <= n_steps; k++) {
      for (int i = 0; i < N_blocks; i++) {
         double sum = 0.0;
         double sum_2 = 0.0;

         for (int j = 0; j < i + 1; j++) {
            sum += discrete_rw[j][k];
            sum_2 += discrete_rw_2[j][k];
         }

         progressive_discrete_rw[i][k] = sum / (i + 1);
         progressive_discrete_rw_2[i][k] = sum_2 / (i + 1);

         progressive_discrete_rw_error[i][k] = error_matrix(progressive_discrete_rw, progressive_discrete_rw_2, i, k);
      }
   }
}

void continuos_rw_function(Random &rnd, int N_blocks,int throws_per_block, int n_steps, double progressive_continuous_rw[][101], double progressive_continuous_rw_error[][101]){
   
   double continuous_rw[N_blocks][101]={0.0}; //valori di sqrt(<r^2>) per ogni blocco e passo (dati)
   double continuous_rw_2[N_blocks][101]={0.0};
   double progressive_continuous_rw_2[N_blocks][101]={0.0};

   for (int i = 0; i < N_blocks; i++) {
      for (int k = 0; k <= n_steps; k++) {
         double sum_r2 = 0.0;

         for (int j = 0; j < throws_per_block; j++) {
            double x = 0.0;
            double y = 0.0;
            double z = 0.0;

            for (int s = 0; s < k; s++) {
               continuous_step(rnd, x, y, z);
            }

            double r2 = x*x + y*y + z*z;
            sum_r2 += r2;
         }

         double mean_r2 = sum_r2 / throws_per_block;

         continuous_rw[i][k] = sqrt(mean_r2);
         continuous_rw_2[i][k] = continuous_rw[i][k] * continuous_rw[i][k];
      }
   }

   for (int k = 0; k <= n_steps; k++) {
      for (int i = 0; i < N_blocks; i++) {
         double sum = 0.0;
         double sum_2 = 0.0;

         for (int j = 0; j < i + 1; j++) {
            sum += continuous_rw[j][k];
            sum_2 += continuous_rw_2[j][k];
         }

         progressive_continuous_rw[i][k] = sum / (i + 1);
         progressive_continuous_rw_2[i][k] = sum_2 / (i + 1);

         progressive_continuous_rw_error[i][k] = error_matrix(progressive_continuous_rw, progressive_continuous_rw_2, i, k);
      }
   }
}

int main(){

   Random rnd;
   initialize_random(rnd);

   int N_throws = 10000;    
   int N_blocks = 100;
   int throws_per_block = N_throws / N_blocks;
   int n_steps = 100; 

   double progressive_discrete_rw[N_blocks][101]={0.0};
   double progressive_discrete_rw_error[N_blocks][101]={0.0};

   discrete_rw_function(rnd, N_blocks, throws_per_block, n_steps, progressive_discrete_rw, progressive_discrete_rw_error);

   double progressive_continuous_rw[N_blocks][101]={0.0};
   double progressive_continuous_rw_error[N_blocks][101]={0.0};

   continuos_rw_function(rnd, N_blocks, throws_per_block, n_steps, progressive_continuous_rw, progressive_continuous_rw_error);

   //stampa
   ofstream output("../Results/Esercizio_02.2_results.txt");

   output << "Block" << ";"
          << "Step" << ";"
          << "Progressive_discrete_rw" << ";"
          << "Progressive_discrete_rw_error" << ";"
          << "Progressive_continuous_rw" << ";"
          << "Progressive_continuous_rw_error" << endl;

   for(int i = 0; i < N_blocks; i++){
      for(int k = 0; k <= n_steps; k++){
         output << i << ";"
                << k << ";"
                << progressive_discrete_rw[i][k] << ";"
                << progressive_discrete_rw_error[i][k] << ";"
                << progressive_continuous_rw[i][k] << ";"
                << progressive_continuous_rw_error[i][k] << endl;
      }
   }

   output.close();
   
   rnd.SaveSeed();

   return 0;
}