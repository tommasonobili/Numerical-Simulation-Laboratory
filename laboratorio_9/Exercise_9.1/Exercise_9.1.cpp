#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "random.h"
#include <cmath>
#include <algorithm>
#include <utility>

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


struct City {
    int id;
    double x;
    double y;
};

struct Individual {   // è un percorso
    vector<int> path;
    double loss;
};

vector<City> read_cities(const string &filename) {
    vector<City> cities;

    ifstream input(filename);

    if (!input.is_open()) {
        cerr << "PROBLEM: Unable to open " << filename << endl;
        return cities;
    }

    City city;  // Crea una variabile temporanea di tipo City

    while (input >> city.id >> city.x >> city.y) {
        cities.push_back(city);
    }

    input.close();

    return cities;
}


double distance_between_cities(const City &a, const City &b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;

    return sqrt(dx * dx + dy * dy);
}


//lunghezza totale del percorso di un individuo.
double path_length(const Individual &individual, const vector<City> &cities) {
    double length = 0.0;

    int number_of_cities = individual.path.size();

    for (int i = 0; i < number_of_cities - 1; i++) { //Questo ciclo somma le distanze tra città consecutive. Se è 1 5 8 -> somma d(1,5)+d(5,8)+d(5,9)
        int id_a = individual.path[i];
        int id_b = individual.path[i + 1];

        length += distance_between_cities(cities[id_a - 1], cities[id_b - 1]);
    }

    int last_id = individual.path[number_of_cities - 1];
    int first_id = individual.path[0];

    length += distance_between_cities(cities[last_id - 1], cities[first_id - 1]);

    return length;
}


//controlla che l indivuiduo abbia senso (path ok, neinte ripetizioni)
bool check_individual(const Individual &individual, int number_of_cities) {
    if ((int)individual.path.size() != number_of_cities) {
        return false;
    }

    if (individual.path[0] != 1) {
        return false;
    }

    vector<int> counter(number_of_cities + 1, 0); // conta quante volte appare una città

    for (int i = 0; i < number_of_cities; i++) {
        int city_id = individual.path[i];

        if (city_id < 1 || city_id > number_of_cities) {
            return false;
        }

        counter[city_id]++;
    }

    for (int i = 1; i <= number_of_cities; i++) {
        if (counter[i] != 1) {
            return false;
        }
    }

    return true;
}

//crea un percorso casuale valido.
Individual generate_random_individual(Random &rnd, int number_of_cities) {
    Individual individual;

    individual.path.resize(number_of_cities);

    for (int i = 0; i < number_of_cities; i++) {
        individual.path[i] = i + 1;
    }

    // Fisher-Yates shuffle, ma lasciando fissa la città 1 in prima posizione
    for (int i = 1; i < number_of_cities; i++) {
        int j = i + int(rnd.Rannyu(0.0, double(number_of_cities - i)));

        swap(individual.path[i], individual.path[j]);
    }

    individual.loss = 0.0;

    return individual;
}

//crea N individui(percorsi) casuali -> Popolazione
vector<Individual> generate_population(Random &rnd, int population_size, int number_of_cities) {
    vector<Individual> population;

    for (int i = 0; i < population_size; i++) {
        Individual individual = generate_random_individual(rnd, number_of_cities);

        if (!check_individual(individual, number_of_cities)) {
            cerr << "PROBLEM: generated invalid individual" << endl;
        }

        population.push_back(individual);
    }

    return population;
}


//Scorre tutti gli individui e assegna a ciascuno la lunghezza del suo percorso.
//Dopo questa funzione, ogni individuo ha un valore sensato di loss.
void evaluate_population(vector<Individual> &population, const vector<City> &cities) {
    for (int i = 0; i < (int)population.size(); i++) {
        population[i].loss = path_length(population[i], cities);
    }
}

//ordina la popolazione dal percorso migliore al peggiore
void sort_population(vector<Individual> &population) {
    sort(population.begin(), population.end(),
         [](const Individual &a, const Individual &b) {
             return a.loss < b.loss;
         });
}


//-------------------------------------------------------
//OPERATORI
//-------------------------------------------------------

//SELETTORE

int select_index(Random &rnd, int population_size, double selection_power) {
    double r = rnd.Rannyu();

    //index = int(M * r^p) (DOPO AVERE ORDINATA LA POPOLAZIONE)
    int index = int(population_size * pow(r, selection_power));

    if (index >= population_size) {
        index = population_size - 1;
    }

    return index;
}

