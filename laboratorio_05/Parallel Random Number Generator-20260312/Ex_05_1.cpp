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


//-----------------------------------------------FUNDAMENTAL UNIFORM-------------------------------------------------------------------

//modulo quadro funzione d'onda stato 100
double fundamental_wave_function_squared_modulus(double x, double y, double z){
    double modulus_2=(1/M_PI)*exp(-2*sqrt(x*x+y*y+z*z));
    return modulus_2;
}

//Accettazione stato 100
bool A_fundamental(Random &rnd, double x, double y, double z, double x_new, double y_new, double z_new, int &accepted_per_block){
    double A=min(1.0,fundamental_wave_function_squared_modulus(x_new,y_new,z_new)/fundamental_wave_function_squared_modulus(x,y,z));      //T, ovvero lo step, è simmetrico. Si elide nella frazione

    double random = rnd.Rannyu();

    if(random<A){
        accepted_per_block++;
        return true;
    }
    else{
        return false;
    }

}

//metropolis stato 100
double metropolis_uniform_fundamental(Random &rnd, double &x,double &y,double &z,int &accepted_per_block){
    //mi deve ritornare r estratto dalla distribuzione di probabilità "fundamental_wave_function_modulus"
    
    double x_new=x+rnd.Rannyu(-1.2,1.2);
    double y_new=y+rnd.Rannyu(-1.2,1.2);
    double z_new=z+rnd.Rannyu(-1.2,1.2);

    if(A_fundamental(rnd, x,y,z,x_new,y_new,z_new, accepted_per_block)){ //x_new è estratto secondo distrib di probabilità definita dal modulo della funzione d'onda

        x=x_new;
        y=y_new;
        z=z_new;

        return sqrt(x*x+y*y+z*z);
    }

    else{

        return sqrt(x*x+y*y+z*z);

    }
    
}

//generazione r da distribuzione di probabilità 100
void fundamental_uniform_function(Random &rnd, int N_blocks, int throws_per_block, double progressive_r_uniform_fundamental[], double progressive_r_uniform_fundamental_error[]){

    double r_uniform_fundamental[N_blocks];
    double r_uniform_fundamental_2[N_blocks];

    double progressive_r_uniform_fundamental_2[N_blocks];


    double x=0.1;
    double y=0.1;
    double z=0.1;

    //apro file per salvare posizioni per tracciare traiettoria
    ofstream trajectory("../trajectories/trajectory_uniform_100.dat");
    trajectory << "x y z\n";

    int accepted_total = 0;

    //equilibriamento iniziale
    for(int i=0; i<1000; i++){
        int dummy = 0;
        metropolis_uniform_fundamental(rnd, x, y, z, dummy);
    }

    //valori per ogni blocco
    for(int i=0; i<N_blocks;i++){
        double sum=0;

        int accepted_per_block=0;

        for(int j=0;j<throws_per_block;j++){
            sum+=metropolis_uniform_fundamental(rnd,x,y,z,accepted_per_block);
            if(j % 1000 == 0){
                trajectory << x << " " << y << " " << z << "\n";
            }
        }

        r_uniform_fundamental[i]=sum/throws_per_block;
        r_uniform_fundamental_2[i]=r_uniform_fundamental[i]*r_uniform_fundamental[i];

        //cout << "Acceptance rate: " << double(accepted_per_block)/throws_per_block << endl;
        accepted_total+=accepted_per_block;
    }

    trajectory.close();

    cout << "Mean uniform fundamental state acceptance rate: " << double(accepted_total)/(N_blocks*throws_per_block) << endl;

    //media progressiva
    for(int i=0;i<N_blocks;i++){
        double sum=0;
        double sum_2=0;

        for(int j=0;j<i+1;j++){
            sum+=r_uniform_fundamental[j];
            sum_2+=r_uniform_fundamental_2[j];
        }

        progressive_r_uniform_fundamental[i]=sum/(i+1);
        progressive_r_uniform_fundamental_2[i]=sum_2/(i+1);

        progressive_r_uniform_fundamental_error[i]=error(progressive_r_uniform_fundamental,progressive_r_uniform_fundamental_2,i);

    }
}


//-----------------------------------------------EXCITED UNIFORM----------------------------------------------------------------------

