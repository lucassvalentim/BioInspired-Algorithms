#include "genetic_algorithm.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <tuple>
#include <algorithm>
#include <iomanip>
#include <map>

// Função para ler a entrada
void reading_files(
    char *capacidadeTxt, 
    char *profitsTxt, 
    char *optimalSolutionTxt, 
    char *weightsTxt, 
    std::size_t &capacidade,
    std::string &optimalSolution,
    std::vector<std::size_t> &profits,
    std::vector<std::size_t> &weights
) {
    std::ifstream cTxt(capacidadeTxt);
    std::ifstream pTxt(profitsTxt);
    std::ifstream sTxt(optimalSolutionTxt);
    std::ifstream wTxt(weightsTxt);

    if(!cTxt.is_open() || !pTxt.is_open() || !sTxt.is_open() || !wTxt.is_open()) {
        std::cout << "Erro ao tentar abrir um dos arquivos" << std::endl;
        exit(1);
    }
    // Leitura da capacidade
    while (cTxt >> capacidade);

    // Leitura da solução ótima;
    char t;
    while(sTxt >> t) {
        optimalSolution += t;
    }

    // Leitura dos valores de cada item;
    std::size_t p;
    while(pTxt >> p) {
        profits.push_back(p);
    }

    // Leitura dos pesos de cada item;
    std::size_t w;
    while(wTxt >> w) {
        weights.push_back(w);        
    }
}

// Função para calcular o fitness da solução ótima
long long calculate_optimal_fitness(const genetic_algorithm::Knapsack& knap) {
    long long optimal_fitness = 0;
    for(std::size_t i = 0; i < knap.get_optimal_solution().size(); i++) {
        if(knap.get_optimal_solution()[i] == '1') {
            optimal_fitness += knap.get_profits()[i];
        }
    }
    return optimal_fitness;
}

// Função para executar o algoritmo genético com diferentes parâmetros
void run_parameter_combination(
    genetic_algorithm::Knapsack& knap,
    std::size_t pop_size,
    std::size_t max_generations,
    std::size_t parent_ratio,
    std::size_t mutate_probability,
    std::size_t elite_ratio,
    int choice_method_selection,
    std::ofstream& output_file,
    long long optimal_fitness
) {
    double best_fitness = 0.0;
    double sum_fitness = 0.0;
    double sum_squared_fitness = 0.0;
    
    // Calcular o número de execuções para manter pop_size * executions = 10000
    std::size_t executions = 20;
    
    for (std::size_t i = 0; i < executions; ++i) {
        auto results = genetic_algorithm::genetic_algorithm(
            max_generations, knap, choice_method_selection,
            pop_size, parent_ratio, mutate_probability, elite_ratio
        );
        
        double final_fitness = results.best_individuos_per_generation.back().get_fitness();
        sum_fitness += final_fitness;
        sum_squared_fitness += final_fitness * final_fitness;
        
        if (final_fitness > best_fitness) {
            best_fitness = final_fitness;
        }
    }
    
    double avg_fitness = sum_fitness / executions;
    double variance = (sum_squared_fitness / executions) - (avg_fitness * avg_fitness);
    double std_deviation = sqrt(variance);
    double diff_from_optimal = optimal_fitness - best_fitness;
    
    // Escrever resultados no arquivo
    output_file << pop_size << ","
                << max_generations << ","
                << parent_ratio << ","
                << mutate_probability << ","
                << elite_ratio << ","
                << choice_method_selection << ","
                << executions << ","
                << std::fixed << std::setprecision(2)
                << avg_fitness << ","
                << best_fitness << ","
                << std_deviation << ","
                << optimal_fitness << ","
                << diff_from_optimal << "\n";
}

int main(int argc, char **argv) {
    if(argc != 5) {
        std::cout << "./<executavel> <capacidade.txt> <profits.txt> <optimalSolution.txt> <weights.txt>" << std::endl;
        return 1;
    }
    
    // Leitura dos arquivos
    std::size_t capacidade;
    std::string optimalSolution;
    std::vector<std::size_t> profits;
    std::vector<std::size_t> weights;

    reading_files(argv[1], argv[2], argv[3], argv[4], capacidade, optimalSolution, profits, weights);
    
    // Criação da mochila
    genetic_algorithm::Knapsack knap(capacidade, optimalSolution, weights, profits);
    
    // Calcular fitness da solução ótima
    long long optimal_fitness = calculate_optimal_fitness(knap);
    std::cout << "Fitness da solução ótima: " << optimal_fitness << std::endl;

    // Configurações de parâmetros a serem testados
    std::map<std::size_t,std::size_t> pop_sizes_with_max_generations = {{25, 40}, {50, 20}, {100, 10}}; 
    std::vector<std::size_t> parent_ratios = {50, 70, 100}; // Taxa de cruzamento (%)
    std::vector<std::size_t> mutate_probabilities = {1, 2, 5}; // Taxa de mutação (%)
    std::vector<std::size_t> elite_ratios = {1, 2, 5}; // Elitismo (%)
    std::vector<int> selection_methods = {1, 2}; // 1 = torneio, 2 = roleta
    
    // Abrir arquivo de saída
    std::ofstream output_file("resultados/results.csv");
    if (!output_file.is_open()) {
        std::cerr << "Erro ao abrir arquivo de resultados." << std::endl;
        return 1;
    }
    
    // Cabeçalho do CSV atualizado
    output_file << "PopSize,Generations,ParentRatio,MutateProb,EliteRatio,SelectionMethod,Executions,AvgFitness,BestFitness,StdDeviation,OptimalFitness,DifferenceFromOptimal\n";
    
    // Executar combinações de parâmetros
    for (const auto& pop_size : pop_sizes_with_max_generations) {
        for (auto parent_ratio : parent_ratios) {
            for (auto mutate_probability : mutate_probabilities) {
                for (auto elite_ratio : elite_ratios) {
                    for (auto selection_method : selection_methods) {
                        run_parameter_combination(
                            knap, pop_size.first, pop_size.second, parent_ratio,
                            mutate_probability, elite_ratio, selection_method,
                            output_file, optimal_fitness
                        );
                        
                        // Mostrar progresso
                        std::cout << "Executado: PopSize=" << pop_size.first 
                                  << ", Gen=" << pop_size.second
                                  << ", Cross=" << parent_ratio
                                  << ", Mut=" << mutate_probability
                                  << ", Elite=" << elite_ratio
                                  << ", Sel=" << selection_method
                                  << std::endl;
                    }
                }
            }
        }
    }
    
    output_file.close();
    std::cout << "Todos os testes concluídos. Resultados salvos em results.csv" << std::endl;
    
    return 0;
}