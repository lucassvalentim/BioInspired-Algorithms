import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Carregar dados
df = pd.read_csv('../resultados/convergence_data.csv')

# Calcular estatísticas
df_stats = df.groupby(['Run', 'Generation']).agg({
    **{f'Ind{i}': ['max', 'mean'] for i in range(50)},
}).reset_index()

# Nível ótimo (substitua pelo valor correto)
optimal_fitness = 13549094  # Exemplo

# Configurar plot
plt.figure(figsize=(12, 7))
plt.title('Convergência do Algoritmo Genético (20 execuções)')
plt.xlabel('Geração')
plt.ylabel('Fitness')

# Plotar cada execução
for run in df_stats['Run'].unique():
    run_data = df_stats[df_stats['Run'] == run]
    generations = run_data['Generation']
    best_fitness = run_data[[(f'Ind{i}', 'max') for i in range(50)]].max(axis=1)
    
    # Normalizar em relação ao ótimo
    convergence = best_fitness / optimal_fitness
    plt.plot(generations, convergence, alpha=0.4, color='blue')

# Média entre execuções
mean_convergence = df_stats.groupby('Generation')[[(f'Ind{i}', 'max') for i in range(50)]].mean().max(axis=1) / optimal_fitness
plt.plot(mean_convergence.index, mean_convergence, linewidth=3, color='red', label='Média')

# Linha do ótimo
plt.axhline(y=1, color='green', linestyle='--', label='Ótimo')

plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig('convergence_plot.png', dpi=300)
plt.show()