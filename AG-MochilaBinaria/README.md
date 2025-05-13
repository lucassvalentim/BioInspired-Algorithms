AG-MochilaBinaria
=================

Este repositório contém a implementação de um Algoritmo Genético (AG) para resolver o problema da Mochila Binária, além de scripts auxiliares em Python para análise de resultados.

Estrutura do repositório
-------------------------

- **AnaliseCompleta.pdf** Documento com explicação detalhada da análise realizada em cima do algoritmo.
- **Makefile** Arquivo de configuração para compilar os executáveis principais.
- **genetic_algorithm.cpp / genetic_algorithm.hpp** Implementação do Algoritmo Genético genérico.
- **mainTabela.cpp** Módulo principal que executa o AG e gera tabelas de resultados (saída em CSV).
- **mainConvergencia.cpp** Módulo principal que executa o AG e gera dados para análise de convergência (saída em CSV).
- **entradas/** Diretório com instâncias de teste (arquivos de texto):

  - `pXX/p.txt` — número de indivíduos e capacidade.
  - `pXX/w.txt` — pesos dos itens.
  - `pXX/p.txt` — valores (profits) dos itens.
  - `pXX/s.txt` — parâmetros adicionais (por exemplo, seed ou configurações específicas).
- **pythonScripts/** Scripts em Python para processar os resultados gerados pelos executáveis:

  - `scriptTabela.py` — lê arquivos CSV de resultados e gera tabelas consolidadas.
  - `scriptConvergencia.py` — lê dados de convergência e plota gráficos de convergência.
- **resultados/** Saídas geradas pelos scripts/Python após uma execução de exemplo:

  - `results01.csv`, `results02.csv` — tabelas consolidadas de experimentos.
  - `convergence_data.csv` — dados brutos para o gráfico de convergência.
  - `convergence_plot.png` — gráfico gerado a partir de `convergence_data.csv`.
  - `Os resultados da pasta "resultados" foram obtidos em cima dos arquivos de entrada presentes na pasta "entradas/p08"`

Como compilar
-------------

1. Assegure que você tenha instalado um compilador C++ compatível com C++17 (por exemplo, `g++`).
2. No terminal, navegue até a raiz do repositório (`AG-MochilaBinaria/`).
3. Execute:

   ```
   make
   ```

   Isso irá gerar dois executáveis:

   - `mainTabela`
   - `mainConvergencia`
4. Para remover arquivos objeto e executáveis gerados:

   ```
   make clean
   ```

Como executar
-------------

- **Gerar tabelas de resultados**

  ```
  ./mainTabela <capacidade.txt> <profits.txt> <optimalSolution.txt> <weights.txt>
  ```

  Substitua `p01` por qualquer outra pasta de instância em `entradas/`.
- **Gerar dados de convergência**

  ```
  ./mainConvergencia <capacidade.txt> <profits.txt> <optimalSolution.txt> <weights.txt>
  ```

Após a execução, serão gerados arquivos CSV na pasta `resultados/`.
