import pandas as pd
import numpy as np

# Carregar os dados do CSV
df = pd.read_csv('../resultados/results02.csv')

# Calcular uma métrica composta (quanto menor, melhor)
# Pondera diferença (60%), desvio padrão (30%) e complemento da média (10%)
df['Score'] = (0.6 * df['DifferenceFromOptimal'] / df['OptimalFitness'] + 
               0.3 * df['StdDeviation'] / df['OptimalFitness'] + 
               0.1 * (1 - df['AvgFitness'] / df['OptimalFitness']))

# Encontrar a linha com o melhor score (menor valor)
best_row = df.loc[df['Score'].idxmin()]

# Resultados
print("=== MELHOR COMBINAÇÃO DE PARÂMETROS ===")
print(f"Linha: {best_row.name + 2}")  # +2 porque o CSV tem header e começa em 0
print(f"PopSize: {best_row['PopSize']}")
print(f"Generations: {best_row['Generations']}")
print(f"ParentRatio: {best_row['ParentRatio']}%")
print(f"MutateProb: {best_row['MutateProb']}%")
print(f"EliteRatio: {best_row['EliteRatio']}%")
print(f"SelectionMethod: {'Torneio' if best_row['SelectionMethod'] == 1 else 'Roleta'}")
print("\n=== RESULTADOS ===")
print(f"Diferença para o ótimo: {best_row['DifferenceFromOptimal']} ({best_row['DifferenceFromOptimal']/best_row['OptimalFitness']*100:.2f}%)")
print(f"Fitness médio: {best_row['AvgFitness']:.2f}")
print(f"Melhor fitness: {best_row['BestFitness']}")
print(f"Desvio padrão: {best_row['StdDeviation']:.2f}")
print(f"Score: {best_row['Score']:.6f}")

# Mostrar as top 5 combinações
print("\n=== TOP 5 COMBINAÇÕES ===")
top_5 = df.sort_values('Score').head(5)
print(top_5[['PopSize', 'Generations', 'ParentRatio', 'MutateProb', 'EliteRatio', 
             'SelectionMethod', 'DifferenceFromOptimal', 'AvgFitness', 'StdDeviation', 'Score']].to_string(index=False))