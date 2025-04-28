#include "genetic_algorithm.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <future>    // para std::async e std::future
#include <numeric>   // para std::accumulate
#include <cmath>     // para std::sqrt
#include <tuple>     // para std::tuple
#include <algorithm> // para std::min_element, std::max_element

// Função para calcular a média
double mean(const std::vector<double>& data) {
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    return sum / static_cast<double>(data.size());
}

// Função para calcular o desvio padrão
double standard_deviation(const std::vector<double>& data, double data_mean) {
    double sum = 0.0;
    for (const auto& value : data) {
        sum += (value - data_mean) * (value - data_mean);
    }
    return std::sqrt(sum / static_cast<double>(data.size()));
}

int main() {
    // Definições gerais
    int dimensions = 3;
    std::pair<double, double> bounds = {-2.0, 2.0};

    // Parâmetros a variar
    std::vector<int> mutation_rates = {1, 5, 10};          // Taxa de mutação (%)
    std::vector<int> crossover_rates = {60, 80, 100};      // Taxa de cruzamento (%)
    std::vector<int> population_sizes = {50, 100, 500};    // Tamanho da população
    std::vector<int> num_generations = {50, 100, 300};     // Número de gerações
    std::vector<int> elitism_rates = {5, 25, 50};          // Taxa de elitismo (%)

    // Arquivo CSV de saída
    std::ofstream csv_file("../resultados/resultados_fatoriais.csv");
    csv_file << "MutationRate,CrossoverRate,PopulationSize,NumGenerations,ElitismRate,"
             << "MeanBestFitness,StdDevBestFitness,MeanGlobalFitness,StdDevGlobalFitness\n";

    // Para salvar o melhor conjunto encontrado
    double best_overall_mean = std::numeric_limits<double>::max();
    std::tuple<int, int, int, int, int> best_params;
    std::vector<std::vector<double>> best_runs;

    int combination_counter = 0;

    // Loop de experimentos fatoriais
    for (int mutation : mutation_rates) {
        for (int crossover : crossover_rates) {
            for (int pop_size : population_sizes) {
                for (int generations : num_generations) {
                    for (int elitism : elitism_rates) {
                        
                        combination_counter++;
                        std::cout << "\nTestando combinação " << combination_counter
                                  << ": mutation=" << mutation
                                  << ", crossover=" << crossover
                                  << ", pop_size=" << pop_size
                                  << ", generations=" << generations
                                  << ", elitism=" << elitism
                                  << std::endl;

                        // Executar 10 vezes em paralelo
                        std::vector<std::future<std::vector<double>>> futures;

                        for (int i = 0; i < 10; ++i) {
                            futures.push_back(std::async(std::launch::async, [=]() {
                                return genetic_algorithm::genetic_algorithm(
                                    dimensions,
                                    bounds,
                                    pop_size,
                                    crossover,
                                    mutation,
                                    elitism,
                                    generations
                                );
                            }));
                        }

                        // Receber os resultados
                        std::vector<std::vector<double>> all_results(10);
                        for (int i = 0; i < 10; ++i) {
                            all_results[i] = futures[i].get();
                        }

                        // Coletar dados para análise
                        std::vector<double> best_fitness_each_run;
                        std::vector<double> mean_fitness_each_run;

                        for (const auto& run : all_results) {
                            std::vector<double> temp = run;
                            std::sort(temp.begin(), temp.end());
                            best_fitness_each_run.push_back(temp.front());  // menor fitness = melhor
                            mean_fitness_each_run.push_back(mean(temp));    // média dos fitness
                        }

                        // Calcula métricas
                        double mean_best = mean(best_fitness_each_run);
                        double std_best = standard_deviation(best_fitness_each_run, mean_best);
                        double mean_global = mean(mean_fitness_each_run);
                        double std_global = standard_deviation(mean_fitness_each_run, mean_global);

                        // Escreve no CSV
                        csv_file << mutation << ","
                                 << crossover << ","
                                 << pop_size << ","
                                 << generations << ","
                                 << elitism << ","
                                 << mean_best << ","
                                 << std_best << ","
                                 << mean_global << ","
                                 << std_global << "\n";

                        // Atualiza melhor conjunto, se necessário
                        if (mean_best < best_overall_mean) {
                            best_overall_mean = mean_best;
                            best_params = {mutation, crossover, pop_size, generations, elitism};
                            best_runs = all_results;
                        }
                    }
                }
            }
        }
    }

    csv_file.close();
    std::cout << "\nExperimentos completos. Resultados salvos em 'resultados_fatoriais.csv'.\n";

    // Salvar detalhes da melhor configuração encontrada
    std::ofstream best_file("../resultados/melhor_execucao.txt");
    best_file << "Melhor configuração encontrada:\n";
    best_file << "Taxa de mutação: " << std::get<0>(best_params) << "%\n";
    best_file << "Taxa de cruzamento: " << std::get<1>(best_params) << "%\n";
    best_file << "Tamanho da população: " << std::get<2>(best_params) << "\n";
    best_file << "Número de gerações: " << std::get<3>(best_params) << "\n";
    best_file << "Taxa de elitismo: " << std::get<4>(best_params) << "%\n";
    best_file << "\nResultados das 10 execuções:\n";

    for (size_t i = 0; i < best_runs.size(); ++i) {
        best_file << "Execução " << i + 1 << ": ";
        for (size_t j = 0; j < best_runs[i].size(); ++j) {
            best_file << best_runs[i][j];
            if (j != best_runs[i].size() - 1) best_file << ", ";
        }
        best_file << "\n";
    }
    best_file.close();
    std::cout << "Melhor execução detalhada salva em 'melhor_execucao.txt'.\n";

    return 0;
}
