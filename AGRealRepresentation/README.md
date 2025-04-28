# Projeto: Algoritmo Genético com Representação Real

** Lucas Henrique Valentim Rocha **

## Estrutura de Diretórios

```
AGRealRepresentation/
├── main.cpp             # Executa o experimento fatorial com diversas combinações de parâmetros
├── genetic_algorithm.hpp # Contém a implementação do Algoritmo Genético (função genetic_algorithm)
├── main.py              # Script Python para gerar gráficos de evolução dos resultados
├── resultados_fatoriais.csv  # Resultado consolidado de todas as combinações testadas
├── melhor_execucao.txt      # Fitness das 10 execuções da melhor configuração encontrada
├── evolucao_10_execucoes.png # Gráfico da evolução das 10 execuções da melhor combinação
```

## Descrição dos Arquivos

- **main.cpp**: Arquivo C++ que executa o algoritmo genético para todas as combinações de parâmetros:
  - Taxa de mutação: 1%, 5%, 10%
  - Taxa de cruzamento: 60%, 80%, 100%
  - Tamanho da população: 50, 100, 500
  - Número de gerações: 50, 100, 300
  - Taxa de elitismo: 5%, 25%, 50%

  Cada combinação é executada 10 vezes em paralelo para acelerar o processo.

- **genetic_algorithm.hpp**: Header file com a implementação da função `genetic_algorithm`, que:
  - Gera uma população inicial.
  - Aplica seleção, cruzamento BLX-αβ e mutação.
  - Evolui até o número máximo de gerações.
  - Retorna os fitness ao longo das gerações.

- **main.py**: Script Python para:
  - Ler `melhor_execucao.txt`
  - Plotar o gráfico com a evolução das 10 execuções.
  - Salvar o gráfico como `evolucao_10_execucoes.png`.

- **resultados_fatoriais.csv**:
  - Contém o resultado consolidado de todas as combinações testadas.
  - Cada linha representa uma combinação de parâmetros.
  - Colunas:
    - MutationRate, CrossoverRate, PopulationSize, NumGenerations, ElitismRate
    - MeanBestFitness, StdDevBestFitness, MeanGlobalFitness, StdDevGlobalFitness

- **melhor_execucao.txt**:
  - Detalha a melhor configuração de parâmetros encontrada.
  - Lista os fitness geracionais de cada uma das 10 execuções para essa configuração.

- **evolucao_10_execucoes.png**:
  - Gráfico mostrando a evolução da aptidão (fitness) para as 10 execuções da melhor combinação.


## Como Executar

1. Compile o projeto C++:

```bash
# Exemplo usando g++
make
./ga_test
```
