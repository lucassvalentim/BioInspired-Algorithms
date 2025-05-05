#include <bits/stdc++.h>
using std::cout;
using std::endl;

// Gerador de números aleatórios
std::mt19937 rng(std::random_device{}());

namespace algorithm
{
    namespace detail
    {   
        // Gera um vetor de valores aleatórios dentro dos limites especificados
        auto get_random_vector(const size_t length, const std::pair<double, double>& bound){
            std::vector<double> individual;
            std::generate_n(std::back_inserter(individual), length, [&]() -> double {
                std::uniform_real_distribution<double> randomNumber(bound.first, bound.second);
                return randomNumber(rng);
            });
            return individual;
        }
    }

    class individual {
    public:
        // Construtor padrão
        individual() : m_value(0), m_fitness(0.0) {}

        // Construtor que gera vetor aleatório
        individual(std::size_t length, const std::pair<double, double>& bound)
            : m_value(detail::get_random_vector(length, bound)), m_fitness(0.0){
            calculate_fitness(m_value);
        }

        // Construtor que recebe um vetor pronto
        individual(const std::vector<double>& value, const std::pair<double, double>& bound)
            : m_value(value), m_fitness(0.0){
            calculate_fitness(m_value);
        }

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
            return m_value == rhs.m_value; // melhor comparação pelo vetor
        }

    private:
        // Calcula o fitness usando a função de Ackley
        void calculate_fitness(const std::vector<double>& x){
            int n = x.size();
            double somatorio1 = 0, somatorio2 = 0;
            for (int i = 0; i < n; i++){
                somatorio1 += pow(x[i], 2);
                somatorio2 += cos(2 * M_PI * x[i]);
            }
            m_fitness = -20 * exp(-0.2 * sqrt(somatorio1 / n)) 
                        - exp(somatorio2 / n) 
                        + 20 + exp(1);
        }

        std::vector<double> m_value;
        double m_fitness;
    };

    // Criação de dois filhos a partir de dois pais (crossover + possível mutação)
    std::pair<individual, individual> create_child(const individual& X, const individual& Y, const std::pair<double, double>& bound, double alpha, double beta, int mutate_probability){
        std::uniform_int_distribution<int> randomMutation(1, 100);
        std::uniform_real_distribution<double> randomMutationBound(bound.first, bound.second);

        individual better = X;
        individual worse = Y;
        if (better.get_fitness() > worse.get_fitness())
            std::swap(better, worse);

        std::size_t length = better.get_value().size();
        std::vector<double> child1(length), child2(length);

        for (std::size_t i = 0; i < length; i++){
            double d = std::abs(better[i] - worse[i]);
            if (randomMutation(rng) <= mutate_probability){
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

        return { individual(child1, bound), individual(child2, bound) };
    }

    // Método da roleta para seleção
    individual roulette_method(const std::vector<individual>& m_population){
        std::vector<double> fitness;
        double total_fitness = 0;

        // Calcula fitness invertido para problemas de minimização
        for (const auto& ind : m_population){
            double inv_fitness = 1.0 / (ind.get_fitness() + 1e-8);
            fitness.push_back(inv_fitness);
            total_fitness += inv_fitness;
        }

        // Normaliza
        for (auto& f : fitness){
            f /= total_fitness;
        }

        // Cria a roleta acumulada
        std::vector<double> roulette(fitness.size());
        roulette[0] = fitness[0];
        for (std::size_t i = 1; i < fitness.size(); i++){
            roulette[i] = roulette[i-1] + fitness[i];
        }

        std::uniform_real_distribution<double> dis(0, 1);
        double rate_choice = dis(rng);

        for (std::size_t i = 0; i < roulette.size(); i++){
            if (rate_choice <= roulette[i]){
                return m_population[i];
            }
        }
        return m_population.back(); // fallback
    }

    // Seleção de pais únicos
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
    

    class population{
    public:
        population(std::size_t length_population, std::size_t parent_ratio, std::size_t mutate_probability, std::size_t transfer_elite_ratio, const std::pair<double, double>& bound, std::size_t length)
            : m_generation(1), m_parent_ratio(parent_ratio), m_mutate_probability(mutate_probability)
        {
            m_transfer_count = (transfer_elite_ratio * length_population) / 100; // corrigido
            m_new_individuals_per_generation = length_population - m_transfer_count;
            m_population.reserve(length_population);

            std::generate_n(std::back_inserter(m_population), length_population, [&]() {
                return individual(length, bound);
            });
            sort();
        }

        std::size_t get_generation() const {
            return m_generation;
        }

        const std::vector<individual>& get_pop() const {
            return m_population;
        }

        void sort(){
            std::sort(m_population.begin(), m_population.end());
        }

        void create_next_generation(const std::pair<double, double>& bound){
            m_generation++;

            std::vector<individual> next_generation;
            next_generation.reserve(m_population.size());

            // Mantém a elite
            for (std::size_t i = 0; i < m_transfer_count; i++){
                next_generation.push_back(m_population[i]);
            }

            // Gera o restante
            std::vector<individual> parents = selection_parents(m_population, m_transfer_count);

            if (parents.size() % 2 != 0){
                parents.push_back(m_population.front());
            }

            std::uniform_int_distribution<int> parentsRatio(1, 100);

            for (std::size_t i = 0; i < parents.size(); i += 2){
                if (parentsRatio(rng) <= m_parent_ratio){
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
}

int main() {
    int length = 3;
    std::pair<double, double> bound = {-10.0, 10.0};
    const std::size_t length_population = 100;
    const std::size_t parent_ratio = 90; // % chance de cruzar
    const std::size_t mutate_probability = 10; // % chance de mutação
    const std::size_t transfer_elite_ratio = 20; // % da população é elite

    algorithm::population pop(length_population, parent_ratio, mutate_probability, transfer_elite_ratio, bound, length);

    int max_iterations = 300;
    while (max_iterations--){
        cout << pop.front().get_fitness() << endl;
        pop.create_next_generation(bound);
    }

    return 0;
}