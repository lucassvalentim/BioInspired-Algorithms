#include <bits/stdc++.h>
#include <chrono>
#include <fstream>
#include <numeric>
using namespace std;

#define ll long long
#define endl '\n'

random_device rd;
mt19937 generator(rd());

enum CrossoverType { OX, CX };

struct Individual {
    vector<int> path;
    double fitness;
    double cost;
    
    Individual() : fitness(0.0), cost(0.0) {}
    Individual(vector<int> p) : path(p), fitness(0.0), cost(0.0) {}
};

struct TestConfig {
    int popSize;
    int maxGenerations;
    double crossRate;
    double mutationRate;
    CrossoverType crossoverType;
    double bestCost;
    double executionTime;
};

vector<vector<double>> readGraph(int vertexCount) {
    vector<vector<double>> graph(vertexCount, vector<double>(vertexCount));
    
    ifstream matriz_txt("input/entrada.txt");
    if(!matriz_txt){
        cout << "Erro ao abrir o arquivo" << endl;
        exit(1);
    }

    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            matriz_txt >> graph[i][j];
        }
    }
    return graph;
}

double calculateCost(const vector<vector<double>>& graph, const vector<int>& path) {
    double totalCost = 0;
    int n = path.size();
    for (int i = 0; i < n - 1; i++) {
        totalCost += graph[path[i]][path[i + 1]];
    }
    totalCost += graph[path[n-1]][path[0]];
    return totalCost;
}

vector<Individual> generateInitialPopulation(const vector<vector<double>>& graph, int vertexCount, int popSize) {
    vector<Individual> population;
    population.reserve(popSize);

    while (population.size() < popSize) {
        vector<int> path(vertexCount);
        iota(path.begin(), path.end(), 0);
        shuffle(path.begin(), path.end(), generator);
        population.emplace_back(path);
    }

    return population;
}

void evaluatePopulation(const vector<vector<double>>& graph, vector<Individual>& population) {
    for (auto& individual : population) {
        individual.cost = calculateCost(graph, individual.path);
        individual.fitness = 1.0 / (1.0 + individual.cost);
    }
}

Individual tournamentSelection(const vector<Individual>& population, int tournamentSize = 3) {
    uniform_int_distribution<int> dis(0, population.size() - 1);
    
    Individual best = population[dis(generator)];
    for (int i = 1; i < tournamentSize; i++) {
        Individual candidate = population[dis(generator)];
        if (candidate.fitness > best.fitness) {
            best = candidate;
        }
    }
    return best;
}

pair<Individual, Individual> orderCrossover(const Individual& parent1, const Individual& parent2) {
    int n = parent1.path.size();
    uniform_int_distribution<int> dis(0, n - 1);
    
    int start = dis(generator);
    int end = dis(generator);
    if (start > end) swap(start, end);
    
    Individual child1, child2;
    child1.path.resize(n, -1);
    child2.path.resize(n, -1);
    
    for (int i = start; i <= end; i++) {
        child1.path[i] = parent1.path[i];
        child2.path[i] = parent2.path[i];
    }
    
    auto fillChild = [&](Individual& child, const Individual& parent, const Individual& otherParent) {
        unordered_set<int> used;
        for (int i = start; i <= end; i++) {
            used.insert(child.path[i]);
        }
        
        int pos = (end + 1) % n;
        for (int i = 0; i < n; i++) {
            int idx = (end + 1 + i) % n;
            if (used.find(otherParent.path[idx]) == used.end()) {
                while (child.path[pos] != -1) {
                    pos = (pos + 1) % n;
                }
                child.path[pos] = otherParent.path[idx];
                used.insert(otherParent.path[idx]);
            }
        }
    };
    
    fillChild(child1, parent1, parent2);
    fillChild(child2, parent2, parent1);
    
    return {child1, child2};
}

pair<Individual, Individual> cycleCrossover(const Individual& parent1, const Individual& parent2) {
    int n = parent1.path.size();
    Individual child1, child2;
    child1.path.resize(n, -1);
    child2.path.resize(n, -1);
    
    vector<bool> visited(n, false);
    bool useParent1 = true;
    
    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            int current = i;
            do {
                if (useParent1) {
                    child1.path[current] = parent1.path[current];
                    child2.path[current] = parent2.path[current];
                } else {
                    child1.path[current] = parent2.path[current];
                    child2.path[current] = parent1.path[current];
                }
                visited[current] = true;
                
                auto it = find(parent2.path.begin(), parent2.path.end(), parent1.path[current]);
                current = distance(parent2.path.begin(), it);
            } while (current != i);
            
            useParent1 = !useParent1;
        }
    }
    
    return {child1, child2};
}

pair<Individual, Individual> crossover(const Individual& parent1, const Individual& parent2, 
                                     CrossoverType crossoverType) {
    switch(crossoverType) {
        case CX: return cycleCrossover(parent1, parent2);
        case OX: 
        default: return orderCrossover(parent1, parent2);
    }
}

