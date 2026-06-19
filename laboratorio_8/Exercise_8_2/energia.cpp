#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <vector>

#include "random.h"

using namespace std;

double mu;
double sigma;

void initialize_random(Random &rnd) {
    int seed[4];
    int p1, p2;

    ifstream Primes("Primes");
    if (Primes.is_open()) {
        Primes >> p1 >> p2;
    } else {
        cerr << "PROBLEM: Unable to open Primes" << endl;
        exit(1);
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
        exit(1);
    }
}

double error(const vector<double> &media, const vector<double> &media2, int n) {
    if (n == 0) {
        return 0.0;
    } else {
        return sqrt((media2[n] - pow(media[n], 2)) / n);
    }
}

// Potenziale V(x)
double potential(double x) {
    return pow(x, 4) - 2.5 * x * x;
}

// Energia locale: H psi / psi
double Energy(double x, double mu, double sigma) {
    double s2 = sigma * sigma;
    double s4 = s2 * s2;

    double g1 = exp(-pow(x - mu, 2) / (2.0 * s2));
    double g2 = exp(-pow(x + mu, 2) / (2.0 * s2));

    double psi_x = g1 + g2;

    double psi_second =
        g1 * (pow(x - mu, 2) / s4 - 1.0 / s2)
        +
        g2 * (pow(x + mu, 2) / s4 - 1.0 / s2);

    double kinetic = -0.5 * psi_second / psi_x;

    return kinetic + potential(x);
}

// |psi_T(x)|^2
double probability_distribution_squared_modulus(double x, double mu, double sigma) {
    double psi =
        exp(-pow(x - mu, 2) / (2.0 * pow(sigma, 2))) +
        exp(-pow(x + mu, 2) / (2.0 * pow(sigma, 2)));

    return psi * psi;
}

// Accettazione Metropolis su x
bool Acceptance(Random &rnd, double x, double x_new, int &accepted_per_block) {
    double p_old = probability_distribution_squared_modulus(x, mu, sigma);
    double p_new = probability_distribution_squared_modulus(x_new, mu, sigma);

    double A = min(1.0, p_new / p_old);

    double random = rnd.Rannyu();

    if (random < A) {
        accepted_per_block++;
        return true;
    } else {
        return false;
    }
}

// Singolo passo Metropolis su x
double metropolis(Random &rnd, double &x, int &accepted_per_block) {
    double x_new = x + rnd.Rannyu(-1.2, 1.2);

    if (Acceptance(rnd, x, x_new, accepted_per_block)) {
        x = x_new;
    }

    return Energy(x, mu, sigma);
}

// Calcola la media a blocchi dell'energia
void H_mean_blocks(
    Random &rnd,
    int N_blocks,
    int throws_per_block,
    vector<double> &progressive_H,
    vector<double> &progressive_H_error,
    bool save_samples
) {
    vector<double> H(N_blocks, 0.0);
    vector<double> H_2(N_blocks, 0.0);
    vector<double> progressive_H_2(N_blocks, 0.0);

    double x = 0.1;

    ofstream samples;

    if (save_samples) {
        samples.open("../sampled_configurations.dat");
        samples << "x" << endl;
    }

    // Equilibrazione iniziale
    for (int i = 0; i < 1000; i++) {
        int dummy = 0;
        metropolis(rnd, x, dummy);
    }

    // Blocchi
    for (int i = 0; i < N_blocks; i++) {
        double sum = 0.0;
        int accepted_per_block = 0;

        for (int j = 0; j < throws_per_block; j++) {
            sum += metropolis(rnd, x, accepted_per_block);

            if (save_samples) {
                samples << x << endl;
            }
        }

        H[i] = sum / throws_per_block;
        H_2[i] = H[i] * H[i];
    }

    // Medie progressive
    for (int i = 0; i < N_blocks; i++) {
        double sum = 0.0;
        double sum_2 = 0.0;

        for (int j = 0; j < i + 1; j++) {
            sum += H[j];
            sum_2 += H_2[j];
        }

        progressive_H[i] = sum / (i + 1);
        progressive_H_2[i] = sum_2 / (i + 1);

        progressive_H_error[i] = error(progressive_H, progressive_H_2, i);
    }

    if (save_samples) {
        samples.close();
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3 && argc != 4) {
        cerr << "Usage: " << argv[0] << " mu sigma [save_blocks]" << endl;
        return 1;
    }

    bool save_blocks = false;

    if (argc == 4) {
        string option = argv[3];

        if (option == "save_blocks") {
            save_blocks = true;
        }
    }

    mu = atof(argv[1]);
    sigma = atof(argv[2]);

    if (sigma <= 0.0) {
        cerr << "Error: sigma must be positive" << endl;
        return 1;
    }

    Random rnd;
    initialize_random(rnd);

    int N_throws;
    int N_blocks = 100;

    if (save_blocks) {
        N_throws = 1000000;
    } else {
        N_throws = 10000;
    }

    int throws_per_block = N_throws / N_blocks;

    vector<double> progressive_H(N_blocks, 0.0);
    vector<double> progressive_H_error(N_blocks, 0.0);

    H_mean_blocks(
        rnd,
        N_blocks,
        throws_per_block,
        progressive_H,
        progressive_H_error,
        save_blocks
    );

    if (save_blocks) {
        ofstream output("../Esercizio_08.2_final_energy_blocks.txt");

        output << "MC_steps" << ";"
               << "Progressive_Energy" << ";"
               << "Progressive_Energy_error" << endl;

        for (int i = 0; i < N_blocks; i++) {
            output << (i + 1) * throws_per_block << ";"
                   << progressive_H[i] << ";"
                   << progressive_H_error[i] << endl;
        }

        output.close();
    }

    cout << setprecision(17) 
        << progressive_H[N_blocks - 1] << " "
        << progressive_H_error[N_blocks - 1] 
        << endl;

    rnd.SaveSeed();

    return 0;
}