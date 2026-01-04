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


def get_average_fitness_range(base_dir, game_components, bs, cs, sys):

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


def plot_fitness_range(base_dir, bs, cs, sys):

    # Game components range from 20 to 100 (in steps of 10)
    game_components_range = range(20, 101, 10)

    x_values = []
    y_lowest = []
    y_highest = []
    
    for game_components in game_components_range:
        avg_lowest, avg_highest = get_average_fitness_range(base_dir, game_components, bs, cs, sys)
        if avg_lowest is not None and avg_highest is not None:
            x_values.append(game_components)
            y_lowest.append(avg_lowest)
            y_highest.append(avg_highest)
            
            print(f"Game Components: {game_components}, Lowest: {avg_lowest:.6f}, Highest: {avg_highest:.6f}, Range: {avg_highest - avg_lowest:.6f}")
        else:
            print(f"Game Components: {game_components}, No data available")
    
    if not x_values:
        print("No data available to plot")
        return
    
    # Create the range plot
    plt.figure(figsize=(12, 7))
    
    # Fill the range area
    plt.fill_between(x_values, y_lowest, y_highest, alpha=0.3, color='lightblue', label='Fitness Range')
    
    # Plot the boundary lines
    plt.plot(x_values, y_lowest, marker='o', linewidth=2, markersize=6, color='blue', label='Average Lowest Fitness')
    plt.plot(x_values, y_highest, marker='s', linewidth=2, markersize=6, color='red', label='Average Highest Fitness')
    
    plt.xlabel('Number of Game Components', fontsize=12)
    plt.ylabel('Fitness Value', fontsize=12)
    plt.title(f'BS={bs:.1f}, CS={cs:.1f}, SYS={sys:.1f}', fontsize=14)
    plt.grid(True, alpha=0.3)
    plt.xticks(range(20, 101, 10))
    plt.legend(loc='best')
    
    plt.tight_layout()
    plt.show()


def main():
    # Get the base directory (parent of this script)
    base_dir = Path(__file__).parent
    
    BS = 1.0
    CS = 0.0
    SYS = 0.0
    
    print(f"Analyzing fitness range for BS={BS}, CS={CS}, SYS={SYS}")
    print("-" * 60)
    
    # Plot the range
    plot_fitness_range(base_dir, BS, CS, SYS)


if __name__ == "__main__":
    main()
