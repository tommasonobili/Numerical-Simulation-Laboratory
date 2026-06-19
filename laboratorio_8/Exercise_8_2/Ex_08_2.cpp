#include <iostream>
#include <fstream>
#include <string>
#include "random.h"
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <limits>
#include <array>

using namespace std;

const double sigma_min = 0.05;
double last_H_error = 0.0;

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


double H_VECCHIO_CODICE_PROPOSTO(double mu, double sigma) {

    // The trial wave function depends on sigma, which must be positive.
    // If the proposed sigma is too small or non-physical, this point is rejected
    // by assigning an infinite energy.
    if (sigma <= sigma_min) {
        last_H_error = 0.0;
        return numeric_limits<double>::infinity();
    }

    // Build the command that will be executed in the terminal.
    ostringstream cmd;
    cmd << "./energia.exe "
        << setprecision(17) << mu << " "
        << setprecision(17) << sigma;

    // Launch energia.exe and open a pipe to read its standard output.
    // The option "r" means that the current program reads what energia.exe prints.
    // This is needed because energia.exe prints the estimated energy and error
    // instead of returning them directly as C++ variables.
    FILE* pipe = popen(cmd.str().c_str(), "r");

    // If pipe is null, the external executable could not be started.
    if (!pipe) {
        throw runtime_error("Errore: impossibile lanciare energia.exe");
    }

    // Buffer used to read the text printed by energia.exe.
    // The output is expected to contain two numbers:
    // final_energy final_error
    array<char, 256> buffer;
    string result;

    // Read all lines printed by energia.exe and concatenate them into result.
    // In this case the output should be just one line, but the loop is general.
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }

    // Close the pipe and obtain the exit status of energia.exe.
    // A non-zero return code means that energia.exe ended with an error.
    int return_code = pclose(pipe);

    if (return_code != 0) {
        throw runtime_error("Errore: energia.exe terminato con errore");
    }

    // Convert the string output into numerical values.
    // iss reads the content of result as if it were an input stream.
    istringstream iss(result);
    double H;
    double H_err;

    // Read the first number printed by energia.exe.
    // This is the final progressive block average of the VMC energy
    // for the proposed parameters mu and sigma.
    if (!(iss >> H)) {
        throw runtime_error(
            "Errore: output di energia.exe non leggibile come double. Output ricevuto: ["
            + result + "]"
        );
    }

    // Read the second number printed by energia.exe.
    // This is the statistical uncertainty from data blocking.
    // It is stored in the global variable last_H_error so that the SA code
    // can save the uncertainty associated with the current/best energy.
    if (iss >> H_err) {
        last_H_error = H_err;
    } else {
        last_H_error = 0.0;
    }

    // Return only the estimated energy.
    // This value is used by the Simulated Annealing algorithm to decide
    // whether the proposed pair (mu, sigma) should be accepted.
    return H;
}


// Distribuzione di Boltzmann nello spazio dei parametri variazionali
double probability_distribution(double H_current, double H_new, double T) {

    double pdf = exp(-(H_new - H_current) / T);

    return pdf;
}

// Accettazione Metropolis per Simulated Annealing
bool Acceptance(Random &rnd, double H_current, double H_new, int &accepted_per_temperature, double T) {

    double A = min(1.0, probability_distribution(H_current, H_new, T));

    double random = rnd.Rannyu();

    if (random < A) {
        accepted_per_temperature++;
        return true;
    } else {
        return false;
    }
}

// Singolo passo Metropolis nello spazio (mu, sigma)
double metropolis(Random &rnd,double &mu,double &sigma,int &accepted_per_temperature,double T,double &H_current,double &H_current_error) {

    double mu_new = mu + rnd.Rannyu(-0.04, 0.04);
    double sigma_new = sigma + rnd.Rannyu(-0.03, 0.03);

    if (sigma_new <= sigma_min) {
        return H_current;
    }

    double H_new = H_VECCHIO_CODICE_PROPOSTO(mu_new, sigma_new);
    double H_new_error = last_H_error;

    if (Acceptance(rnd, H_current, H_new, accepted_per_temperature, T)) {

        mu = mu_new;
        sigma = sigma_new;

        H_current = H_new;
        H_current_error = H_new_error;
    }

    return H_current;
}