void swapMutation(Individual& individual, double mutationRate) {
    uniform_real_distribution<double> prob(0.0, 1.0);
    
    if (prob(generator) <= mutationRate) {
        uniform_int_distribution<int> dis(0, individual.path.size() - 1);
        int i = dis(generator);
        int j = dis(generator);
        swap(individual.path[i], individual.path[j]);
    }
}

void twoOptImprovement(const vector<vector<double>>& graph, Individual& individual) {
    int n = individual.path.size();
    bool improved = true;
    
    while (improved) {
        improved = false;
        for (int i = 0; i < n - 1 && !improved; i++) {
            for (int j = i + 2; j < n && !improved; j++) {
                double oldCost = graph[individual.path[i]][individual.path[i+1]] + 
                                graph[individual.path[j]][individual.path[(j+1)%n]];
                double newCost = graph[individual.path[i]][individual.path[j]] + 
                                graph[individual.path[i+1]][individual.path[(j+1)%n]];
                
                if (newCost < oldCost) {
                    reverse(individual.path.begin() + i + 1, individual.path.begin() + j + 1);
                    individual.cost += newCost - oldCost;
                    improved = true;
                }
            }
        }
    }
    individual.fitness = 1.0 / (1.0 + individual.cost);
}

pair<double, vector<double>> GeneticAlgorithmWithConvergence(const vector<vector<double>>& graph, int vertexCount, 
                       double mutationRate = 0.02, double crossRate = 0.8, 
                       int popSize = 100, CrossoverType crossoverType = OX) {
    
    vector<Individual> population = generateInitialPopulation(graph, vertexCount, popSize);
    evaluatePopulation(graph, population);
    
    sort(population.begin(), population.end(), [](const Individual& a, const Individual& b) {
        return a.fitness > b.fitness;
    });
    
    for (int i = 0; i < min(10, (int)population.size()); i++) {
        twoOptImprovement(graph, population[i]);
    }
    
    const int maxGenerations = 500;
    const int stagnationLimit = 50;
    int generation = 0;
    int stagnationCount = 0;
    double bestCost = population[0].cost;
    
    vector<double> convergenceData;
    convergenceData.push_back(bestCost);
    
    uniform_real_distribution<double> crossProb(0.0, 1.0);
    
    while (generation < maxGenerations && stagnationCount < stagnationLimit) {
        vector<Individual> newPopulation;
        newPopulation.reserve(popSize);
        
        int eliteSize = popSize / 10;
        for (int i = 0; i < eliteSize; i++) {
            newPopulation.push_back(population[i]);
        }
        
        while (newPopulation.size() < popSize) {
            Individual parent1 = tournamentSelection(population);
            Individual parent2 = tournamentSelection(population);
            
            if (crossProb(generator) < crossRate) {
                auto children = crossover(parent1, parent2, crossoverType);
                
                swapMutation(children.first, mutationRate);
                swapMutation(children.second, mutationRate);
                
                children.first.cost = calculateCost(graph, children.first.path);
                children.first.fitness = 1.0 / (1.0 + children.first.cost);
                children.second.cost = calculateCost(graph, children.second.path);
                children.second.fitness = 1.0 / (1.0 + children.second.cost);
                
                newPopulation.push_back(children.first);
                if (newPopulation.size() < popSize) {
                    newPopulation.push_back(children.second);
                }
            } else {
                swapMutation(parent1, mutationRate);
                parent1.cost = calculateCost(graph, parent1.path);
                parent1.fitness = 1.0 / (1.0 + parent1.cost);
                newPopulation.push_back(parent1);
            }
        }
        
        population = move(newPopulation);
        sort(population.begin(), population.end(), [](const Individual& a, const Individual& b) {
            return a.fitness > b.fitness;
        });
        
        if (generation % 25 == 0) {
            for (int i = 0; i < min(5, (int)population.size()); i++) {
                twoOptImprovement(graph, population[i]);
            }
        }
        
        if (population[0].cost < bestCost - 1e-6) {
            bestCost = population[0].cost;
            stagnationCount = 0;
        } else {
            stagnationCount++;
        }
        
        convergenceData.push_back(bestCost);
        generation++;
    }
    
    return {population[0].cost, convergenceData};
}

double GeneticAlgorithm(const vector<vector<double>>& graph, int vertexCount, 
                       double mutationRate = 0.02, double crossRate = 0.8, 
                       int popSize = 100, CrossoverType crossoverType = OX) {
    auto result = GeneticAlgorithmWithConvergence(graph, vertexCount, mutationRate, crossRate, popSize, crossoverType);
    return result.first;
}

