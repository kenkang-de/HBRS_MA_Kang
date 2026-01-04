import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import os
from pathlib import Path


def get_highest_generation_from_csv(csv_path):

    if not os.path.exists(csv_path):
        return None
    
    try:
        # Read the CSV file
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


def get_average_generation_for_batch(base_dir, batch_size):

    generation_values = []
    
    # Iterate through runs 1 to 10
    for run in range(1, 11):
        run_dir = f"30_{run}"
        batch_dir = f"BATCH({batch_size})TEAM(10)"
        csv_path = os.path.join(base_dir, "Data", "BatchSize", run_dir, batch_dir, "BalancingLogResults.csv")
        
        generation = get_highest_generation_from_csv(csv_path)
        if generation is not None:
            generation_values.append(generation)
    
    # Calculate average
    if generation_values:
        return np.mean(generation_values)
    else:
        return None


def plot_batch_vs_generation(base_dir, save_path=None):
    """Create line plot showing average generation vs batch size."""
    batch_sizes = [20, 30, 40, 50, 60]
    avg_generations = []
    
    print("Processing batch size data...")
    for batch_size in batch_sizes:
        avg_gen = get_average_generation_for_batch(base_dir, batch_size)
        avg_generations.append(avg_gen if avg_gen is not None else 0)
        print(f"Batch Size {batch_size}: Average Generation = {avg_gen}")
    
    # Create line plot
    plt.figure(figsize=(10, 6))
    plt.plot(batch_sizes, avg_generations, marker='o', linewidth=2, markersize=8, color='#2E86AB')
    
    # Configure plot
    plt.xlabel('Batch Size', fontsize=12)
    plt.ylabel('Average Highest Generation', fontsize=12)
    plt.title('Total Number of Game Components:60\nBS=1.0, CS=0.0, SYS=0.0', fontsize=14)
    plt.grid(True, alpha=0.3)
    plt.xticks(batch_sizes)
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, format='svg', bbox_inches='tight')
        print(f"Saved: {save_path}")
        plt.close()
    else:
        plt.show()


def main():

    # Get the base directory (parent of this script)
    base_dir = Path(__file__).parent
    
    # Create output directory
    output_dir = os.path.join(base_dir, "GenerationGraph")
    os.makedirs(output_dir, exist_ok=True)
    
    print("Generating batch size vs generation line plot:")
    print("-" * 60)
    
    # Generate plot
    save_path = os.path.join(output_dir, "BatchSize_vs_Generation.svg")
    plot_batch_vs_generation(base_dir, save_path=save_path)
    
    print("-" * 60)
    print(f"Successfully generated batch size plot!")
    print(f"Output directory: {output_dir}")


if __name__ == "__main__":
    main()
