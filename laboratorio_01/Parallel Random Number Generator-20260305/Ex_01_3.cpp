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

// esperimento Buffon per stimare pi
void buffon_pi_function(Random &rnd, double progressive_pi[], double progressive_pi_error[],int N_blocks, int throws_per_block, double L, double d){

   double pi[N_blocks];
   double pi_2[N_blocks]={0.0};
   double progressive_pi_2[N_blocks]={0.0};

   for(int i=0;i<N_blocks;i++){

      double N_hit=0.0;

      for(int j=0;j<throws_per_block;j++){

         double x=rnd.Rannyu(0.0, d/2.0);

         //double theta=rnd.Rannyu(0.0, M_PI/2.0); così uso PI per stimare PI -> provo a generare sin(theta) senza usare pi:
         // estraggo un punto uniformemente nella circonferenza unitaria del primo quadrante e uso la coordinata y per ottenere sin(theta) (perché sin(theta) = y/r)
         double x_circ, y_circ, r;
         for(;;){
            x_circ = rnd.Rannyu();
            y_circ = rnd.Rannyu();
            r = sqrt(x_circ*x_circ + y_circ*y_circ);

            if(r <= 1.0 && r != 0.0) break;
            }

         double sin_theta = y_circ / r;


         if(x <= (L/2)*sin_theta){
            N_hit++;
         }
      }
      pi[i]=(2.0*L*throws_per_block)/(d*N_hit);
      pi_2[i]=pi[i]*pi[i];
   }

   //faccio progressive mean dei miei dati(pi) all'aumentare di essi (con relativo errore)
   for(int i=0;i<N_blocks;i++){

      double sum_pi=0;
      double sum_pi_2=0;

      for(int j=0;j<i+1;j++){
         sum_pi+=pi[j];
         sum_pi_2+=pi_2[j];
      }

      progressive_pi[i]=sum_pi/(i+1);
      progressive_pi_2[i]=sum_pi_2/(i+1);

      progressive_pi_error[i]=error(progressive_pi,progressive_pi_2,i);
   }
}

int main(){

    Random rnd;
    initialize_random(rnd);

    // parametri del problema
    double L = 1;    // lunghezza ago
    double d = 1.2;    // distanza tra le linee, con d > L

    if(d <= L){
        cout << "Errore: la distanza tra le linee deve essere maggiore della lunghezza dell'ago." << endl;
        return 1;
    }

    int N_throws = 10000;    
    int N_blocks = 100;
    int throws_per_block = N_throws / N_blocks;

    double progressive_pi[N_blocks] = {0.0};
    double progressive_pi_error[N_blocks] = {0.0};

    buffon_pi_function(rnd, progressive_pi, progressive_pi_error,N_blocks, throws_per_block, L, d);


    ofstream output("../Results/Esercizio_01.3_results.txt");

    output << "Progressive_pi" << ";"
           << "Progressive_pi_error" << endl;

    for(int i=0; i<N_blocks; i++){
        output << progressive_pi[i] << ";"
               << progressive_pi_error[i] << endl;
    }

    output.close();

    rnd.SaveSeed();

    return 0;
}