#include <bits/stdc++.h>
using namespace std;

#define ll long long
#define endl '\n'
const double ERROR_THRESHOLD = 1e-2;

random_device rd;
mt19937 generator(rd());

vector<double> runs_each_iteration;
// Função para ler o grafo e calcular a matriz de distâncias entre os vértices
ifstream matriz_txt("input/lau15_dist.txt");
vector<vector<double>> readGraph(int vertexCount){
    vector<vector<double>> graph(vertexCount, vector<double>(vertexCount));
    
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

// Inicializa os feromônios na matriz
void initializePheromones(vector<vector<double>> &pheromones, double initialValue, int vertexCount) {
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            if (i != j)
                pheromones[i][j] = initialValue;
        }
    }
}

// Inicializa a matriz heurística baseada na distância
void initializeHeuristic(vector<vector<double>> &heuristic, vector<vector<double>> &graph, int vertexCount) {
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            if (i != j)
                heuristic[i][j] = 1 / graph[i][j];
        }
    }
}

// Calcula as probabilidades de escolha de um vértice baseado em feromônio e heurística
vector<pair<int, double>> calculateProbabilities(vector<vector<double>> &pheromones, vector<vector<double>> &heuristic, vector<bool> visited, int current, double alpha, double beta, int vertexCount) {
    vector<pair<int, double>> probabilities;
    double sum = 0;
    for (int j = 0; j < vertexCount; j++) {
        if (!visited[j])
            sum += pow(pheromones[current][j], alpha) * pow(heuristic[current][j], beta);
    }

    for (int j = 0; j < vertexCount; j++) {
        if (!visited[j]) {
            double probability = (pow(pheromones[current][j], alpha) * pow(heuristic[current][j], beta)) / sum;
            probabilities.push_back({j, probability});
        }
    }

    if (probabilities.empty())
        probabilities.push_back({current, 1});

    return probabilities;
}

// Método de roleta para escolher próximo vértice
int rouletteWheelSelection(vector<pair<int, double>> &probabilities) {
    vector<pair<int, double>> roulette(probabilities.size());
    roulette[0] = probabilities[0];

    for (int i = 1; i < probabilities.size(); i++) {
        roulette[i].first = probabilities[i].first;
        roulette[i].second = roulette[i - 1].second + probabilities[i].second;
    }

    uniform_real_distribution<double> dis(0, 1);
    double randomValue = dis(generator);
    for (auto &entry : roulette) {
        if (randomValue <= entry.second) {
            return entry.first;
        }
    }

    return probabilities.back().first;
}

// Calcula o custo total de um caminho percorrido
double calculatePathCost(vector<vector<double>> &graph, vector<int> &path) {
    double totalCost = 0;
    for (int i = 1; i < path.size(); i++) {
        totalCost += graph[path[i - 1]][path[i]];
    }

    totalCost += graph[path.back()][path[0]];
    return totalCost;
}

// Retorna o melhor caminho encontrado
vector<int> findBestPath(vector<vector<double>> &graph, vector<vector<int>> &paths) {
    vector<pair<double, int>> pathCosts(paths.size());
    for (int i = 0; i < paths.size(); i++) {
        pathCosts[i] = {calculatePathCost(graph, paths[i]), i};
    }

    sort(pathCosts.begin(), pathCosts.end());
    return paths[pathCosts[0].second];
}

// Evaporação dos feromônios
void evaporatePheromones(vector<vector<double>> &pheromones, double evaporationRate, int vertexCount) {
    for (int i = 0; i < vertexCount; i++) {
        for (int j = 0; j < vertexCount; j++) {
            pheromones[i][j] *= (1 - evaporationRate);
        }
    }
}

// Reforço dos feromônios baseado no melhor caminho encontrado
void reinforcePheromones(vector<vector<double>> &graph, 
    vector<int> &bestPath, 
    vector<vector<double>> &pheromones, 
    const double Q
) {
    double delta = Q / calculatePathCost(graph, bestPath);
    for (int i = 0; i < bestPath.size() - 1; i++) {
        pheromones[bestPath[i]][bestPath[i + 1]] += delta;
        pheromones[bestPath[i + 1]][bestPath[i]] += delta;
    }
}

