import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import os
from pathlib import Path


def get_fitness_range_from_csv(csv_path):

    if not os.path.exists(csv_path):
        return None, None
    
    try:
        df = pd.read_csv(csv_path, converters={'Entry1': str})
        
        df = df.dropna(subset=['Entry1'])
        df = df[df['Entry1'].str.strip() != '']
        
        if len(df) == 0:
            return None, None
        
        fitness_values = []
        for entry1_str in df['Entry1']:
            try:
                fitness_value = float(entry1_str.split()[0])
                fitness_values.append(fitness_value)
            except:
                continue
        
        if not fitness_values:
            return None, None
        
        lowest_fitness = min(fitness_values)
        highest_fitness = max(fitness_values)
        
        return lowest_fitness, highest_fitness
    except Exception as e:
        print(f"Error reading {csv_path}: {e}")
        return None, None


def get_average_fitness_increment(base_dir, game_components, bs, cs, sys):

    dir_name = str(game_components // 2)
    
    strategy_dir = f"BS({bs:.1f})CS({cs:.1f})SYS({sys:.1f})"
    
    lowest_values = []
    highest_values = []
    
    for run in range(1, 11):
        run_dir = f"{dir_name}_{run}"
        csv_path = os.path.join(base_dir, "Data", dir_name, run_dir, strategy_dir, "BalancingLogResults.csv")
        
        lowest, highest = get_fitness_range_from_csv(csv_path)
        if lowest is not None and highest is not None:
            lowest_values.append(lowest)
            highest_values.append(highest)
    
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

    dir_name = str(game_components // 2)
    
    strategy_dir = f"BS({bs:.1f})CS({cs:.1f})SYS({sys:.1f})"
    
    fitness_values = []
    generation_values = []
    
    for run in range(1, 11):
        run_dir = f"{dir_name}_{run}"
        csv_path = os.path.join(base_dir, "Data", dir_name, run_dir, strategy_dir, "BalancingLogResults.csv")
        
        fitness, generation = get_highest_fitness_from_csv(csv_path)
        if fitness is not None and generation is not None:
            fitness_values.append(fitness)
            generation_values.append(generation)
    
    if fitness_values and generation_values:
        return np.mean(fitness_values), np.mean(generation_values)
    else:
        return None, None


def plot_strategies_comparison(base_dir, strategies, plot_type='increment'):

    game_components_range = range(20, 101, 10)
    
    colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', 
              '#8c564b', '#e377c2', '#7f7f7f', '#bcbd22', '#17becf',
              '#aec7e8', '#ffbb78', '#98df8a', '#ff9896', '#c5b0d5']
    
    plt.figure(figsize=(12, 7))
    
    for idx, (bs, cs, sys) in enumerate(strategies):
        x_values = []
        y_values = []
        
        strategy_label = f"BS={bs:.1f}, CS={cs:.1f}, SYS={sys:.1f}"
        
        for game_components in game_components_range:
            if plot_type == 'increment':
                value = get_average_fitness_increment(base_dir, game_components, bs, cs, sys)
                if value is not None:
                    x_values.append(game_components)
                    y_values.append(value)
            else:
                avg_fitness, avg_generation = get_average_fitness_and_generation(
                    base_dir, game_components, bs, cs, sys)
                
                if avg_fitness is not None and avg_generation is not None:
                    x_values.append(game_components)
                    if plot_type == 'generation':
                        y_values.append(avg_generation)
                    else:  # fitness
                        y_values.append(avg_fitness)
        
        if x_values:
            color = colors[idx % len(colors)]
            plt.plot(x_values, y_values, marker='o', linewidth=2, markersize=6, 
                    color=color, label=strategy_label)
            print(f"{strategy_label}: {len(x_values)} data points")
    
    plt.xlabel('Number of Game Components', fontsize=12)
    
    if plot_type == 'increment':
        plt.ylabel('Fitness Increment (Highest - Lowest)', fontsize=12)
        plt.title('Fitness Increment vs Game Components\nfor Different Strategies', fontsize=14)
    elif plot_type == 'generation':
        plt.ylabel('Average Generation', fontsize=12)
        plt.title('Average Generation vs Game Components\nfor Different Strategies', fontsize=14)
    else:
        plt.ylabel('Average Fitness', fontsize=12)
        plt.title('Average Fitness vs Game Components\nfor Different Strategies', fontsize=14)
    
    plt.grid(True, alpha=0.3)
    plt.xticks(range(20, 101, 10))
    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left', fontsize=9)
    plt.tight_layout()
    plt.show()


def main():

    base_dir = Path(__file__).parent
    

    strategies = [
        (0.5, 0.5, 0.0),
        (0.6, 0.4, 0.0),
        (0.7, 0.3, 0.0),
        (0.8, 0.2, 0.0),
        (0.9, 0.1, 0.0),
        (1.0, 0.0, 0.0),
    ]
    
    
    print("Comparing strategies:")
    for bs, cs, sys in strategies:
        print(f"  BS={bs:.1f}, CS={cs:.1f}, SYS={sys:.1f}")
    print("-" * 60)

    plot_type = 'increment'  

    plot_strategies_comparison(base_dir, strategies, plot_type=plot_type)


if __name__ == "__main__":
    main()
