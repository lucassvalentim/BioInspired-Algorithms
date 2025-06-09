import pandas as pd
import matplotlib.pyplot as plt

# Caminho para o seu arquivo CSV
csv_path = "../data/convergencia_20_execucoes.csv"

# Leitura do CSV
df = pd.read_csv(csv_path)

# Selecionar apenas as colunas de iteração (ind0 a ind48), ignorando NaNs
iter_columns = [col for col in df.columns if col.startswith('ind') and df[col].notna().all()]
df_iter = df[iter_columns]

# Plotar gráfico de convergência
plt.figure(figsize=(12, 6))
for i in range(len(df_iter)):
    plt.plot(range(len(df_iter.columns)), df_iter.iloc[i], label=f'Execução {i+1}', alpha=0.6)

plt.xlabel('Iterações')
plt.ylabel('Valor')
plt.title('Convergência das Execuções')
plt.grid(True)
plt.tight_layout()
plt.savefig('../data/convergence_plot20_execucoes.png', dpi=300)