//CROSSOVER

//Questa serve al crossover per controllare se una città è già presente nel figlio.
bool contains_city(const vector<int> &path, int city_id, int end_position) {
    for (int i = 0; i < end_position; i++) {
        if (path[i] == city_id) {
            return true;
        }
    }

    return false;
}

Individual make_child_from_parents(const Individual &first_parent, const Individual &second_parent, int cut) {
    int number_of_cities = first_parent.path.size();

    Individual child;
    child.path.resize(number_of_cities);

    // La città 1 resta fissa
    child.path[0] = 1;

    // Copio la prima parte dal primo genitore
    for (int i = 1; i < cut; i++) {
        child.path[i] = first_parent.path[i];
    }

    // Completo con le città mancanti nell'ordine del secondo genitore
    int child_position = cut;

    for (int i = 1; i < number_of_cities; i++) {
        int candidate_city = second_parent.path[i];

        if (!contains_city(child.path, candidate_city, child_position)) {
            child.path[child_position] = candidate_city;
            child_position++;
        }
    }

    child.loss = 0.0;

    return child;
}

pair<Individual, Individual> crossover(Random &rnd, const Individual &mother, const Individual &father) {
    int number_of_cities = mother.path.size();

    // Scelgo un unico punto di taglio per entrambi i figli
    int cut = int(rnd.Rannyu(2.0, double(number_of_cities)));

    Individual child1 = make_child_from_parents(mother, father, cut);
    Individual child2 = make_child_from_parents(father, mother, cut);

    return make_pair(child1, child2);
}


//MUTATIONS

void swap_mutation(Random &rnd, Individual &individual) {
    int number_of_cities = individual.path.size();

    int i = int(rnd.Rannyu(1.0, double(number_of_cities)));
    int j = int(rnd.Rannyu(1.0, double(number_of_cities)));

    while (j == i) {
        j = int(rnd.Rannyu(1.0, double(number_of_cities)));
    }

    swap(individual.path[i], individual.path[j]);

    individual.loss = 0.0;
}

void shift_mutation(Random &rnd, Individual &individual) {
    int number_of_cities = individual.path.size();

    int tail_size = number_of_cities - 1;

    if (tail_size < 2) {
        return;
    }

    int max_block_length = tail_size / 2;

    int block_length = 1 + int(rnd.Rannyu(0.0, double(max_block_length)));

    int start = int(rnd.Rannyu(0.0, double(tail_size - block_length + 1)));

    vector<int> tail(individual.path.begin() + 1, individual.path.end());

    vector<int> block(
        tail.begin() + start,
        tail.begin() + start + block_length
    );

    tail.erase(
        tail.begin() + start,
        tail.begin() + start + block_length
    );

    int insertion_position = int(rnd.Rannyu(0.0, double(tail.size() + 1)));

    while (insertion_position == start) {
        insertion_position = int(rnd.Rannyu(0.0, double(tail.size() + 1)));
    }

    tail.insert(
        tail.begin() + insertion_position,
        block.begin(),
        block.end()
    );

    for (int i = 0; i < tail_size; i++) {
        individual.path[i + 1] = tail[i];
    }

    individual.loss = 0.0;
}


//----------------------------------------------------
// EVOLUZIONE POPOLAZIONE
//----------------------------------------------------

vector<Individual> create_next_generation(Random &rnd,
                                          const vector<Individual> &population,
                                          int population_size,
                                          int number_of_cities,
                                          double selection_power,
                                          double crossover_probability,
                                          double swap_probability,
                                          double shift_probability) {
    vector<Individual> new_population;

    while ((int)new_population.size() < population_size) {
        int mother_index = select_index(rnd, population_size, selection_power);
        int father_index = select_index(rnd, population_size, selection_power);

        while (father_index == mother_index) {
            father_index = select_index(rnd, population_size, selection_power);
        }

        Individual mother = population[mother_index];
        Individual father = population[father_index];

        Individual child1;
        Individual child2;

        if (rnd.Rannyu() < crossover_probability) {
            pair<Individual, Individual> children = crossover(rnd, mother, father);

            child1 = children.first;
            child2 = children.second;
        } else {
            child1 = mother;
            child2 = father;

            child1.loss = 0.0;
            child2.loss = 0.0;
        }


        if (rnd.Rannyu() < swap_probability) {
            swap_mutation(rnd, child1);
        }

        if (rnd.Rannyu() < shift_probability) {
            shift_mutation(rnd, child1);
        }

        if (rnd.Rannyu() < swap_probability) {
            swap_mutation(rnd, child2);
        }

        if (rnd.Rannyu() < shift_probability) {
            shift_mutation(rnd, child2);
        }

        if (!check_individual(child1, number_of_cities)) {
            cerr << "PROBLEM: invalid child 1 in new generation" << endl;
        }

        if (!check_individual(child2, number_of_cities)) {
            cerr << "PROBLEM: invalid child 2 in new generation" << endl;
        }

        new_population.push_back(child1);

        if ((int)new_population.size() < population_size) {
            new_population.push_back(child2);
        }
    }

    return new_population;
}






