import itertools
import numpy as np
from clonalg import clonalg_tsp, ler_matriz_tsp
import csv

# Caminho da instância TSP
path = "./Instancias/sgb128_dist.txt"
distance_matrix = ler_matriz_tsp(path)

# Parâmetros para teste fatorial
pop_sizes = [50, 100, 200]
generations_list = [300, 500]
betas = [2, 3]
mutation_rates = [0.2, 0.5]
ns = [30, 50]
ds = [20, 40]

# Gera todas as combinações de parâmetros
param_grid = list(itertools.product(pop_sizes, generations_list, betas, mutation_rates, ns, ds))

results = []

print("Iniciando o teste fatorial no CLONALG com 10 execuções por combinação...\n")

for i, (pop_size, generations, beta, mutation_rate, n, d) in enumerate(param_grid):
    print(f"Configuração {i+1}/{len(param_grid)}:")
    print(f"pop_size={pop_size}, generations={generations}, beta={beta}, mutation_rate={mutation_rate}, n={n}, d={d}")

    distances = []

    for run in range(10):
        _, best_distance, _ = clonalg_tsp(
            distance_matrix,
            pop_size=pop_size,
            generations=generations,
            beta=beta,
            mutation_rate=mutation_rate,
            n=n,
            d=d
        )
        distances.append(best_distance)

    mean_distance = np.mean(distances)
    std_distance = np.std(distances)

    results.append({
        "pop_size": pop_size,
        "generations": generations,
        "beta": beta,
        "mutation_rate": mutation_rate,
        "n": n,
        "d": d,
        "mean": mean_distance,
        "std": std_distance
    })

# Ordena pelas melhores médias (e menores desvios)
results.sort(key=lambda x: (x["mean"], x["std"]))

# === Salva Top 5 em um TXT ===
with open("top5_resultados_lau15_dist.txt", "w") as f:
    f.write("=== Top 5 melhores configurações (CLONALG TSP) ===\n\n")
    for i, res in enumerate(results[:5]):
        f.write(f"Configuração {i+1}:\n")
        for k, v in res.items():
            f.write(f"{k}: {v}\n")
        f.write("\n")

# === Salva todos os resultados em um CSV ===
with open("resultados_completos_sgb128_dist.csv", "w", newline='') as csvfile:
    fieldnames = ["pop_size", "generations", "beta", "mutation_rate", "n", "d", "mean", "std"]
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
    writer.writeheader()
    for row in results:
        writer.writerow(row)

print("\nTeste fatorial concluído.")
