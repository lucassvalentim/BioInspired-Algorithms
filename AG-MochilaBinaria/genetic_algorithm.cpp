#include "genetic_algorithm.hpp"
#include <algorithm>
#include <numeric>
#include <set>
#include <iostream>

namespace genetic_algorithm {

std::mt19937 rng(std::random_device{}());

/* ----------- Knapsack ----------- */
Knapsack::Knapsack(std::size_t capacity, std::string optimal_solution,
                   std::vector<std::size_t> weights, std::vector<std::size_t> profits)
    : m_capacity(capacity), m_optimal_solution(optimal_solution),
      m_weights(std::move(weights)), m_profits(std::move(profits)) {}

std::size_t Knapsack::get_capacity() const { 
    return m_capacity; 
}
const std::string& Knapsack::get_optimal_solution() const { 
    return m_optimal_solution; 
}
const std::vector<std::size_t>& Knapsack::get_weights() const { 
    return m_weights; 
}
const std::vector<std::size_t>& Knapsack::get_profits() const { 
    return m_profits; 
}

/* ----------- Individual ----------- */
Individual::Individual() : m_value(""), m_fitness(0), m_capacity(0) {}

Individual::Individual(const Knapsack& knap, std::size_t length)
    : m_value(method::get_random_string(length)), m_fitness(0), m_capacity(0) {
    for (std::size_t i = 0; i < length; ++i)
        if (m_value[i] == '1') m_capacity += knap.get_weights()[i];
}

Individual::Individual(const Knapsack& knap, const std::string& value)
    : m_value(value), m_fitness(0), m_capacity(0) {
    for (std::size_t i = 0; i < value.size(); ++i)
        if (value[i] == '1') m_capacity += knap.get_weights()[i];
}

const std::string& Individual::get_value() const {
    return m_value; 
}
long long Individual::get_fitness() const { 
    return m_fitness; 
}
std::size_t Individual::get_capacity() const { 
    return m_capacity; 
}
void Individual::set_fitness(long long fitness) { 
    m_fitness = fitness; 
}

char Individual::operator[](size_t i) const { 
    return m_value[i]; 
}
bool Individual::operator>(const Individual& rhs) const { 
    return m_fitness > rhs.m_fitness; 
}
bool Individual::operator<(const Individual& rhs) const { 
    return m_fitness < rhs.m_fitness; 
}
bool Individual::operator==(const Individual& rhs) const { 
    return m_fitness == rhs.m_fitness; 
}

/* ----------- Methods ----------- */
namespace method {

std::string get_random_string(std::size_t length) {
    std::string s(length, '0');
    std::uniform_int_distribution<int> bit_dist(0, 1);
    for (auto& c : s)
        c = bit_dist(rng) ? '1' : '0';
    return s;
}

long long calculate_fitness(const Knapsack& knap, const std::string& x) {
    std::size_t capacity = 0, fitness = 0;
    for (std::size_t i = 0; i < x.size(); ++i) {
        if (x[i] == '1') {
            capacity += knap.get_weights()[i];
            fitness += knap.get_profits()[i];
        }
    }
    if (capacity <= knap.get_capacity())
        return fitness;
    return fitness - (fitness * (capacity - knap.get_capacity()));
}

Individual roulette_selection(const std::vector<Individual>& population) {
    std::vector<double> fitnesses;
    fitnesses.reserve(population.size());

    for (const auto& ind : population) {
        fitnesses.push_back(static_cast<double>(ind.get_fitness()));
    }

    // Corrigir fitness se houver valores negativos
    double min_fitness = *std::min_element(fitnesses.begin(), fitnesses.end());
    if (min_fitness < 0) {
        for (auto& f : fitnesses)
            f += std::abs(min_fitness) + 1e-6;  // evita zero
    }

    // Recalcular total após possível shift
    double total_fitness = std::accumulate(fitnesses.begin(), fitnesses.end(), 0.0);

    // Normalizar
    for (auto& f : fitnesses)
        f /= total_fitness;

    // Acumular
    std::vector<double> cumulative(fitnesses.size());
    std::partial_sum(fitnesses.begin(), fitnesses.end(), cumulative.begin());

    // Sorteio
    std::uniform_real_distribution<> dist(0.0, 1.0);
    double r = dist(rng);

    for (std::size_t i = 0; i < cumulative.size(); ++i)
        if (r <= cumulative[i])
            return population[i];

    return population.back(); // fallback
}

Individual tournament_selection(const std::vector<Individual>& population, std::size_t size, double prob_victory) {
    std::set<std::size_t> indices;
    std::uniform_int_distribution<std::size_t> index_dist(0, population.size() - 1);

    while (indices.size() < size)
        indices.insert(index_dist(rng));

    std::vector<Individual> tournament;
    for (auto idx : indices)
        tournament.push_back(population[idx]);

    std::sort(tournament.begin(), tournament.end(), std::greater<>());

    std::uniform_real_distribution<> prob_dist(0, 1);
    return (prob_dist(rng) < prob_victory) ? tournament[0] : tournament.back();
}

Individual create_child(const Individual& p1, const Individual& p2, const Knapsack& knap, int parent_ratio, int mutate_probability) {
    std::uniform_int_distribution<int> chance(0, 100);
    if (chance(rng) > parent_ratio)
        return p1;

    std::string child_value;
    std::uniform_int_distribution<std::size_t> cut_dist(1, p1.get_value().size() - 2);
    std::size_t cut = cut_dist(rng);

    child_value = p1.get_value().substr(0, cut) + p2.get_value().substr(cut);

    for (char& c : child_value)
        if (chance(rng) < mutate_probability)
            c = (c == '1') ? '0' : '1';

    return Individual(knap, child_value);
}

} // namespace method

/* ----------- Population ----------- */
Population::Population(std::size_t pop_size, std::size_t parent_ratio, std::size_t mutate_probability,
                       std::size_t elite_ratio, int choice_selection_method, const Knapsack& knap)
    : m_generation(0), m_parent_ratio(parent_ratio),
      m_mutate_probability(mutate_probability), m_choice_selection_method(choice_selection_method), m_knapsack(knap) {

    m_elite_count = (elite_ratio * pop_size) / 100;
    m_population.reserve(pop_size);
    for (std::size_t i = 0; i < pop_size; ++i)
        m_population.emplace_back(knap, knap.get_weights().size());

    calculate_fitness_all();
}

void Population::calculate_fitness_all() {
    for (auto& ind : m_population) {
        long long fit = method::calculate_fitness(m_knapsack, ind.get_value());
        ind.set_fitness(fit);
    }
    std::sort(m_population.begin(), m_population.end(), std::greater<>());
}

void Population::evolve() {
    next_generation();
    m_generation++;
}

void Population::next_generation() {
    std::vector<Individual> new_population;
    new_population.reserve(m_population.size());

    // Elitismo
    for (std::size_t i = 0; i < m_elite_count; ++i)
        new_population.push_back(m_population[i]);

    while (new_population.size() < m_population.size()) {
        Individual p1;
        Individual p2;
        if(m_choice_selection_method == 1){
            p1 = method::tournament_selection(m_population, 3, 0.90);
            p2 = method::tournament_selection(m_population, 3, 0.90);
        }else{
            p1 = method::roulette_selection(m_population);
            p2 = method::roulette_selection(m_population);
        }
        Individual child = method::create_child(p1, p2, m_knapsack, m_parent_ratio, m_mutate_probability);
        child.set_fitness(method::calculate_fitness(m_knapsack, child.get_value()));
        new_population.push_back(child);
    }

    m_population = std::move(new_population);
    std::sort(m_population.begin(), m_population.end(), std::greater<>());
}

const Individual& Population::best_individual() const {
    return m_population.front();
}

std::size_t Population::get_generation() const {
    return m_generation;
}

const std::vector<Individual>& Population::get_individuals() const {
    return m_population;
}

// Função principal do algoritmo genético
DataResults genetic_algorithm(
    int max_generations,
    genetic_algorithm::Knapsack& knap,
    int choice_method_selection,
    std::size_t pop_size,
    std::size_t parent_ratio,
    std::size_t mutate_probability,
    std::size_t elite_ratio
){
    Population pop(pop_size, parent_ratio, mutate_probability, elite_ratio, choice_method_selection, knap);

    std::vector<std::size_t> fitness;
    DataResults results;
    while (max_generations--) {
        std::cout << pop.best_individual().get_fitness() << std::endl;
        results.best_individuos_per_generation.push_back(pop.best_individual());
        results.population_per_generation.push_back(pop.get_individuals());
        pop.evolve();
    }

    std::sort(results.best_individuos_per_generation.begin(), results.best_individuos_per_generation.end());

    return results;
}

} // namespace genetic_algorithm
