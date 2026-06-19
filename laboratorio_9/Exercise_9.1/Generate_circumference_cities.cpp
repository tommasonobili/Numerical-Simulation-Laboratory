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

void generate_cities_on_circle(Random &rnd, double radius, int number_of_cities, const string &filename) {
    ofstream output(filename);

    if (!output.is_open()) {
        cerr << "PROBLEM: Unable to open " << filename << endl;
        return;
    }

    const double pi = acos(-1.0);

    for (int i = 0; i < number_of_cities; i++) {
        double theta = rnd.Rannyu(0.0, 2.0 * pi);

        double x = radius * cos(theta);
        double y = radius * sin(theta);

        output << i + 1 << " " << x << " " << y << endl;
    }

    output.close();
}

int main() {
    Random rnd;
    initialize_random(rnd);

    double radius = 1.0;
    int number_of_cities = 34;

    generate_cities_on_circle(rnd, radius, number_of_cities, "cities_circle.txt");

    rnd.SaveSeed();

    return 0;
}