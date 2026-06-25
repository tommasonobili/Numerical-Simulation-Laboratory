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

void generate_cities_in_square(Random &rnd, double side, int number_of_cities, const string &filename) {
    ofstream output(filename);

    if (!output.is_open()) {
        cerr << "PROBLEM: Unable to open " << filename << endl;
        return;
    }

    for (int i = 0; i < number_of_cities; i++) {


        double x = rnd.Rannyu(0,side);
        double y = rnd.Rannyu(0,side);

        output << i + 1 << " " << x << " " << y << endl;
    }

    output.close();
}

int main() {
    Random rnd;
    initialize_random(rnd);

    double side = 1.0;
    int number_of_cities = 34;

    generate_cities_in_square(rnd, side, number_of_cities, "cities_square.txt");

    rnd.SaveSeed();

    return 0;
}