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

namespace details
{    
    // Classe que representará a mochila corrente da solução.
    class knapsack{
    public:
        knapsack(std::size_t capacity,
            std::size_t optimal_solution,
            std::vector<std::size_t> weights,
            std::vector<std::size_t> profits);
    
        std::size_t get_capacity() const{
            return this->capacity;
        }

        std::size_t get_optimal_solution() const{
            return this->optimal_solution;
        }

        std::vector<std::size_t> get_weights() const{
            return this->weights;
        }

        std::vector<std::size_t> get_profits() const{
            return this->profits;
        }
    
    private:
        std::size_t capacity;
        std::size_t optimal_solution;
        std::vector<std::size_t> weights;
        std::vector<std::size_t> profits;
    };
    // Gera uma string aleatória representando um indivíduo (combinação de itens na lista)
    std::string get_random_string(const size_t length);
    // Definição da função objetivo
    std::size_t calculate_fitness(std::string x);    
}

class individual{
public:
    individual() : m_value(""), m_fitness(0){};
    individual(const size_t length);
    individual(const std::string &value);

    // Acesso ao individuo
    std::string get_value() const {
        return this->m_value;
    }

    // Acesso ao fitness do individuo
    std::size_t get_fitness() const {
        return this-> m_fitness;
    }

    // Setar o fitness do individuo
    void set_fitness(std::size_t fintess){
        this-> m_fitness = fintess;
    }

    // Sobrecarga de operadores
    char operator[](size_t i) const{
        return this->m_value[i];
    }

    bool operator>(const individual &rhs) const {
        return this->m_fitness > rhs.m_fitness;
    }

    bool operator<(const individual &rhs) const {
        return this->m_fitness < rhs.m_fitness;
    }

    bool operator==(const individual &rhs) const {
        return this->m_fitness == rhs.m_fitness;
    }

private:
    // Inidividuo representado como um vetor de caractéres binários
    // (1) item pertence a mochila (0) item não pertence a mochila 
    std::string m_value;
    size_t m_fitness;  
};
} // namespace genetic_algorithm

#endif // GENETIC_ALGORITHM_HPP