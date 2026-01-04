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


def generate_strategy_color_threeway(cs, sys):

    lightness_map = {
        0.1: 0.25,   
        0.2: 0.5,
        0.3: 0.75,
        0.4: 1.0,  
    }
    
    bs = 0.5
    lightness = lightness_map.get(round(cs, 1), 0.75)
    
    red = int(255 * 0.5 * lightness)       
    green = int(255 * sys * 2 * lightness)  
    blue = int(255 * cs * 2 * lightness)    
    
    return f'#{red:02x}{green:02x}{blue:02x}'


def plot_threeway_bar_chart(base_dir, strategies, save_path=None):
    game_components_range = list(range(20, 101, 10))
    n_strategies = len(strategies)
    n_components = len(game_components_range)
    
    gen_data = {f"BS={bs:.1f}, CS={cs:.1f}, SYS={sys:.1f}": [] for bs, cs, sys in strategies}
    
    baseline_gen_data = []
    for game_components in game_components_range:
        avg_fitness, avg_generation = get_average_fitness_and_generation(
            base_dir, game_components, 1.0, 0.0, 0.0)
        baseline_gen_data.append(avg_generation if avg_generation is not None else 0)
    
    # Calculate average generation for each strategy and game component
    for bs, cs, sys in strategies:
        strategy_label = f"BS={bs:.1f}, CS={cs:.1f}, SYS={sys:.1f}"
        
        for game_components in game_components_range:
            avg_fitness, avg_generation = get_average_fitness_and_generation(
                base_dir, game_components, bs, cs, sys)
            gen_data[strategy_label].append(avg_generation if avg_generation is not None else 0)
    
    fig, ax = plt.subplots(figsize=(14, 9))
    bar_width = 0.18
    positions = np.arange(n_components)
    
    for idx, (bs, cs, sys) in enumerate(strategies):
        strategy_label = f"BS={bs:.1f}, CS={cs:.1f}, SYS={sys:.1f}"
        offset = (idx - n_strategies/2 + 0.5) * bar_width
        color = generate_strategy_color_threeway(cs, sys)
        ax.bar(positions + offset, gen_data[strategy_label], bar_width, 
               label=strategy_label, color=color)
    
    ax.plot(positions, baseline_gen_data, color='#FF6600', linewidth=2.5, 
            marker='s', markersize=8, label='BS=1.0 (Baseline)', zorder=10)
    
    avg_gen_per_component = []
    for i in range(n_components):
        values_at_component = [gen_data[f"BS={bs:.1f}, CS={cs:.1f}, SYS={sys:.1f}"][i] for bs, cs, sys in strategies]
        avg_gen_per_component.append(np.mean(values_at_component))
    
    ax.plot(positions, avg_gen_per_component, color='#CC2CCC', linewidth=2.5, 
            marker='o', markersize=8, label='Average', zorder=10)
    
    # Configure plot
    ax.set_xlabel('Number of Game Components', fontsize=12)
    ax.set_ylabel('Average Generation', fontsize=12)
    ax.set_xticks(positions)
    ax.set_xticklabels(game_components_range)
    ax.legend(loc='upper left', fontsize=10)
    ax.grid(True, alpha=0.3, axis='y')
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
    
    # Define three-way strategies (BS=0.5 with varying CS and SYS)
    threeway_strategies = [
        (0.5, 0.1, 0.4),
        (0.5, 0.2, 0.3),
        (0.5, 0.3, 0.2),
        (0.5, 0.4, 0.1),
    ]
    
    # Generate three-way strategies bar chart
    save_path = os.path.join(output_dir, "Generation_BarChart_ThreeWay.svg")
    plot_threeway_bar_chart(base_dir, threeway_strategies, save_path=save_path)
    

if __name__ == "__main__":
    main()