// Reforço dos feromônios baseado em todos os caminhos
void reinforcePheromones(vector<vector<double>> &graph, 
    vector<vector<int>> antPaths, 
    vector<vector<double>> &pheromones, 
    const double Q
) {
    for (int i = 0; i < pheromones.size(); i++) {
        for(int j = 0; j < pheromones.size(); j++) {
            if(i == j) continue;

            double soma = 0;
            for(int k = 0; k < antPaths.size(); k++){
                for(int vertice = 0; vertice < antPaths[k].size() - 1; vertice++){
                    if(antPaths[k][vertice] == i && antPaths[k][vertice + 1] == j){
                        double delta = Q / calculatePathCost(graph, antPaths[k]);
                        soma += delta; 
                    }
                }
            }

            pheromones[i][j] += soma; 
        }
    }
}

void reinforcePheromones(vector<vector<double>> &graph, 
    vector<vector<int>> &antPaths, 
    vector<int> &bestPath, 
    vector<vector<double>> &pheromones, 
    const double Q, 
    const double epsilon
) {
    int n = pheromones.size();

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) continue;

            double soma = 0.0;
            double deltaBestAnt = 0.0;

            // Somatório de reforço de todas as formigas
            for (auto& path : antPaths) {
                double cost = calculatePathCost(graph, path);
                for (int k = 0; k < path.size() - 1; k++) {
                    if (path[k] == i && path[k + 1] == j) {
                        soma += Q / cost;
                        break;
                    }
                }
            }

            // Reforço elitista da melhor formiga até agora
            double bestCost = calculatePathCost(graph, bestPath);
            for (int k = 0; k < bestPath.size() - 1; k++) {
                if (bestPath[k] == i && bestPath[k + 1] == j) {
                    deltaBestAnt = epsilon * (Q / bestCost);
                    break;
                }
            }

            // Reforço total da aresta (i, j)
            pheromones[i][j] += soma + deltaBestAnt;
        }
    }
}

// Algoritmo de Otimização por Colônia de Formigas (ACO)
double ACO(vector<vector<double>> &graph, 
    int vertexCount, 
    double evaporationRate,
    const int maxIterations, 
    double alpha, 
    double beta,
    const double Q,
    const double epsilon,
    const int choiceMethod,
    const bool teste = false
) {
    if(teste) runs_each_iteration.resize(0);

    int antCount = vertexCount;
    int iteration = 0;

    vector<vector<double>> pheromones(vertexCount, vector<double>(vertexCount));
    vector<vector<double>> heuristic(vertexCount, vector<double>(vertexCount));
    initializePheromones(pheromones, 0.1, vertexCount);
    initializeHeuristic(heuristic, graph, vertexCount);

    vector<int> bestSolution;
    while (iteration < maxIterations) {
        vector<vector<int>> antPaths;
        for (int ant = 0; ant < antCount; ant++) {
            vector<bool> visited(vertexCount, false);
            int current = ant;
            visited[current] = true;
            vector<int> currentPath{current};

            while (currentPath.size() < vertexCount) {
                vector<pair<int, double>> probabilities = calculateProbabilities(pheromones, heuristic, visited, current, alpha, beta, vertexCount);
                int next = rouletteWheelSelection(probabilities);
                visited[next] = true;
                current = next;
                currentPath.push_back(current);
            }
            currentPath.push_back(ant);
            antPaths.push_back(currentPath);
        }

        evaporatePheromones(pheromones, evaporationRate, vertexCount);
        vector<int> bestPath = findBestPath(graph, antPaths);

        double bestPathValue = calculatePathCost(graph, bestPath);
        double bestSolutionValue = calculatePathCost(graph, bestSolution);

        if (bestSolution.empty() || bestPathValue < bestSolutionValue) {
            bestSolution = bestPath;
        }

        if(choiceMethod == 1)
            reinforcePheromones(graph, antPaths, pheromones, Q);
        else if(choiceMethod == 2)
            reinforcePheromones(graph, antPaths, bestSolution, pheromones, Q, epsilon);
        else
            reinforcePheromones(graph,bestPath, pheromones, Q);

        
        if(teste){
            runs_each_iteration.push_back(bestSolutionValue);
        }
        iteration++;
    }
    return calculatePathCost(graph, bestSolution);
}

