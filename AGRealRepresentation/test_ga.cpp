#include "genetic_algorithm.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm> // para std::sort
#include <string>
#include <iomanip>   // para std::setprecision
#include <utility>   // para std::pair

void save_fitness_per_run(const std::vector<double>& fitnesses, int run_number) {
    std::ofstream file("fitness_run_" + std::to_string(run_number) + ".txt");
    
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir arquivo para salvar a execução " << run_number << std::endl;
        return;
    }

    for (size_t i = 0; i < fitnesses.size(); ++i) {
        file << i << " " << std::fixed << std::setprecision(20) << fitnesses[i] << std::endl;
    }

    file.close();
}

int main() {
    // ==================================
    // Parâmetros do algoritmo genético
    // ==================================
    int dimensions = 2;
    std::pair<double, double> bounds = {-2.0, 2.0};
    int population_size = 100;
    int parent_ratio = 90;         // percentual de pais
    int mutation_probability = 10; // percentual de mutação
    int elite_ratio = 10;           // percentual de elitismo
    int max_generations = 100;

    int num_execucoes = 10;          // Número de execuções para obter estatísticas

    // Vetor para armazenar o melhor fitness de cada execução
    std::vector<double> melhores_fitness;

    for (int execucao = 0; execucao < num_execucoes; ++execucao) {
        std::cout << "Execução " << execucao + 1 << " de " << num_execucoes << std::endl;

        // Executa o algoritmo genético
        auto fitnesses = genetic_algorithm::genetic_algorithm(
            dimensions,
            bounds,
            population_size,
            parent_ratio,
            mutation_probability,
            elite_ratio,
            max_generations
        );

        // Salva a evolução do fitness desta execução
        save_fitness_per_run(fitnesses, execucao);

        // Encontra o melhor fitness desta execução (menor valor)
        auto melhor_fitness = *std::min_element(fitnesses.begin(), fitnesses.end());
        melhores_fitness.push_back(melhor_fitness);
    }

    // Salva os melhores fitness de cada execução em um arquivo resumo
    std::ofstream file_summary("melhores_fitness.txt");
    if (file_summary.is_open()) {
        for (int i = 0; i < num_execucoes; ++i) {
            file_summary << i << " " << std::fixed << std::setprecision(6) << melhores_fitness[i] << std::endl;
        }
        file_summary.close();
    } else {
        std::cerr << "Erro ao abrir o arquivo melhores_fitness.txt" << std::endl;
    }

    std::cout << "Todas as execuções foram finalizadas com sucesso." << std::endl;

    return 0;
}