//----------------------------------------------------
//OUTPUTS
//----------------------------------------------------

void print_individual(const Individual &individual) {
    for (int i = 0; i < (int)individual.path.size(); i++) {
        cout << individual.path[i] << " ";
    }

    cout << "   L = " << individual.loss << endl;
}

//calcola la media delle loss della metà migliore della popolazione
double average_best_half(const vector<Individual> &population) {
    int half_size = population.size() / 2;

    double sum = 0.0;

    for (int i = 0; i < half_size; i++) {
        sum += population[i].loss;
    }

    return sum / double(half_size);
}

void save_best_path(const Individual &best, const string &filename) {
    ofstream output(filename);

    if (!output.is_open()) {
        cerr << "PROBLEM: Unable to open " << filename << endl;
        return;
    }

    for (int i = 0; i < (int)best.path.size(); i++) {
        output << best.path[i] << endl;
    }

    // Chiudo il percorso tornando alla città iniziale
    output << best.path[0] << endl;

    output.close();
}




int main() {
    Random rnd;
    initialize_random(rnd);

    const string cities_filename = "cities_circle.txt";

    const int number_of_cities = 34;
    const int population_size = 1000;
    const int number_of_generations = 1000;

    const double selection_power = 2.0;
    const double crossover_probability = 0.80;
    const double shift_probability = 0.10;
    const double swap_probability = 0.10;

    //early stopping
    vector<double> early_stopping_best(number_of_generations + 1);  
    vector<double> early_stopping_best_half(number_of_generations + 1);


    vector<City> cities = read_cities(cities_filename);

    if ((int)cities.size() != number_of_cities) {
        cerr << "PROBLEM: wrong number of cities" << endl;
        cerr << "Read " << cities.size() << " cities instead of " << number_of_cities << endl;
        return 1;
    }

    vector<Individual> population =
        generate_population(rnd, population_size, number_of_cities);

    evaluate_population(population, cities);
    sort_population(population);

    cout << "Initial population:" << endl;
    cout << "Best L = " << population[0].loss << endl;
    cout << "Average best half = " << average_best_half(population) << endl;

    save_best_path(population[0], "best_path_initial.txt");


    //OUTPUT
    ofstream losses_output("losses.dat");

    if (!losses_output.is_open()) {
        cerr << "PROBLEM: Unable to open losses.dat" << endl;
        return 1;
    }

    losses_output << 0 << " "
                << population[0].loss << " "
                << average_best_half(population) << endl;


    //EVOLUTION

    cout << endl;
    cout << "Evolution:" << endl;

    for (int generation = 1; generation <= number_of_generations; generation++) {
        population = create_next_generation(rnd,
                                            population,
                                            population_size,
                                            number_of_cities,
                                            selection_power,
                                            crossover_probability,
                                            swap_probability,
                                            shift_probability);

        evaluate_population(population, cities);
        sort_population(population);

        losses_output << generation << " "
              << population[0].loss << " "
              << average_best_half(population) << endl;

        cout << "Generation " << generation
             << "   best L = " << population[0].loss
             << "   average best half = " << average_best_half(population)
             << endl;

        //early stopping
        early_stopping_best[generation]=population[0].loss;
        early_stopping_best_half[generation]=average_best_half(population);

        if (generation > 11){
            if ((fabs(early_stopping_best[generation - 10] - early_stopping_best[generation])<=0.00001)&&
                (fabs(early_stopping_best_half[generation - 10] - early_stopping_best_half[generation])<=0.02)) {
                break;
            }
        }
    }

    save_best_path(population[0], "best_path_final.txt");

    cout << endl;
    cout << "Final best individual:" << endl;
    print_individual(population[0]);

    losses_output.close();

    rnd.SaveSeed();

    return 0;
}