void fatorial_test(vector<vector<double>> &graph, int vertexCount){
    vector<double> evaporationRates = {0.1, 0.3, 0.5};
    vector<int> maxIterationsSet = {50, 100, 300}; // Mantido fixo para este exemplo
    vector<double> alphas = {1.0, 2.5};
    vector<double> betas = {1, 2.0, 5.0};
    vector<double> Qs = {10, 100};
    vector<double> epsilons = {3, 5, 10};
    vector<int> choiceMethod = {1, 2, 3};
    
    ofstream out("data/teste_fatorial.csv");
    out << "evaporationRates,maxIteration,alpha,beta,Q,epsilon,method,AverageFitness\n";

    const int numRun = 20;

    for(auto pho : evaporationRates){
        for(auto maxIteration : maxIterationsSet){
            for(auto alpha : alphas){
                for(auto beta : betas){
                    for(auto Q : Qs){
                        for(auto epsilon : epsilons){
                            for(auto method : choiceMethod){
                                vector<double> resultados;

                                for(int i = 0; i < numRun; i++){
                                    double fitness = ACO(graph, 
                                        vertexCount, 
                                        pho, 
                                        maxIteration, 
                                        alpha, beta, 
                                        Q, epsilon, 
                                        method);

                                    resultados.push_back(fitness);
                                }

                                double average_fitness = accumulate(resultados.begin(), resultados.end(), 0.0) / resultados.size();
                                out << pho << "," << maxIteration << "," 
                                << alpha << "," << beta << "," 
                                << Q << "," << epsilon << "," 
                                << method << "," << fixed << setprecision(4) << average_fitness << "\n";

                                cout << "Configuração testada:\n" << "p= " << pho << "\nMaxIteration: " << maxIteration << "\na= " 
                                << alpha << "\nb= " << beta << "\nQ= " 
                                << Q << "\ne= " << epsilon << "\nmethod= " 
                                << method << "\nmedia: " << fixed << setprecision(4) << average_fitness << "\n"; 
                            }
                        }
                    }
                }
            }
        }
    }

    out.close();
    cout << "Experimentos Realizados\n";

}

void run_best_parameters(vector<vector<double>> &graph, int vertexCount){
    double evaporationRates = 0.3;
    int maxIterationsSet = 50; // Mantido fixo para este exemplo
    double alphas = 2.5;
    double betas =  5.0;
    double Qs = 10;
    double epsilons =  10;
    int choiceMethod = 3;
    
    ofstream out("data/convergencia_10execucoes.csv");
    out << "Execucao,Geracao";
    for(int i = 0; i < maxIterationsSet; i++){
        out << ",ind" << i;
    }
    out << '\n';

    const int numRun = 20;
    
    vector<vector<double>> resultados(numRun);
    for(int i = 0; i < numRun; i++){
        double fitness = ACO(graph, 
            vertexCount, 
            evaporationRates, 
            maxIterationsSet, 
            alphas, betas, 
            Qs, epsilons, 
            choiceMethod,
            true
        );

        resultados[i] = runs_each_iteration;
    }

    
    out.close();
    cout << "Experimentos Realizados\n";

}

int main() {
    int vertexCount;
    matriz_txt >> vertexCount;

    cout << vertexCount << endl;
    vector<vector<double>> graph(vertexCount, vector<double>(vertexCount));
    graph = readGraph(vertexCount);
    
   

    return 0;
}
