import matplotlib.pyplot as plt
import os
from clonalg import clonalg_tsp,  ler_matriz_tsp

def run_multiple_executions(distance_matrix, num_executions=10, instance_file="", save_dir="convergence_plots"):
    all_histories = []

    if not os.path.exists(save_dir):
        os.makedirs(save_dir)

    for i in range(num_executions):
        print(f"Executando rodada {i + 1}/{num_executions} para {instance_file}...")
        best_perm, best_makespan, history = clonalg_tsp(
            distance_matrix,
            pop_size=100,
            generations=500,
            beta=3,
            mutation_rate=0.5,
            n=50,
            d=20
        )
        all_histories.append(history)

    instance_num = instance_file.split("_")[-1].split(".")[0]

    plt.figure(figsize=(10, 6))
    for i, history in enumerate(all_histories):
        plt.plot(history, label=f'Execução {i + 1}')

    plt.title(f'Convergência - Instância {instance_num}')
    plt.xlabel('Geração')
    plt.ylabel('Makespan')
    plt.legend()
    plt.grid(True)

    output_path = os.path.join(save_dir, f'convergencia_instancia_{instance_num}.png')
    plt.savefig(output_path)
    plt.close()

    print(f"Gráfico salvo em: {output_path}\n")
    return all_histories

if __name__ == "__main__":
    # Lista de arquivos de instância
    instance_files = [
        # "./Instancias/lau15_dist.txt",
        "./Instancias/sgb128_dist.txt"
    ]
    
    for instance_file in instance_files:
        processing_times =  ler_matriz_tsp(instance_file)
        run_multiple_executions(
            processing_times,
            instance_file=instance_file,
            num_executions=10
        )