auto factorialExperiment(const vector<vector<double>>& graph, int vertexCount) {
    ofstream resultsFile("data/factorial_results.csv");
    resultsFile << "PopSize,MaxGenerations,CrossRate,MutationRate,CrossoverType,AverageCost,AverageTime\n";

    vector<int> popSizes = {50, 100, 200};
    vector<int> maxGens = {100, 200, 500};
    vector<double> crossRates = {0.7, 0.8, 0.9};
    vector<double> mutationRates = {0.01, 0.02, 0.05};
    vector<CrossoverType> crossoverTypes = {OX, CX};

    int totalRuns = popSizes.size() * maxGens.size() * crossRates.size() * mutationRates.size() * crossoverTypes.size();
    cout << "Total de combinações a testar: " << totalRuns << endl;

    vector<TestConfig> allResults;

    for (int popSize : popSizes) {
        for (int maxGen : maxGens) {
            for (double crossRate : crossRates) {
                for (double mutationRate : mutationRates) {
                    for (CrossoverType crossoverType : crossoverTypes) {
                        int adjustedMaxGen = maxGen;
                        int evaluations = popSize * adjustedMaxGen;
                        
                        // Run 10 times and get average
                        double totalCost = 0.0;
                        double totalTime = 0.0;
                        int runs = 10;
                        
                        for (int i = 0; i < runs; i++) {
                            auto start = chrono::high_resolution_clock::now();
                            double bestCost = GeneticAlgorithm(graph, vertexCount, mutationRate, 
                                                            crossRate, popSize, crossoverType);
                            auto end = chrono::high_resolution_clock::now();
                            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
                            
                            totalCost += bestCost;
                            totalTime += duration.count();
                        }
                        
                        double avgCost = totalCost / runs;
                        double avgTime = totalTime / runs;
                        
                        TestConfig config{
                            popSize, adjustedMaxGen, crossRate, mutationRate, 
                            crossoverType, avgCost, avgTime
                        };
                        allResults.push_back(config);

                        resultsFile << popSize << "," << adjustedMaxGen << "," << crossRate << "," 
                                    << mutationRate << "," << (crossoverType == OX ? "OX" : "CX") << ","
                                    << avgCost << "," << avgTime << "\n";
                        
                        cout << "PopSize: " << popSize << " MaxGen: " << adjustedMaxGen 
                             << " Cross: " << crossRate << " Mut: " << mutationRate 
                             << " Crossover: " << (crossoverType == OX ? "OX" : "CX")
                             << " AvgCost: " << avgCost << " AvgTime: " << avgTime << "ms\n";
                    }
                }
            }
        }
    }

    auto bestConfig = *min_element(allResults.begin(), allResults.end(), 
        [](const TestConfig& a, const TestConfig& b) {
            return a.bestCost < b.bestCost;
        });

    cout << "\nMelhor configuração encontrada:\n";
    cout << "PopSize: " << bestConfig.popSize << "\n";
    cout << "MaxGenerations: " << bestConfig.maxGenerations << "\n";
    cout << "CrossRate: " << bestConfig.crossRate << "\n";
    cout << "MutationRate: " << bestConfig.mutationRate << "\n";
    cout << "CrossoverType: " << (bestConfig.crossoverType == OX ? "OX" : "CX") << "\n";
    cout << "AvgBestCost: " << bestConfig.bestCost << "\n";
    cout << "AvgExecutionTime: " << bestConfig.executionTime << "ms\n";

    resultsFile.close();

    return bestConfig;
}

void runBestConfigMultipleTimes(const vector<vector<double>>& graph, int vertexCount, 
                              const TestConfig& bestConfig) {
    ofstream convergenceFile("data/convergence_data.csv");
    convergenceFile << "Run,Generation,BestCost\n";

    vector<double> allCosts;
    
    int runs = 20;
    for (int run = 0; run < runs; run++) {
        auto start = chrono::high_resolution_clock::now();
        auto result = GeneticAlgorithmWithConvergence(graph, vertexCount, 
                                         bestConfig.mutationRate, 
                                         bestConfig.crossRate, 
                                         bestConfig.popSize,
                                         bestConfig.crossoverType);
        double bestCost = result.first;
        vector<double> convergenceData = result.second;
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

        // Save convergence data for this run
        for (size_t gen = 0; gen < convergenceData.size(); gen++) {
            convergenceFile << (run+1) << "," << gen << "," << convergenceData[gen] << "\n";
        }
        
        allCosts.push_back(bestCost);
        
        cout << "Execução " << (run+1) << ": " << bestCost << " (Tempo: " << duration.count() << "ms)\n";
    }

    double avg = accumulate(allCosts.begin(), allCosts.end(), 0.0) / allCosts.size();
    double minCost = *min_element(allCosts.begin(), allCosts.end());
    double maxCost = *max_element(allCosts.begin(), allCosts.end());

    cout << "\nEstatísticas após " << runs << " execuções:\n";
    cout << "Média: " << avg << "\n";
    cout << "Melhor: " << minCost << "\n";
    cout << "Pior: " << maxCost << "\n";

    convergenceFile.close();
}

int main() {
    int vertexCount = 128;
    vector<vector<double>> graph = readGraph(vertexCount);    
    auto bestConfig = factorialExperiment(graph, vertexCount);
    
    runBestConfigMultipleTimes(graph, vertexCount, bestConfig);
    return 0;
}