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
        
        # Remove rows where Entry1 is NaN or empty
        df = df.dropna(subset=['Entry1'])
        df = df[df['Entry1'].str.strip() != '']
        
        if len(df) == 0:
            return None
        
        # Get the last row with actual data (highest generation)
        last_row = df.iloc[-1]
        generation_number = int(last_row['Generation'])
        
        return generation_number
    except Exception as e:
        print(f"Error reading {csv_path}: {e}")
        return None


def get_average_generation_for_population(base_dir, population_size, with_doc=True):
    generation_values = []
    folder_name = "Population" if with_doc else "Population_noDOC"
    
    # Iterate through runs 1 to 10
    for run in range(1, 11):
        run_dir = f"50_{run}"
        pop_dir = f"POP({population_size})"
        csv_path = os.path.join(base_dir, "Data", folder_name, run_dir, pop_dir, "BalancingLogResults.csv")
        
        generation = get_highest_generation_from_csv(csv_path)
        if generation is not None:
            generation_values.append(generation)
    
    # Calculate average
    if generation_values:
        return np.mean(generation_values)
    else:
        return None


def plot_doc_comparison(base_dir, save_path=None):
    population_sizes = [50, 60, 70, 80, 90, 100]
    avg_gen_with_doc = []
    avg_gen_without_doc = []
    
    print("Processing data with DOC...")
    for pop_size in population_sizes:
        avg_gen = get_average_generation_for_population(base_dir, pop_size, with_doc=True)
        avg_gen_with_doc.append(avg_gen if avg_gen is not None else 0)
        print(f"  Population {pop_size}: {avg_gen}")
    
    print("\nProcessing data without DOC...")
    for pop_size in population_sizes:
        avg_gen = get_average_generation_for_population(base_dir, pop_size, with_doc=False)
        avg_gen_without_doc.append(avg_gen if avg_gen is not None else 0)
        print(f"  Population {pop_size}: {avg_gen}")
    
    # Create line plot
    plt.figure(figsize=(10, 6))
    plt.plot(population_sizes, avg_gen_with_doc, marker='o', linewidth=2, markersize=8, 
             color='#2E86AB', label='With DOC')
    plt.plot(population_sizes, avg_gen_without_doc, marker='s', linewidth=2, markersize=8, 
             color='#C73E1D', label='Without DOC')
    
    # Configure plot
    plt.xlabel('Chromosome Population Size', fontsize=12)
    plt.ylabel('Average Highest Generation', fontsize=12)
    plt.title('Total Number of Game Components: 100\nBS=1.0, CS=0.0, SYS=0.0', fontsize=14)
    plt.grid(True, alpha=0.3)
    plt.xticks(population_sizes)
    plt.legend(fontsize=11)
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, format='svg', bbox_inches='tight')
        print(f"\nSaved: {save_path}")
        plt.close()
    else:
        plt.show()


def main():
    base_dir = Path(__file__).parent
    
    # Create output directory
    output_dir = os.path.join(base_dir, "GenerationGraph")
    os.makedirs(output_dir, exist_ok=True)
    
    # Generate plot
    save_path = os.path.join(output_dir, "Population_DOC_Comparison.svg")
    plot_doc_comparison(base_dir, save_path=save_path)


if __name__ == "__main__":
    main()