// Simulated Annealing: salvo energia, errore, traiettoria e migliori parametri trovati
void simulated_annealing(
    Random &rnd,
    int N_temperature_steps,
    int moves_per_temperature,
    double SA_Energy[],
    double SA_Energy_error[],
    double &best_mu,
    double &best_sigma,
    double &best_H,
    double &best_H_error
) {

    double mu = 1.1;
    double sigma = 0.5;

    double H_current = H_VECCHIO_CODICE_PROPOSTO(mu, sigma);
    double H_current_error = last_H_error;

    best_mu = mu;
    best_sigma = sigma;
    best_H = H_current;
    best_H_error = H_current_error;

    ofstream trajectory("../trajectory_mu_sigma.dat");

    trajectory << "SA_step" << ";"
               << "mu" << ";"
               << "sigma" << endl;

    trajectory << 0 << ";"
               << mu << ";"
               << sigma << endl;

    int accepted_total = 0;

    for (int i = 0; i < N_temperature_steps; i++) {

        double T_current = 1.0 / double(i + 1);

        int accepted_per_temperature = 0;

        for (int j = 0; j < moves_per_temperature; j++) {

            metropolis(rnd,mu,sigma,accepted_per_temperature,T_current,H_current,H_current_error);

            if (H_current < best_H) {
                best_H = H_current;
                best_H_error = H_current_error;
                best_mu = mu;
                best_sigma = sigma;
            }

            int SA_step = i * moves_per_temperature + j + 1;

            trajectory << SA_step << ";"
                       << mu << ";"
                       << sigma << endl;
        }

        accepted_total += accepted_per_temperature;

        SA_Energy[i] = H_current;
        SA_Energy_error[i] = H_current_error;
    }

    cout << "Mean acceptance rate: "
         << double(accepted_total) / (N_temperature_steps * moves_per_temperature)
         << endl;

    trajectory.close();
}

int main() {

    Random rnd;
    initialize_random(rnd);

    const int N_SA_steps = 100000;
    const int N_temperature_steps = 1000;
    const int moves_per_temperature = N_SA_steps / N_temperature_steps;

    double SA_Energy[N_temperature_steps] = {0.0};
    double SA_Energy_error[N_temperature_steps] = {0.0};

    double best_mu = 0.0;
    double best_sigma = 0.0;
    double best_H = 0.0;
    double best_H_error = 0.0;

    simulated_annealing(
        rnd,
        N_temperature_steps,
        moves_per_temperature,
        SA_Energy,
        SA_Energy_error,
        best_mu,
        best_sigma,
        best_H,
        best_H_error
    );

    ofstream output("../Esercizio_08.2_SA_energy_steps.txt");

    output << "SA_step" << ";"
           << "Energy" << ";"
           << "Energy_error" << endl;

    for (int i = 0; i < N_temperature_steps; i++) {
        output << (i + 1) * moves_per_temperature << ";"
               << SA_Energy[i] << ";"
               << SA_Energy_error[i] << endl;
    }

    output.close();

    ofstream best_output("../best_parameters.dat");

    best_output << "mu" << ";"
                << "sigma" << ";"
                << "Energy" << ";"
                << "Energy_error" << endl;

    best_output << best_mu << ";"
                << best_sigma << ";"
                << best_H << ";"
                << best_H_error << endl;

    best_output.close();

    cout << "Best parameters found:" << endl;
    cout << "mu = " << best_mu << endl;
    cout << "sigma = " << best_sigma << endl;
    cout << "H = " << best_H << " +/- " << best_H_error << endl;

    rnd.SaveSeed();

    return 0;
}