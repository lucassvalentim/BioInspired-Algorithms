#include "genetic_algorithm.hpp"
#include <iostream>

namespace genetic_algorithm{
    std::mt19937 rng(std::random_device{}());

// Instanciando funções de details
namespace details{
    // Instanciação da mochila corrente de solução;
    knapsack::knapsack(std::size_t capacity, std::size_t optimal_solution,
    std::vector<std::size_t> weights, std::vector<std::size_t> profits){
        this->capacity = capacity;
        this->optimal_solution = optimal_solution;
        this->weights = weights;
        this->profits = profits;
    }

    std::string get_random_string(const size_t length){
        std::string individual(length, 0);
        std::generate_n(individual.begin(), length, [&]() -> char {
            std::string aux = "01";
            std::uniform_int_distribution<int> random(0, 1);
            return aux[random(rng)];
        });

        return individual;
    }

    std::size_t calculate_fitness(knapsack knap, std::string x){
        std::size_t capacity = 0;
        std::size_t fitness = 0;
        for(std::size_t i = 0; i < x.size(); i++){
            if(x[i] == '1') {
                capacity += knap.get_weights()[i];
                fitness += knap.get_profits()[i]; 
            }
        }

        if(capacity <= knap.get_capacity()){
            return fitness;
        }
        
        return fitness - (fitness * (capacity - knap.get_capacity()));
    }
}

// Instanciando individuos
individual::individual(const size_t length){
    this->m_value = details::get_random_string(length);
    this->m_fitness = 0;
}
individual::individual(const std::string &value){
    this->m_value = value;
}



}