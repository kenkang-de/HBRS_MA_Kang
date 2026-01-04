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


def get_average_fitness_range(base_dir, game_components, bs, sys):

    # CS is always 0.0 for BS/SYS comparison
    cs = 0.0
    
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
    
    # Calculate averages
    if lowest_values and highest_values:
        return np.mean(lowest_values), np.mean(highest_values)
    else:
        return None, None


def plot_bs_sys_range_comparison(base_dir, bs_compare, sys_compare, save_path=None):

    # Game components range from 20 to 100 (in steps of 10)
    game_components_range = range(20, 101, 10)
    
    plt.figure(figsize=(12, 7))
    
    # Baseline: BS=1.0, CS=0.0, SYS=0.0 (black)
    bs_baseline = 1.0
    sys_baseline = 0.0
    x_baseline = []
    y_baseline_low = []
    y_baseline_high = []
    
    for game_components in game_components_range:
        avg_lowest, avg_highest = get_average_fitness_range(base_dir, game_components, bs_baseline, sys_baseline)
        if avg_lowest is not None and avg_highest is not None:
            x_baseline.append(game_components)
            y_baseline_low.append(avg_lowest)
            y_baseline_high.append(avg_highest)
    
    if x_baseline:
        # Plot baseline lowest and highest as lines with shaded area
        plt.plot(x_baseline, y_baseline_low, linewidth=2, 
                color='black', linestyle='--')
        plt.plot(x_baseline, y_baseline_high, linewidth=2, 
                color='black', linestyle='-', label=f"BS={bs_baseline:.1f}, SYS={sys_baseline:.1f}")
        plt.fill_between(x_baseline, y_baseline_low, y_baseline_high, color='black', alpha=0.1)
        print(f"BS={bs_baseline:.1f}, SYS={sys_baseline:.1f}: {len(x_baseline)} data points")
    
    # Comparison: Custom BS/SYS (blue)
    x_compare = []
    y_compare_low = []
    y_compare_high = []
    
    for game_components in game_components_range:
        avg_lowest, avg_highest = get_average_fitness_range(base_dir, game_components, bs_compare, sys_compare)
        if avg_lowest is not None and avg_highest is not None:
            x_compare.append(game_components)
            y_compare_low.append(avg_lowest)
            y_compare_high.append(avg_highest)
    
    if x_compare:
        # Plot comparison lowest and highest as lines with shaded area
        plt.plot(x_compare, y_compare_low, linewidth=2, 
                color='#1f77b4', linestyle='--')
        plt.plot(x_compare, y_compare_high, linewidth=2, 
                color='#1f77b4', linestyle='-', label=f"BS={bs_compare:.1f}, SYS={sys_compare:.1f}")
        plt.fill_between(x_compare, y_compare_low, y_compare_high, color='#1f77b4', alpha=0.1)
        print(f"BS={bs_compare:.1f}, SYS={sys_compare:.1f}: {len(x_compare)} data points")
    
    # Configure plot
    plt.xlabel('Number of Game Components', fontsize=12)
    plt.ylabel('Average Fitness', fontsize=12)
    plt.title(f'BS={bs_compare:.1f}, SYS={sys_compare:.1f}', fontsize=14)
    plt.grid(True, alpha=0.3)
    plt.xticks(range(20, 101, 10))
    plt.legend(loc='best', fontsize=9)
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
    
    # Define the 5 BS/SYS combinations to compare
    comparisons = [
        (0.9, 0.1),
        (0.8, 0.2),
        (0.7, 0.3),
        (0.6, 0.4),
        (0.5, 0.5),
    ]
    
    print("Generating SVG files for BS/SYS range comparisons (CS=0.0):")
    print(f"  Baseline: BS=1.0, SYS=0.0 (black)")
    print("-" * 60)
    
    # Generate each comparison
    for bs_compare, sys_compare in comparisons:
        # Create filename
        filename = f"Range_BS{bs_compare:.1f}_SYS{sys_compare:.1f}_vs_Baseline.svg"
        save_path = os.path.join(base_dir, filename)
        
        print(f"Processing: BS={bs_compare:.1f}, SYS={sys_compare:.1f}")
        
        # Generate and save the plot
        plot_bs_sys_range_comparison(base_dir, bs_compare, sys_compare, save_path=save_path)
    
    print("-" * 60)
    print(f"Successfully generated {len(comparisons)} SVG files!")


if __name__ == "__main__":
    main()
