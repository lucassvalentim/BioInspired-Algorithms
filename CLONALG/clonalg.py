import numpy as np
import random

def ler_matriz_tsp(caminho_arquivo):
    with open(caminho_arquivo, 'r') as f:
        linhas = f.readlines()
    
    matriz = [list(map(int, linha.strip().split())) for linha in linhas]
    return np.array(matriz)


def found_tsp_distance(matrix, route):
    distance = 0
    for i in range(len(route)):
        origem = route[i]
        destino = route[(i + 1) % len(route)]
        distance += matrix[origem][destino]
    return distance 

def generate_population(pop_size, num_tasks):
    base = np.arange(num_tasks)
    initial_population = np.array([np.random.permutation(base) for _ in range(pop_size)])
    return initial_population

def evaluate_population(population, distance_matrix):
    return np.array([found_tsp_distance(distance_matrix, ind) for ind in population])


def clone_top_n(population, fitnesses, n, beta=3):
    top_indices = np.argsort(fitnesses)[:n]
    clones = []

    for i, idx in enumerate(top_indices):
        num_clones = int(beta * (n - i) / n)
        clones.extend([population[idx].copy() for _ in range(max(1, num_clones))])

    return np.array(clones)


def mutate(individual, mutation_rate):
    mutated = individual.copy()
    if random.random() < mutation_rate:
        i, j = np.random.choice(len(mutated), 2, replace=False)
        mutated[i], mutated[j] = mutated[j], mutated[i]
    return mutated


def mutate_clones(clones, mutation_rate_base, clone_fitnesses):
    max_fit = np.max(clone_fitnesses)
    min_fit = np.min(clone_fitnesses)
    
    # Evita divisão por zero se todos forem iguais
    if max_fit == min_fit:
        probs = np.full(len(clones), mutation_rate_base)
    else:
        # Normaliza fitness: maior fitness = maior taxa de mutação
        norm = (clone_fitnesses - min_fit) / (max_fit - min_fit + 1e-10)
        # Calcula taxa adaptativa: inversamente proporcional à afinidade
        probs = mutation_rate_base * (0.5 + 0.5 * norm) 
    
    # Aplica mutações individualmente
    return np.array([mutate(clone, p) for clone, p in zip(clones, probs)])


def replace_population(population, fitnesses, clones, clone_fitnesses):
    combined = np.vstack((population, clones))
    combined_fitness = np.hstack((fitnesses, clone_fitnesses))

    best_indices = np.argsort(combined_fitness)[:len(population)]
    return combined[best_indices], combined_fitness[best_indices]

def gerar_novos_individuos(qtd, num_cidades):
    return np.array([np.random.permutation(num_cidades) for _ in range(qtd)])

def clonalg_tsp(distance_matrix, pop_size=30, generations=100, beta=3, mutation_rate=0.2, n=10, d=5):
    num_cidades = distance_matrix.shape[0]
    population = generate_population(pop_size, num_cidades)
    fitnesses = evaluate_population(population, distance_matrix)

    best_solution = None
    best_distance = float('inf')
    history = []

    for gen in range(generations):
        # Clonagem apenas dos n melhores
        clones = clone_top_n(population, fitnesses, n=n, beta=beta)
        clone_fitnesses = evaluate_population(clones, distance_matrix)

        mutated_clones = mutate_clones(clones, mutation_rate, clone_fitnesses)
        clone_fitnesses = evaluate_population(mutated_clones, distance_matrix)

        # Substituição por melhores entre população original e clones
        population, fitnesses = replace_population(population, fitnesses, mutated_clones, clone_fitnesses)

        # Substitui os d piores indivíduos por novos
        if d > 0:
            worst_indices = np.argsort(fitnesses)[-d:]
            novos = gerar_novos_individuos(d, num_cidades)
            novos_fitness = evaluate_population(novos, distance_matrix)
            population[worst_indices] = novos
            fitnesses[worst_indices] = novos_fitness

        # Atualiza melhor solução
        gen_best_idx = np.argmin(fitnesses)
        gen_best_fit = fitnesses[gen_best_idx]
        if gen_best_fit < best_distance:
            best_distance = gen_best_fit
            best_solution = population[gen_best_idx].copy()

        history.append(best_distance)

    return best_solution, best_distance, history