//modulo quadro funzione d'onda stato 210
double excited_wave_function_squared_modulus(double x, double y, double z){
    double r = sqrt(x*x + y*y + z*z);
    return (1.0/(32.0*M_PI)) * z*z * exp(-r);
}

//Accettazione stato 210
bool A_excited(Random &rnd, double x, double y, double z, double x_new, double y_new, double z_new, int &accepted_per_block){
    double A=min(1.0,excited_wave_function_squared_modulus(x_new,y_new,z_new)/excited_wave_function_squared_modulus(x,y,z));      //T, ovvero lo step, è simmetrico. Si elide nella frazione

    double random = rnd.Rannyu();

    if(random<A){
        accepted_per_block++;
        return true;
    }
    else{
        return false;
    }

}

//metropolis stato 210
double metropolis_uniform_excited(Random &rnd, double &x,double &y,double &z,int &accepted_per_block){
    //mi deve ritornare r estratto dalla distribuzione di probabilità "excited_wave_function_modulus"
    
    double x_new=x+rnd.Rannyu(-2.95,2.95);
    double y_new=y+rnd.Rannyu(-2.95,2.95);
    double z_new=z+rnd.Rannyu(-2.95,2.95);

    if(A_excited(rnd, x,y,z,x_new,y_new,z_new, accepted_per_block)){ //x_new è estratto secondo distrib di probabilità definita dal modulo della funzione d'onda

        x=x_new;
        y=y_new;
        z=z_new;

        return sqrt(x*x+y*y+z*z);
    }

    else{

        return sqrt(x*x+y*y+z*z);

    }
    
}

//generazione r da distribuzione di probabilità 210
void excited_uniform_function(Random &rnd, int N_blocks, int throws_per_block, double progressive_r_uniform_excited[], double progressive_r_uniform_excited_error[]){

    double r_uniform_excited[N_blocks];
    double r_uniform_excited_2[N_blocks];

    double progressive_r_uniform_excited_2[N_blocks];


    double x=0.0;
    double y=0.0;
    double z=2.0;

    //apro file per salvare posizioni per tracciare traiettoria
    ofstream trajectory("../trajectories/trajectory_uniform_210.dat");
    trajectory << "x y z\n";

    int accepted_total = 0;

    //equilibriamento iniziale
    for(int i=0; i<1000; i++){
        int dummy = 0;
        metropolis_uniform_excited(rnd, x, y, z, dummy);
    }

    //valori per ogni blocco
    for(int i=0; i<N_blocks;i++){
        double sum=0;

        int accepted_per_block=0;

        for(int j=0;j<throws_per_block;j++){
            sum+=metropolis_uniform_excited(rnd,x,y,z,accepted_per_block);
            if(j % 1000 == 0){
                trajectory << x << " " << y << " " << z << "\n";
            }
        }

        r_uniform_excited[i]=sum/throws_per_block;
        r_uniform_excited_2[i]=r_uniform_excited[i]*r_uniform_excited[i];

        //cout << "Acceptance rate: " << double(accepted_per_block)/throws_per_block << endl;
        accepted_total+=accepted_per_block;
    }

    trajectory.close();

    cout << "Mean uniform excited state acceptance rate: " << double(accepted_total)/(N_blocks*throws_per_block) << endl;

    //media progressiva
    for(int i=0;i<N_blocks;i++){
        double sum=0;
        double sum_2=0;

        for(int j=0;j<i+1;j++){
            sum+=r_uniform_excited[j];
            sum_2+=r_uniform_excited_2[j];
        }

        progressive_r_uniform_excited[i]=sum/(i+1);
        progressive_r_uniform_excited_2[i]=sum_2/(i+1);

        progressive_r_uniform_excited_error[i]=error(progressive_r_uniform_excited,progressive_r_uniform_excited_2,i);

    }
}


//-----------------------------------------------FUNDAMENTAL GAUSSIAN-----------------------------------------------------------------

//metropolis stato 100
double metropolis_gaussian_fundamental(Random &rnd, double &x,double &y,double &z,int &accepted_per_block){
    //mi deve ritornare r estratto dalla distribuzione di probabilità "fundamental_wave_function_modulus"
    
    double x_new=x+rnd.Gauss(0,0.75);
    double y_new=y+rnd.Gauss(0,0.75);
    double z_new=z+rnd.Gauss(0,0.75);

    if(A_fundamental(rnd, x,y,z,x_new,y_new,z_new, accepted_per_block)){ //x_new è estratto secondo distrib di probabilità definita dal modulo della funzione d'onda

        x=x_new;
        y=y_new;
        z=z_new;

        return sqrt(x*x+y*y+z*z);
    }

    else{

        return sqrt(x*x+y*y+z*z);

    }
    
}

