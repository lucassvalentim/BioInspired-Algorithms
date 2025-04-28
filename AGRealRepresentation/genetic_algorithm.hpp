#ifndef GENETIC_ALGORITHM_HPP
#define GENETIC_ALGORITHM_HPP

#include <vector>
#include <utility>
#include <random>
#include <algorithm>
#include <cmath>
#include <set>

namespace genetic_algorithm {

// Gerador de números aleatórios
extern std::mt19937 rng;

namespace detail
{   
    // Gera um vetor de valores aleatórios dentro dos limites especificados
    std::vector<double> get_random_vector(const size_t length, const std::pair<double, double>& bound);
}

class individual {
public:
    // Construtor padrão
    individual() : m_value(0), m_fitness(0.0) {}

    // Construtor que gera vetor aleatório
    individual(std::size_t length, const std::pair<double, double>& bound);

    // Construtor que recebe um vetor pronto
    individual(const std::vector<double>& value, const std::pair<double, double>& bound);

    // Acesso ao vetor de valores
    std::vector<double> get_value() const {
        return m_value;
    }

    // Acesso ao fitness
    double get_fitness() const {
        return m_fitness;
    }

    // Sobrecarga de operadores
    double operator[](int i) const {
        return m_value[i];
    }

    bool operator>(const individual& rhs) const {
        return m_fitness > rhs.m_fitness;
    }

    bool operator<(const individual& rhs) const {
        return m_fitness < rhs.m_fitness;
    }

    bool operator==(const individual& rhs) const {
        return m_value == rhs.m_value;
    }

private:
    // Calcula o fitness usando a função de Ackley
    void calculate_fitness(const std::vector<double>& x);

    std::vector<double> m_value;
    double m_fitness;
};

// Criação de dois filhos a partir de dois pais (crossover + possível mutação)
std::pair<individual, individual> create_child(const individual& X, const individual& Y, 
    const std::pair<double, double>& bound, double alpha, double beta, int mutate_probability);

// Método da roleta para seleção
individual roulette_method(const std::vector<individual>& m_population);

// Seleção de pais únicos
std::vector<individual> selection_parents(const std::vector<individual>& m_population, std::size_t m_transfer_count);

class population {
public:
    population(std::size_t length_population, std::size_t parent_ratio, std::size_t mutate_probability, 
               std::size_t transfer_elite_ratio, const std::pair<double, double>& bound, std::size_t length);

    std::size_t get_generation() const {
        return m_generation;
    }

    const std::vector<individual>& get_pop() const {
        return m_population;
    }

    void sort() {
        std::sort(m_population.begin(), m_population.end());
    }

    void create_next_generation(const std::pair<double, double>& bound);

    const individual& front() const {
        return m_population.front();
    }

private:
    std::vector<individual> m_population;
    std::size_t m_generation;
    std::size_t m_parent_ratio;
    std::size_t m_mutate_probability;
    std::size_t m_transfer_count;
    std::size_t m_new_individuals_per_generation;
};

// Função principal do algoritmo genético
std::vector<double> genetic_algorithm(
    int dimensions,
    const std::pair<double, double>& bounds,
    size_t population_size = 500,
    size_t parent_ratio = 90,
    size_t mutation_probability = 10,
    size_t elite_ratio = 2,
    int max_generations = 100,
    bool verbose = false
);

} // namespace genetic_algorithm

#endif // GENETIC_ALGORITHM_HPP