import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import re

# ======= Parte 1: Ler CSV e salvar como .xlsx =======

# Lê o CSV
df = pd.read_csv('../resultados/resultados_fatoriais.csv')

# Salva como Excel (.xlsx)
excel_filename = '../resultados/resultados_fatoriais.xlsx'
df.to_excel(excel_filename, index=False)
print(f"Tabela salva como '{excel_filename}'.")

# Exibe a tabela formatada no terminal
print("\nTabela de Resultados Formatada:\n")
print(df.to_string(index=False))

# ======= Parte 2: Ler melhor execução e gerar gráficos =======

# Função para ler o ../resultados/melhor_execucao.txt
def ler_melhor_execucao(path):
    with open(path, 'r') as file:
        content = file.read()

    execucoes = re.findall(r'Execução \d+: (.+)', content)
    lista_execucoes = []
    for execucao in execucoes:
        fitness = list(map(float, execucao.split(', ')))
        lista_execucoes.append(fitness)
    
    return np.array(lista_execucoes)

# Lê as execuções
fitness_execucoes = ler_melhor_execucao('../resultados/melhor_execucao.txt')

# Número de gerações
geracoes = np.arange(fitness_execucoes.shape[1])

# Cria o gráfico
plt.figure(figsize=(10, 6))

# Gráfico: Todas as execuções
for i in range(fitness_execucoes.shape[0]):
    plt.plot(geracoes, fitness_execucoes[i], label=f'Execução {i+1}')

plt.xlabel('Geração')
plt.ylabel('Aptidão')
plt.title('Evolução das 10 Execuções')
plt.legend()
plt.grid(True)

# Salvar como PNG
grafico_filename = '../resultados/evolucao_10_execucoes.png'
plt.savefig(grafico_filename)
print(f"Gráfico salvo como '{grafico_filename}'.")

# plt.show()
