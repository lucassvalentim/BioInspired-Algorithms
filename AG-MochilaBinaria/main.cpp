#include "genetic_algorithm.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <numeric>   // para std::accumulate
#include <cmath>     // para std::sqrt
#include <tuple>     // para std::tuple
#include <algorithm> // para std::min_element, std::max_element
#include <fstream>
#include <string>

// Função para ler a entrada;
void reading_files(
    char *capacidadeTxt, 
    char *profitsTxt, 
    char *optimalSolutionTxt, 
    char *weightsTxt, 
    std::size_t &capacidade,
    std::string &optimalSolution,
    std::vector<std::size_t> &profits,
    std::vector<std::size_t> &weights
){
    std::ifstream cTxt(capacidadeTxt);
    std::ifstream pTxt(profitsTxt);
    std::ifstream sTxt(optimalSolutionTxt);
    std::ifstream wTxt(weightsTxt);

    if(!cTxt.is_open() || !pTxt.is_open() || !sTxt.is_open() || !wTxt.is_open()){
        std::cout << "Erro ao tentar abrir um dos arquivos" << std::endl;
        exit(1);
    }
    // Leitura da capacidade
    while (cTxt >> capacidade);

    // Leitura da solução ótima;
    std::string t;
    while(sTxt >> t){
        optimalSolution += t;
    }

    // Leitura dos valores de cada item;
    std::size_t p;
    while(pTxt >> p){
        profits.push_back(p);
    }

    // Leitura dos pesos de cada item;
    std::size_t w;
    while(wTxt >> w){
        weights.push_back(w);        
    }
}

int main(int argc, char **argv){
    
    if(argc != 5){
        std::cout << "./<executavel> <capacidade.txt> <profits.txt> <optimalSolution.txt> <weights.txt>" << std::endl;
    }

    
    // Leitura dos arquivos
    std::size_t capacidade;
    std::string optimalSolution;
    std::vector<std::size_t> profits;
    std::vector<std::size_t> weights;

    reading_files(argv[1], argv[2], argv[3], argv[4], capacidade, optimalSolution, profits, weights);

    
    // Criação da mochila e impressão dos dados
    genetic_algorithm::Knapsack knap(capacidade, optimalSolution, weights, profits);
    
    std::size_t max_generation = 100;
    std::size_t pop_size = 500;
    std::size_t parent_ratio = 100;
    std::size_t mutate_probability = 2;
    std::size_t elite_ratio = 2;
    int choice_method_selection = 1;

    genetic_algorithm::DataResults results = genetic_algorithm::genetic_algorithm(max_generation, knap, choice_method_selection, pop_size, parent_ratio, mutate_probability, elite_ratio);
    
    for(auto c :  results.best_individuos_per_generation){
        std::cout << c.get_fitness() << std::endl;
    }

    std::cout << results.best_individuos_per_generation[results.best_individuos_per_generation.size() - 1].get_fitness() << std::endl;
    
    return 0;
}