//generazione r da distribuzione di probabilità 100
void fundamental_gaussian_function(Random &rnd, int N_blocks, int throws_per_block, double progressive_r_fundamental[], double progressive_r_fundamental_error[]){

    double r_fundamental[N_blocks];
    double r_fundamental_2[N_blocks];

    double progressive_r_fundamental_2[N_blocks];


    double x=0.1;
    double y=0.1;
    double z=0.1;

    //apro file per salvare posizioni per tracciare traiettoria
    ofstream trajectory("../trajectories/trajectory_gaussian_100.dat");
    trajectory << "x y z\n";    

    int accepted_total = 0;

    //equilibriamento iniziale
    for(int i=0; i<1000; i++){
        int dummy = 0;
        metropolis_gaussian_fundamental(rnd, x, y, z, dummy);
    }

    //valori per ogni blocco
    for(int i=0; i<N_blocks;i++){
        double sum=0;

        int accepted_per_block=0;

        for(int j=0;j<throws_per_block;j++){
            sum+=metropolis_gaussian_fundamental(rnd,x,y,z,accepted_per_block);
            if(j % 1000 == 0){
                trajectory << x << " " << y << " " << z << "\n";
            }
        }

        r_fundamental[i]=sum/throws_per_block;
        r_fundamental_2[i]=r_fundamental[i]*r_fundamental[i];

        //cout << "Acceptance rate: " << double(accepted_per_block)/throws_per_block << endl;
        accepted_total+=accepted_per_block;
    }

    trajectory.close();

    cout << "Mean gaussian fundamental state acceptance rate: " << double(accepted_total)/(N_blocks*throws_per_block) << endl;

    //media progressiva
    for(int i=0;i<N_blocks;i++){
        double sum=0;
        double sum_2=0;

        for(int j=0;j<i+1;j++){
            sum+=r_fundamental[j];
            sum_2+=r_fundamental_2[j];
        }

        progressive_r_fundamental[i]=sum/(i+1);
        progressive_r_fundamental_2[i]=sum_2/(i+1);

        progressive_r_fundamental_error[i]=error(progressive_r_fundamental,progressive_r_fundamental_2,i);

    }
}


//-----------------------------------------------EXCITED GAUSSIAN---------------------------------------------------------------------

//metropolis stato 210
double metropolis_gaussian_excited(Random &rnd, double &x,double &y,double &z,int &accepted_per_block){
    //mi deve ritornare r estratto dalla distribuzione di probabilità "excited_wave_function_modulus"
    
    double x_new=x+rnd.Gauss(0,1.85);
    double y_new=y+rnd.Gauss(0,1.85);
    double z_new=z+rnd.Gauss(0,1.85);

    if(A_excited(rnd, x,y,z,x_new,y_new,z_new, accepted_per_block)){ //x_new è estratto secondo distrib di probabilità definita dal modulo della funzione d'onda

        x=x_new;
        y=y_new;
        z=z_new;

        return sqrt(x*x+y*y+z*z);
    }

    else{

        return sqrt(x*x+y*y+z*z);

    }
    
}

