import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import os
from pathlib import Path


def get_highest_fitness_from_csv(csv_path):
    if not os.path.exists(csv_path):
        return None, None
    
    try:
        df = pd.read_csv(csv_path, converters={'Entry1': str, 'Generation': str})
        
        df = df.dropna(subset=['Entry1'])
        df = df[df['Entry1'].str.strip() != '']
        
        if len(df) == 0:
            return None, None
        
        last_row = df.iloc[-1]
        
        generation_number = int(last_row['Generation'])
        
        entry1_str = last_row['Entry1']

        fitness_value = float(entry1_str.split()[0])
        
        return fitness_value, generation_number
    except Exception as e:
        print(f"Error reading {csv_path}: {e}")
        return None, None


def get_average_fitness_and_generation(base_dir, game_components, bs, cs, sys):

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
        else:
            print(f"  Warning: Could not read {csv_path}")
    
    # Calculate averages
    if fitness_values and generation_values:
        return np.mean(fitness_values), np.mean(generation_values)
    else:
        return None, None


def plot_fitness_vs_components(base_dir, bs, cs, sys):

    game_components_range = range(20, 101, 10)
    
    # Collect data and plot average generation
    x_values = []
    y_values = []
    
    for game_components in game_components_range:
        avg_fitness, avg_generation = get_average_fitness_and_generation(base_dir, game_components, bs, cs, sys)
        if avg_fitness is not None and avg_generation is not None:
            x_values.append(game_components)
            y_values.append(avg_generation)
            
            print(f"Game Components: {game_components}, Fitness: {avg_fitness:.6f}, Generation: {avg_generation:.2f}")
        else:
            print(f"Game Components: {game_components}, No data available")
    
    if not x_values:
        print("No data available to plot")
        return
    
    # Create the plot
    plt.figure(figsize=(12, 7))
    plt.plot(x_values, y_values, marker='o', linewidth=2, markersize=8)
    plt.xlabel('Number of Game Components', fontsize=12)
    plt.ylabel('Average Generation', fontsize=12)
    plt.title(f'BS={bs:.1f}, CS={cs:.1f}, SYS={sys:.1f}', fontsize=14)
    plt.grid(True, alpha=0.3)
    plt.xticks(range(20, 101, 10))
    
    # Add some padding to y-axis to prevent label cutoff
    y_min, y_max = min(y_values), max(y_values)
    y_range = y_max - y_min
    plt.ylim(y_min - y_range * 0.1, y_max + y_range * 0.15)
    
    # Add value labels on points
    for x, y in zip(x_values, y_values):
        plt.annotate(f'{y:.2f}', (x, y), textcoords="offset points", 
                    xytext=(0, 10), ha='center', fontsize=9)
    
    plt.tight_layout()
    plt.show()


def main():

    base_dir = Path(__file__).parent
    
    # Configuration: Set your BS, CS, SYS values here
    BS = 1.0
    CS = 0.0
    SYS = 0.0
    
    print(f"Analyzing data for BS={BS}, CS={CS}, SYS={SYS}")
    print("-" * 60)
    
    # Plot the results
    plot_fitness_vs_components(base_dir, BS, CS, SYS)


if __name__ == "__main__":
    main()
