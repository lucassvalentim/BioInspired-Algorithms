#include "genetic_algorithm.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

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
    
    while (cTxt >> capacidade);

    std::string t;
    while(sTxt >> t){
        optimalSolution += t;
    }

    std::size_t p;
    while(pTxt >> p){
        profits.push_back(p);
    }

    std::size_t w;
    while(wTxt >> w){
        weights.push_back(w);        
    }
}

void save_generation_data(int run, int generation, const std::vector<genetic_algorithm::Individual>& population, std::ofstream& outfile) {
    outfile << run << "," << generation;
    for (const auto& ind : population) {
        outfile << "," << ind.get_fitness();
    }
    outfile << "\n";
}

int main(int argc, char **argv){
    if(argc != 5){
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
    
    // Parâmetros do AG
    const std::size_t max_generation = 20;
    const std::size_t pop_size = 50;
    const std::size_t parent_ratio = 50;
    const std::size_t mutate_probability = 2;
    const std::size_t elite_ratio = 5;
    const int choice_method_selection = 2;
    const int total_runs = 20;

    // Arquivo de saída
    std::ofstream outfile("resultados/convergence_data.csv");
    if (!outfile.is_open()) {
        std::cerr << "Erro ao criar arquivo de saída" << std::endl;
        return 1;
    }

    // Cabeçalho do CSV
    outfile << "Run,Generation";
    for (size_t i = 0; i < pop_size; ++i) {
        outfile << ",Ind" << i;
    }
    outfile << "\n";

    // Executar o AG 20 vezes
    for (int run = 0; run < total_runs; ++run) {
        genetic_algorithm::Population pop(pop_size, parent_ratio, mutate_probability, 
                                        elite_ratio, choice_method_selection, knap);
        
        // Salvar dados da população inicial
        save_generation_data(run, 0, pop.get_individuals(), outfile);

        // Evoluir por N gerações
        for (std::size_t gen = 1; gen <= max_generation; ++gen) {
            pop.evolve();
            save_generation_data(run, gen, pop.get_individuals(), outfile);
            
            // Progresso
            if (gen % 5 == 0) {
                std::cout << "Run " << run+1 << "/" << total_runs 
                          << ", Generation " << gen << "/" << max_generation 
                          << ", Best: " << pop.best_individual().get_fitness() << std::endl;
            }
        }
    }

    outfile.close();
    std::cout << "Dados salvos em convergence_data.csv" << std::endl;
    
    return 0;
}