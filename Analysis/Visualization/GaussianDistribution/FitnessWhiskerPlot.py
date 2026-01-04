import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import os
from pathlib import Path


def get_highest_fitness_from_csv(csv_path):

    if not os.path.exists(csv_path):
        return None
    
    try:
        # Read the CSV file with Entry1 column forced to string using converters
        df = pd.read_csv(csv_path, converters={'Entry1': str, 'Generation': str})
        
        # Remove rows where Entry1 is NaN or empty
        df = df.dropna(subset=['Entry1'])
        df = df[df['Entry1'].str.strip() != '']
        
        if len(df) == 0:
            return None
        
        # Get the last row with actual data (highest generation)
        last_row = df.iloc[-1]
        
        entry1_str = last_row['Entry1']
        
        # Extract the first number (biggest fitness value)
        fitness_value = float(entry1_str.split()[0])
        
        return fitness_value
    except Exception as e:
        print(f"Error reading {csv_path}: {e}")
        return None


def get_fitness_values(base_dir, game_components, bs, cs, sys):

    # Convert game components to directory name (divide by 2)
    dir_name = str(game_components // 2)
    
    # Format the strategy directory name
    strategy_dir = f"BS({bs:.1f})CS({cs:.1f})SYS({sys:.1f})"
    
    fitness_values = []
    
    # Iterate through runs 1 to 10
    for run in range(1, 11):
        run_dir = f"{dir_name}_{run}"
        csv_path = os.path.join(base_dir, "Data", dir_name, run_dir, strategy_dir, "BalancingLogResults.csv")
        
        fitness = get_highest_fitness_from_csv(csv_path)
        if fitness is not None:
            fitness_values.append(fitness)
    
    return fitness_values


def plot_fitness_whisker(base_dir, bs, cs, sys):

    # Game components range from 20 to 100 (in steps of 10)
    game_components_range = range(20, 101, 10)
    
    # Collect data for whisker plot
    data_for_plot = []
    labels = []
    
    for game_components in game_components_range:
        fitness_values = get_fitness_values(base_dir, game_components, bs, cs, sys)
        if fitness_values:
            data_for_plot.append(fitness_values)
            labels.append(str(game_components))
            print(f"Game Components: {game_components}, Count: {len(fitness_values)}, "
                  f"Mean: {np.mean(fitness_values):.6f}, Std: {np.std(fitness_values):.6f}")
        else:
            print(f"Game Components: {game_components}, No data available")
    
    if not data_for_plot:
        print("No data available to plot")
        return
    
    # Create the whisker (box) plot
    plt.figure(figsize=(12, 7))
    bp = plt.boxplot(data_for_plot, labels=labels, patch_artist=True, 
                     showmeans=True, meanline=True, widths=0.3)
    
    # Customize box plot colors
    for patch in bp['boxes']:
        patch.set_facecolor('lightblue')
        patch.set_alpha(0.7)
        patch.set_linewidth(1)
    
    for whisker in bp['whiskers']:
        whisker.set_linewidth(1)
    
    for cap in bp['caps']:
        cap.set_linewidth(1)
    
    for median in bp['medians']:
        median.set_color('red')
        median.set_linewidth(2)
    
    for mean in bp['means']:
        mean.set_color('green')
        mean.set_linewidth(2)
    
    plt.xlabel('Number of Game Components', fontsize=12)
    plt.ylabel('Fitness Value', fontsize=12)
    plt.title(f'BS={bs:.1f}, CS={cs:.1f}, SYS={sys:.1f}', fontsize=14)
    plt.grid(True, alpha=0.3, axis='y')
    
    # Add legend
    plt.plot([], [], 'r-', linewidth=2, label='Median')
    plt.plot([], [], 'g-', linewidth=2, label='Mean')
    plt.legend()
    
    plt.tight_layout()
    plt.show()


def main():
    # Get the base directory (parent of this script)
    base_dir = Path(__file__).parent

    BS = 1.0
    CS = 0.0
    SYS = 0.0
    
    print(f"Analyzing fitness distribution for BS={BS}, CS={CS}, SYS={SYS}")
    print("-" * 60)
    
    # Plot the whisker plot
    plot_fitness_whisker(base_dir, BS, CS, SYS)


if __name__ == "__main__":
    main()