//generazione r da distribuzione di probabilità 210
void excited_gaussian_function(Random &rnd, int N_blocks, int throws_per_block, double progressive_r_excited[], double progressive_r_excited_error[]){

    double r_excited[N_blocks];
    double r_excited_2[N_blocks];

    double progressive_r_excited_2[N_blocks];


    double x=0.0;
    double y=0.0;
    double z=2.0;

    //apro file per salvare posizioni per tracciare traiettoria
    ofstream trajectory("../trajectories/trajectory_gaussian_210.dat");
    trajectory << "x y z\n";

    int accepted_total = 0;

    //equilibriamento iniziale
    for(int i=0; i<1000; i++){
        int dummy = 0;
        metropolis_gaussian_excited(rnd, x, y, z, dummy);
    }

    //valori per ogni blocco
    for(int i=0; i<N_blocks;i++){
        double sum=0;

        int accepted_per_block=0;

        for(int j=0;j<throws_per_block;j++){
            sum+=metropolis_gaussian_excited(rnd,x,y,z,accepted_per_block);
            if(j % 1000 == 0){
                trajectory << x << " " << y << " " << z << "\n";
            }
        }

        r_excited[i]=sum/throws_per_block;
        r_excited_2[i]=r_excited[i]*r_excited[i];

        //cout << "Acceptance rate: " << double(accepted_per_block)/throws_per_block << endl;
        accepted_total+=accepted_per_block;
    }

    trajectory.close();

    cout << "Mean gaussian excited state acceptance rate: " << double(accepted_total)/(N_blocks*throws_per_block) << endl;

    //media progressiva
    for(int i=0;i<N_blocks;i++){
        double sum=0;
        double sum_2=0;

        for(int j=0;j<i+1;j++){
            sum+=r_excited[j];
            sum_2+=r_excited_2[j];
        }

        progressive_r_excited[i]=sum/(i+1);
        progressive_r_excited_2[i]=sum_2/(i+1);

        progressive_r_excited_error[i]=error(progressive_r_excited,progressive_r_excited_2,i);

    }
}





int main() {
    Random rnd;
    initialize_random(rnd);

   int N_throws = 10000000;
   int N_blocks = 100;
   int throws_per_block = N_throws/N_blocks;   // numero di lanci per ciascun blocco

//-----------------------------------------------FUNDAMENTAL UNIFORM-------------------------------------------------------------------

    double progressive_r_uniform_fundamental[N_blocks]={0.0};
    double progressive_r_uniform_fundamental_error[N_blocks]={0.0};

    fundamental_uniform_function(rnd, N_blocks, throws_per_block, progressive_r_uniform_fundamental, progressive_r_uniform_fundamental_error);

//-----------------------------------------------EXCITED UNIFORM---------------------------------------------------------------------

    double progressive_r_uniform_excited[N_blocks]={0.0};
    double progressive_r_uniform_excited_error[N_blocks]={0.0};

    excited_uniform_function(rnd, N_blocks, throws_per_block, progressive_r_uniform_excited, progressive_r_uniform_excited_error);  

//-----------------------------------------------FUNDAMENTAL GAUSSIAN-----------------------------------------------------------------

    double progressive_r_gaussian_fundamental[N_blocks]={0.0};
    double progressive_r_gaussian_fundamental_error[N_blocks]={0.0};

    fundamental_gaussian_function(rnd, N_blocks, throws_per_block, progressive_r_gaussian_fundamental, progressive_r_gaussian_fundamental_error);
//-----------------------------------------------EXCITED GAUSSIAN---------------------------------------------------------------------

    double progressive_r_gaussian_excited[N_blocks]={0.0};
    double progressive_r_gaussian_excited_error[N_blocks]={0.0};

    excited_gaussian_function(rnd, N_blocks, throws_per_block, progressive_r_gaussian_excited, progressive_r_gaussian_excited_error);


//-----------------------------------------------OUTPUT------------------------------------------------------------------------------

    ofstream output("../Results/Esercizio_05.1_results.txt");

    output << "Progressive_r_uniform_fundamental" << ";"
            << "Progressive_r_uniform_fundamental_error" << ";"
            << "Progressive_r_uniform_excited" << ";"
            << "Progressive_r_uniform_excited_error" << ";"
            << "Progressive_r_gaussian_fundamental" << ";"
            << "Progressive_r_gaussian_fundamental_error" << ";"
            << "Progressive_r_gaussian_excited" << ";"
            << "Progressive_r_gaussian_excited_error" << endl;



    for (int i = 0; i < N_blocks; i++) {
        output  << progressive_r_uniform_fundamental[i] << ";"
                << progressive_r_uniform_fundamental_error[i] << ";"
                << progressive_r_uniform_excited[i] << ";"
                << progressive_r_uniform_excited_error[i] << ";"
                << progressive_r_gaussian_fundamental[i] << ";"
                << progressive_r_gaussian_fundamental_error[i] << ";"
                << progressive_r_gaussian_excited[i] << ";"
                << progressive_r_gaussian_excited_error[i] << endl;
    }


    output.close();

    rnd.SaveSeed();
    return 0;
}