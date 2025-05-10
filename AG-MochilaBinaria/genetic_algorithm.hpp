#ifndef GENETIC_ALGORITHM_HPP
#define GENETIC_ALGORITHM_HPP

#include <vector>
#include <string>
#include <random>

namespace genetic_algorithm {

// Gerador de números aleatórios global
extern std::mt19937 rng;

// Representação do problema da mochila
class Knapsack {
public:
    Knapsack(std::size_t capacity, std::string optimal_solution,
             std::vector<std::size_t> weights, std::vector<std::size_t> profits);

    std::size_t get_capacity() const;
    const std::string& get_optimal_solution() const;
    const std::vector<std::size_t>& get_weights() const;
    const std::vector<std::size_t>& get_profits() const;

private:
    std::size_t m_capacity;
    std::string m_optimal_solution;
    std::vector<std::size_t> m_weights;
    std::vector<std::size_t> m_profits;
};

// Representação de um indivíduo
class Individual {
public:
    Individual();
    Individual(const Knapsack& knap, std::size_t length);
    Individual(const Knapsack& knap, const std::string& value);

    const std::string& get_value() const;
    long long get_fitness() const;
    std::size_t get_capacity() const;
    void set_fitness(long long fitness);

    char operator[](size_t i) const;
    bool operator>(const Individual& rhs) const;
    bool operator<(const Individual& rhs) const;
    bool operator==(const Individual& rhs) const;

private:
    std::string m_value;
    long long m_fitness;
    std::size_t m_capacity;
};

// Funções auxiliares de método
namespace method {
    long long calculate_fitness(const Knapsack& knap, const std::string& x);
    Individual roulette_selection(const std::vector<Individual>& population);
    Individual tournament_selection(const std::vector<Individual>& population, std::size_t tournament_size, double prob_victory);
    Individual create_child(const Individual& p1, const Individual& p2, const Knapsack& knap, int parent_ratio, int mutate_probability);
    std::string get_random_string(std::size_t length);
}

// População de indivíduos
class Population {
public:
    Population(std::size_t pop_size, std::size_t parent_ratio,
               std::size_t mutate_probability, std::size_t elite_ratio,
               int choice_selection_method, const Knapsack& knap);

    void evolve();
    const Individual& best_individual() const;
    std::size_t get_generation() const;
    const std::vector<Individual>& get_individuals() const;

private:
    void calculate_fitness_all();
    void next_generation();

    std::vector<Individual> m_population;
    std::size_t m_generation;
    std::size_t m_parent_ratio;
    std::size_t m_mutate_probability;
    std::size_t m_elite_count;
    int m_choice_selection_method;
    const Knapsack& m_knapsack;
};

// Classe para guardar resultados
class DataResults {
public:
    DataResults() = default;
    
    std::vector<Individual> best_individuos_per_generation;
    std::vector<std::vector<Individual>> population_per_generation;
};

// Função principal do algoritmo genético
DataResults genetic_algorithm(
    int max_generations,
    genetic_algorithm::Knapsack& knap,
    int choice_method_selection,
    std::size_t pop_size = 500,
    std::size_t parent_ratio = 100,
    std::size_t mutate_probability = 2,
    std::size_t elite_ratio = 2
);

} // namespace genetic_algorithm

#endif // GENETIC_ALGORITHM_HPP
