import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import os
from pathlib import Path


def get_fitness_range_from_csv(csv_path):

    if not os.path.exists(csv_path):
        return None, None
    
    try:
        # Read the CSV file
        df = pd.read_csv(csv_path, converters={'Entry1': str, 'Generation': str})
        
        # Remove rows where Entry1 is NaN or empty
        df = df.dropna(subset=['Entry1'])
        df = df[df['Entry1'].str.strip() != '']
        
        if len(df) == 0:
            return None, None
        
        # Extract all fitness values from Entry1 column
        fitness_values = []
        for entry1_str in df['Entry1']:
            # Extract the first number (fitness value) from format: "0.849346 (0.767700 + 0.081646)"
            fitness_value = float(entry1_str.split()[0])
            fitness_values.append(fitness_value)
        
        highest_fitness = max(fitness_values)
        lowest_fitness = min(fitness_values)
        
        return highest_fitness, lowest_fitness
    except Exception as e:
        print(f"Error reading {csv_path}: {e}")
        return None, None


def get_average_fitness_range_for_batch(base_dir, batch_size):

    highest_fitness_values = []
    lowest_fitness_values = []
    
    # Iterate through runs 1 to 10
    for run in range(1, 11):
        run_dir = f"30_{run}"
        batch_dir = f"BATCH({batch_size})TEAM(10)"
        csv_path = os.path.join(base_dir, "Data", "BatchSize", run_dir, batch_dir, "BalancingLogResults.csv")
        
        highest, lowest = get_fitness_range_from_csv(csv_path)
        if highest is not None and lowest is not None:
            highest_fitness_values.append(highest)
            lowest_fitness_values.append(lowest)
    
    # Calculate averages
    if highest_fitness_values and lowest_fitness_values:
        return np.mean(highest_fitness_values), np.mean(lowest_fitness_values)
    else:
        return None, None


def plot_batch_fitness_range(base_dir, save_path=None):
    batch_sizes = [20, 30, 40, 50, 60]
    avg_highest_fitness = []
    avg_lowest_fitness = []

    for batch_size in batch_sizes:
        avg_high, avg_low = get_average_fitness_range_for_batch(base_dir, batch_size)
        avg_highest_fitness.append(avg_high if avg_high is not None else 0)
        avg_lowest_fitness.append(avg_low if avg_low is not None else 0)
        print(f"Batch Size {batch_size}: Avg Highest = {avg_high:.6f}, Avg Lowest = {avg_low:.6f}")
    
    # Create range plot
    plt.figure(figsize=(10, 6))
    
    plt.plot(batch_sizes, avg_highest_fitness, marker='o', linewidth=2, markersize=8, 
             color='#2E86AB', label='Average Highest Fitness')
    plt.plot(batch_sizes, avg_lowest_fitness, marker='s', linewidth=2, markersize=8, 
             color='#C73E1D', label='Average Lowest Fitness')
    
    plt.fill_between(batch_sizes, avg_lowest_fitness, avg_highest_fitness, 
                     alpha=0.2, color='gray', label='Fitness Range')
    
    # Configure plot
    plt.xlabel('Batch Size', fontsize=12)
    plt.ylabel('Fitness Value', fontsize=12)
    plt.title('Fitness Range vs Batch Size', fontsize=14)
    plt.grid(True, alpha=0.3)
    plt.xticks(batch_sizes)
    plt.legend(fontsize=10)
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, format='svg', bbox_inches='tight')
        print(f"Saved: {save_path}")
        plt.close()
    else:
        plt.show()


def main():

    base_dir = Path(__file__).parent
    
    # Create output directory
    output_dir = os.path.join(base_dir, "GenerationGraph")
    os.makedirs(output_dir, exist_ok=True)
    
    print("Generating batch size fitness range plot:")
    print("-" * 60)
    
    # Generate plot
    save_path = os.path.join(output_dir, "BatchSize_Fitness_Range.svg")
    plot_batch_fitness_range(base_dir, save_path=save_path)
    
    print("-" * 60)
    print(f"Successfully generated batch size fitness range plot!")
    print(f"Output directory: {output_dir}")


if __name__ == "__main__":
    main()
