import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import os
from pathlib import Path


def get_fitness_range_from_csv(csv_path):

    if not os.path.exists(csv_path):
        return None, None
    
    try:
        # Read the CSV file with Entry1 column forced to string using converters
        df = pd.read_csv(csv_path, converters={'Entry1': str})
        
        # Remove rows where Entry1 is NaN or empty
        df = df.dropna(subset=['Entry1'])
        df = df[df['Entry1'].str.strip() != '']
        
        if len(df) == 0:
            return None, None
        
        # Extract all Entry1 values
        fitness_values = []
        for entry1_str in df['Entry1']:
            try:
                # Extract the first number (fitness value) from format "0.849346 (0.767700 + 0.081646)"
                fitness_value = float(entry1_str.split()[0])
                fitness_values.append(fitness_value)
            except:
                continue
        
        if not fitness_values:
            return None, None
        
        # Get lowest and highest fitness
        lowest_fitness = min(fitness_values)
        highest_fitness = max(fitness_values)
        
        return lowest_fitness, highest_fitness
    except Exception as e:
        print(f"Error reading {csv_path}: {e}")
        return None, None


def get_average_fitness_increment(base_dir, game_components, bs, cs):

    # SYS is always 0.0 for BS/CS comparison
    sys = 0.0
    
    # Convert game components to directory name (divide by 2)
    dir_name = str(game_components // 2)
    
    # Format the strategy directory name
    strategy_dir = f"BS({bs:.1f})CS({cs:.1f})SYS({sys:.1f})"
    
    lowest_values = []
    highest_values = []
    
    # Iterate through runs 1 to 10
    for run in range(1, 11):
        run_dir = f"{dir_name}_{run}"
        csv_path = os.path.join(base_dir, "Data", dir_name, run_dir, strategy_dir, "BalancingLogResults.csv")
        
        lowest, highest = get_fitness_range_from_csv(csv_path)
        if lowest is not None and highest is not None:
            lowest_values.append(lowest)
            highest_values.append(highest)
    
    # Calculate averages and increment
    if lowest_values and highest_values:
        avg_lowest = np.mean(lowest_values)
        avg_highest = np.mean(highest_values)
        fitness_increment = avg_highest - avg_lowest
        return fitness_increment
    else:
        return None


def get_highest_fitness_from_csv(csv_path):

    if not os.path.exists(csv_path):
        return None, None
    
    try:
        # Read the CSV file with Entry1 column forced to string using converters
        df = pd.read_csv(csv_path, converters={'Entry1': str, 'Generation': str})
        
        # Remove rows where Entry1 is NaN or empty
        df = df.dropna(subset=['Entry1'])
        df = df[df['Entry1'].str.strip() != '']
        
        if len(df) == 0:
            return None, None
        
        # Get the last row with actual data (highest generation)
        last_row = df.iloc[-1]
        
        # Extract generation number
        generation_number = int(last_row['Generation'])
        
        # Extract Entry1 value (format: "0.849346 (0.767700 + 0.081646)")
        entry1_str = last_row['Entry1']
        
        # Extract the first number (biggest fitness value)
        fitness_value = float(entry1_str.split()[0])
        
        return fitness_value, generation_number
    except Exception as e:
        print(f"Error reading {csv_path}: {e}")
        return None, None


def get_average_fitness_and_generation(base_dir, game_components, bs, cs):

    # SYS is always 0.0 for BS/CS comparison
    sys = 0.0
    
    # Convert game components to directory name (divide by 2)
    dir_name = str(game_components // 2)
    
    # Format the strategy directory name
    strategy_dir = f"BS({bs:.1f})CS({cs:.1f})SYS({sys:.1f})"
    
    fitness_values = []
    generation_values = []
    
    # Iterate through runs 1 to 10
    for run in range(1, 11):
        run_dir = f"{dir_name}_{run}"
        csv_path = os.path.join(base_dir, "Data", dir_name, run_dir, strategy_dir, "BalancingLogResults.csv")
        
        fitness, generation = get_highest_fitness_from_csv(csv_path)
        if fitness is not None and generation is not None:
            fitness_values.append(fitness)
            generation_values.append(generation)
    
    # Calculate averages
    if fitness_values and generation_values:
        return np.mean(fitness_values), np.mean(generation_values)
    else:
        return None, None


def plot_bs_cs_comparison(base_dir, bs_compare, cs_compare, plot_type='increment', save_path=None):

    # Game components range from 20 to 100 (in steps of 10)
    game_components_range = range(20, 101, 10)
    
    plt.figure(figsize=(12, 7))
    
    # Plot 1: Fixed baseline BS=1.0, CS=0.0 (black)
    bs_baseline = 1.0
    cs_baseline = 0.0
    x_baseline = []
    y_baseline = []
    
    for game_components in game_components_range:
        if plot_type == 'increment':
            value = get_average_fitness_increment(base_dir, game_components, bs_baseline, cs_baseline)
            if value is not None:
                x_baseline.append(game_components)
                y_baseline.append(value)
        else:
            avg_fitness, avg_generation = get_average_fitness_and_generation(
                base_dir, game_components, bs_baseline, cs_baseline)
            
            if avg_fitness is not None and avg_generation is not None:
                x_baseline.append(game_components)
                if plot_type == 'generation':
                    y_baseline.append(avg_generation)
                else:  # fitness
                    y_baseline.append(avg_fitness)
    
    if x_baseline:
        plt.plot(x_baseline, y_baseline, marker='o', linewidth=2, markersize=6, 
                color='black', label=f"BS={bs_baseline:.1f}, CS={cs_baseline:.1f}")
        print(f"BS={bs_baseline:.1f}, CS={cs_baseline:.1f}: {len(x_baseline)} data points")
    
    # Plot 2: Comparison BS/CS (blue)
    x_compare = []
    y_compare = []
    
    for game_components in game_components_range:
        if plot_type == 'increment':
            value = get_average_fitness_increment(base_dir, game_components, bs_compare, cs_compare)
            if value is not None:
                x_compare.append(game_components)
                y_compare.append(value)
        else:
            avg_fitness, avg_generation = get_average_fitness_and_generation(
                base_dir, game_components, bs_compare, cs_compare)
            
            if avg_fitness is not None and avg_generation is not None:
                x_compare.append(game_components)
                if plot_type == 'generation':
                    y_compare.append(avg_generation)
                else:  # fitness
                    y_compare.append(avg_fitness)
    
    if x_compare:
        plt.plot(x_compare, y_compare, marker='o', linewidth=2, markersize=6, 
                color='#1f77b4', label=f"BS={bs_compare:.1f}, CS={cs_compare:.1f}")
        print(f"BS={bs_compare:.1f}, CS={cs_compare:.1f}: {len(x_compare)} data points")
    
    # Configure plot
    plt.xlabel('Number of Game Components', fontsize=12)
    
    if plot_type == 'increment':
        plt.ylabel('Fitness Increment (Highest - Lowest)', fontsize=12)
        plt.title('Fitness Increment vs Game Components\nBS/CS Strategy Comparison (SYS=0.0)', fontsize=14)
    elif plot_type == 'generation':
        plt.ylabel('Average Generation', fontsize=12)
        plt.title('Average Generation vs Game Components\nBS/CS Strategy Comparison (SYS=0.0)', fontsize=14)
    else:
        plt.ylabel('Average Fitness', fontsize=12)
        plt.title('Average Fitness vs Game Components\nBS/CS Strategy Comparison (SYS=0.0)', fontsize=14)
    
    plt.grid(True, alpha=0.3)
    plt.xticks(range(20, 101, 10))
    plt.legend(loc='best', fontsize=10)
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, format='svg', bbox_inches='tight')
        print(f"Saved: {save_path}")
        plt.close()
    else:
        plt.show()


def main():
    """
    Main function to generate 5 SVG files comparing different BS/CS ratios to baseline BS=1.0/CS=0.0.
    Generates: BS 0.9-0.5 with CS 0.1-0.5 (each compared to BS=1.0, CS=0.0).
    """
    # Get the base directory (parent of this script)
    base_dir = Path(__file__).parent
    
    # Define the 5 BS/CS combinations to compare
    comparisons = [
        (0.9, 0.1),
        (0.8, 0.2),
        (0.7, 0.3),
        (0.6, 0.4),
        (0.5, 0.5),
    ]
    

    plot_type = 'increment' 

    
    print("Generating SVG files for BS/CS comparisons (SYS=0.0):")
    print(f"  Baseline: BS=1.0, CS=0.0 (black)")
    print("-" * 60)
    
    # Generate each comparison
    for bs_compare, cs_compare in comparisons:
        # Create filename
        filename = f"BS{bs_compare:.1f}_CS{cs_compare:.1f}_vs_Baseline.svg"
        save_path = os.path.join(base_dir, filename)
        
        print(f"Processing: BS={bs_compare:.1f}, CS={cs_compare:.1f}")
        
        # Generate and save the plot
        plot_bs_cs_comparison(base_dir, bs_compare, cs_compare, plot_type=plot_type, save_path=save_path)
    
    print("-" * 60)
    print(f"Successfully generated {len(comparisons)} SVG files!")


if __name__ == "__main__":
    main()
