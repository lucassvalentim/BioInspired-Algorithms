#include "genetic_algorithm.hpp"
#include <iostream>

namespace genetic_algorithm {

std::mt19937 rng(std::random_device{}());

namespace detail
{   
    std::vector<double> get_random_vector(const size_t length, const std::pair<double, double>& bound) {
        std::vector<double> individual;
        std::generate_n(std::back_inserter(individual), length, [&]() -> double {
            std::uniform_real_distribution<double> randomNumber(bound.first, bound.second);
            return randomNumber(rng);
        });
        return individual;
    }
}

individual::individual(std::size_t length, const std::pair<double, double>& bound)
    : m_value(detail::get_random_vector(length, bound)), m_fitness(0.0) {
    calculate_fitness(m_value);
}

individual::individual(const std::vector<double>& value)
    : m_value(value), m_fitness(0.0) {
    calculate_fitness(m_value);
}

void individual::calculate_fitness(const std::vector<double>& x) {
    int n = x.size();
    double somatorio1 = 0, somatorio2 = 0;
    for (int i = 0; i < n; i++) {
        somatorio1 += pow(x[i], 2);
        somatorio2 += cos(2 * M_PI * x[i]);
    }
    m_fitness = -20 * exp(-0.2 * sqrt(somatorio1 / n)) 
                - exp(somatorio2 / n) 
                + 20 + exp(1);
}

std::pair<individual, individual> create_child(const individual& X, const individual& Y, 
    const std::pair<double, double>& bound, double alpha, double beta, int mutate_probability) {
    
    std::uniform_int_distribution<int> randomMutation(1, 100);
    std::uniform_real_distribution<double> randomMutationBound(bound.first, bound.second);

    individual better = X;
    individual worse = Y;
    if (better.get_fitness() > worse.get_fitness())
        std::swap(better, worse);

    std::size_t length = better.get_value().size();
    std::vector<double> child1(length), child2(length);

    for (std::size_t i = 0; i < length; i++) {
        double d = std::abs(better[i] - worse[i]);
        if (randomMutation(rng) <= mutate_probability) {
            child1[i] = randomMutationBound(rng);
            child2[i] = randomMutationBound(rng);
        } else {
            double lower = std::min(better[i] - alpha*d, worse[i] - beta*d);
            double upper = std::max(better[i] + beta*d, worse[i] + alpha*d);
            std::uniform_real_distribution<double> range(lower, upper);
            child1[i] = range(rng);
            child2[i] = range(rng);
        }
    }

    return { individual(child1), individual(child2) };
}

individual roulette_method(const std::vector<individual>& m_population) {
    std::vector<double> fitness;
    double total_fitness = 0;

    // Calcula fitness invertido para problemas de minimização
    for (const auto& ind : m_population) {
        double inv_fitness = 1.0 / (ind.get_fitness() + 1e-8);
        fitness.push_back(inv_fitness);
        total_fitness += inv_fitness;
    }

    // Normaliza
    for (auto& f : fitness) {
        f /= total_fitness;
    }

    // Cria a roleta acumulada
    std::vector<double> roulette(fitness.size());
    roulette[0] = fitness[0];
    for (std::size_t i = 1; i < fitness.size(); i++) {
        roulette[i] = roulette[i-1] + fitness[i];
    }

    std::uniform_real_distribution<double> dis(0, 1);
    double rate_choice = dis(rng);

    for (std::size_t i = 0; i < roulette.size(); i++) {
        if (rate_choice <= roulette[i]) {
            return m_population[i];
        }
    }
    return m_population.back(); // fallback
}

std::vector<individual> selection_parents(const std::vector<individual>& m_population, std::size_t m_transfer_count){
    std::set<std::vector<double>> selected;
    std::vector<individual> parents;
    parents.reserve(m_population.size() - m_transfer_count);

    const int max_attempts = 10;

    while (parents.size() < m_population.size() - m_transfer_count){
        individual candidate = roulette_method(m_population);
        int attempts = 0;
        while (selected.count(candidate.get_value()) && attempts < max_attempts) {
            candidate = roulette_method(m_population);
            attempts++;
        }
        // Se não conseguir diferente, aceita repetido
        selected.insert(candidate.get_value());
        parents.push_back(candidate);
    }
    return parents;
}


population::population(std::size_t length_population, std::size_t parent_ratio, std::size_t mutate_probability, 
                       std::size_t transfer_elite_ratio, const std::pair<double, double>& bound, std::size_t length)
    : m_generation(1), m_parent_ratio(parent_ratio), m_mutate_probability(mutate_probability) {
    
    m_transfer_count = (transfer_elite_ratio * length_population) / 100;
    m_new_individuals_per_generation = length_population - m_transfer_count;
    m_population.reserve(length_population);

    std::generate_n(std::back_inserter(m_population), length_population, [&]() {
        return individual(length, bound);
    });
    sort();
}

void population::create_next_generation(const std::pair<double, double>& bound) {
    m_generation++;

    std::vector<individual> next_generation;
    next_generation.reserve(m_population.size());

    // Mantém a elite
    for (std::size_t i = 0; i < m_transfer_count; i++) {
        next_generation.push_back(m_population[i]);
    }

    // Gera o restante
    std::vector<individual> parents = selection_parents(m_population, m_transfer_count);

    if (parents.size() % 2 != 0) {
        parents.push_back(m_population.front());
    }

    std::uniform_int_distribution<std::size_t> parentsRatio(1, 100);

    for (std::size_t i = 0; i < parents.size(); i += 2) {
        if (parentsRatio(rng) <= m_parent_ratio) {
            auto [child1, child2] = create_child(parents[i], parents[i+1], bound, 0.75, 0.25, m_mutate_probability);
            next_generation.push_back(child1);
            next_generation.push_back(child2);
        } else {
            next_generation.push_back(parents[i]);
            next_generation.push_back(parents[i+1]);
        }
    }

    // Atualiza a população
    m_population = std::move(next_generation);
    sort();
}

std::vector<double> genetic_algorithm(
    int dimensions,
    const std::pair<double, double>& bounds,
    size_t population_size,
    size_t parent_ratio,
    size_t mutation_probability,
    size_t elite_ratio,
    int max_generations) {
    
    population pop(population_size, parent_ratio, mutation_probability, elite_ratio, bounds, dimensions);

    std::vector<double> fitness;
    while (max_generations--) {
        fitness.push_back(pop.front().get_fitness());
        pop.create_next_generation(bounds);
    }

    return fitness;
}
} // namespace genetic_algorithm