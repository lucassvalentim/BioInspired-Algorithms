import pandas as pd

teste_fatorial_df = pd.read_csv("../data/teste_fatorial.csv")
print(teste_fatorial_df.head())

teste_fatorial_df.sort_values('AverageFitness', ascending=True, inplace=True)
print(teste_fatorial_df.head())

with open('../data/melhores_parametros.txt', 'w') as file:
    file.write("Melhores parâmetros:\n")
    file.write(f"P: {teste_fatorial_df.iloc[0, 0]}\n")
    file.write(f"MaxIteration: {teste_fatorial_df.iloc[0, 1]}\n")
    file.write(f"alpha: {teste_fatorial_df.iloc[0, 2]}\n")
    file.write(f"beta: {teste_fatorial_df.iloc[0, 3]}\n")
    file.write(f"epsilon: {teste_fatorial_df.iloc[0, 4]}\n")
    file.write(f"Q: {teste_fatorial_df.iloc[0, 5]}\n")
    file.write(f"method: {teste_fatorial_df.iloc[0, 6]}\n")
    file.write(f"AverageFitness: {teste_fatorial_df.iloc[0, 7]}\n")

    
    
    
    
