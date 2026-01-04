import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import os
from pathlib import Path


def get_highest_generation_from_csv(csv_path):
    if not os.path.exists(csv_path):
        return None
    
    try:
        df = pd.read_csv(csv_path, converters={'Entry1': str, 'Generation': str})
        
        df = df.dropna(subset=['Entry1'])
        df = df[df['Entry1'].str.strip() != '']
        
        if len(df) == 0:
            return None
        
        last_row = df.iloc[-1]
        generation_number = int(last_row['Generation'])
        
        return generation_number
    except Exception as e:
        print(f"Error reading {csv_path}: {e}")
        return None


def get_average_generation_for_stat(base_dir, stat_value):

    generation_values = []
    
    for run in range(1, 11):
        run_dir = f"30_{run}"
        stat_dir = f"STAT({stat_value})"
        csv_path = os.path.join(base_dir, "Data", "StatRange", run_dir, stat_dir, "BalancingLogResults.csv")
        
        generation = get_highest_generation_from_csv(csv_path)
        if generation is not None:
            generation_values.append(generation)
    
    if generation_values:
        return np.mean(generation_values)
    else:
        return None


def plot_stat_vs_generation(base_dir, save_path=None):
    stat_values = [10, 15, 20, 25, 30]
    avg_generations = []
    
    for stat_val in stat_values:
        avg_gen = get_average_generation_for_stat(base_dir, stat_val)
        avg_generations.append(avg_gen if avg_gen is not None else 0)

    
    # Create line plot
    plt.figure(figsize=(10, 6))
    plt.plot(stat_values, avg_generations, marker='o', linewidth=2, markersize=8, color='#2E86AB')
    
    # Configure plot
    plt.xlabel('STAT Value Range', fontsize=12)
    plt.ylabel('Average Highest Generation', fontsize=12)
    plt.title('Total Number of Game Components: 60\nBS=1.0, CS=0.0, SYS=0.0', fontsize=14)
    plt.grid(True, alpha=0.3)
    plt.xticks(stat_values)
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, format='svg', bbox_inches='tight')
        print(f"Saved: {save_path}")
        plt.close()
    else:
        plt.show()


def main():

    base_dir = Path(__file__).parent
    
    output_dir = os.path.join(base_dir, "GenerationGraph")
    os.makedirs(output_dir, exist_ok=True)
    
    save_path = os.path.join(output_dir, "STAT_vs_Generation.svg")
    plot_stat_vs_generation(base_dir, save_path=save_path)
    

if __name__ == "__main__":
    main()
