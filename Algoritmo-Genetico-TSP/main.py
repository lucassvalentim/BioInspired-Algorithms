import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Lê o CSV
df = pd.read_csv('data/convergence_data.csv')  # Substitua pelo nome correto do seu arquivo

# Garante que os dados estão ordenados corretamente
df = df.sort_values(by=['Run', 'Generation'])

# Criação da figura
plt.figure(figsize=(12, 6))
sns.set(style='whitegrid')

# Paleta de cores variada
palette = sns.color_palette("husl", n_colors=df['Run'].nunique())

# Plot das execuções
for idx, run_id in enumerate(df['Run'].unique()):
    run_data = df[df['Run'] == run_id]
    if idx < 5:
        plt.plot(run_data['Generation'], run_data['BestCost'], label=f'Run {run_id}', color=palette[idx], alpha=0.5)
    else:
        plt.plot(run_data['Generation'], run_data['BestCost'], color=palette[idx], alpha=0.3)

# Plot da média
mean_per_generation = df.groupby('Generation')['BestCost'].mean()
plt.plot(mean_per_generation.index, mean_per_generation.values, color='black', linewidth=2.5, label='Média')

# Títulos e rótulos
plt.title('Convergência do Algoritmo Genético')
plt.xlabel('Geração')
plt.ylabel('Melhor Custo')
plt.legend(title='Legenda (parcial)', bbox_to_anchor=(1.05, 1), loc='upper left')

# Ajuste de layout
plt.tight_layout()

# Salvar em PNG
plt.savefig('data/convergencia_genetico.png', dpi=300)  # dpi alto para boa qualidade

# Mostrar o gráfico (opcional)
plt.show()
