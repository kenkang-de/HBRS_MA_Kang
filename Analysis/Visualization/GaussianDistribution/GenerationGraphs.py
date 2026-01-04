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


def generate_strategy_color_cs(bs, cs):
    """Generate color for CS strategies with normalized lightness."""
    lightness_map = {
        (0.9, 0.1): 0.2,
        (0.8, 0.2): 0.4,
        (0.7, 0.3): 0.6,
        (0.6, 0.4): 0.8,
        (0.5, 0.5): 1.0,
    }
    key = (round(bs, 1), round(cs, 1))
    lightness = lightness_map.get(key, 0.5)
    red = int(255 * bs * lightness)
    blue = int(255 * cs * 2 * lightness)
    green = 0
    return f'#{red:02x}{green:02x}{blue:02x}'


def generate_strategy_color_sys(bs, sys):
    """Generate color for SYS strategies with normalized lightness."""
    lightness_map = {
        (0.9, 0.1): 0.2,
        (0.8, 0.2): 0.4,
        (0.7, 0.3): 0.6,
        (0.6, 0.4): 0.8,
        (0.5, 0.5): 1.0,
    }
    key = (round(bs, 1), round(sys, 1))
    lightness = lightness_map.get(key, 0.5)
    red = int(255 * bs * lightness)
    green = int(255 * sys * 2 * lightness)
    blue = 0
    return f'#{red:02x}{green:02x}{blue:02x}'


def plot_generation_bar_chart_cs(base_dir, strategies, save_path=None):
    """Create grouped bar chart for CS strategies showing average generation."""
    game_components_range = list(range(20, 101, 10))
    n_strategies = len(strategies)
    n_components = len(game_components_range)
    
    gen_data = {f"BS={bs:.1f}, CS={cs:.1f}": [] for bs, cs, _ in strategies}
    
    baseline_gen_data = []
    print("Processing baseline BS=1.0, CS=0.0, SYS=0.0...")
    for game_components in game_components_range:
        avg_fitness, avg_generation = get_average_fitness_and_generation(
            base_dir, game_components, 1.0, 0.0, 0.0)
        baseline_gen_data.append(avg_generation if avg_generation is not None else 0)
    
    for bs, cs, sys in strategies:
        strategy_label = f"BS={bs:.1f}, CS={cs:.1f}"
        print(f"Processing {strategy_label}...")
        
        for game_components in game_components_range:
            avg_fitness, avg_generation = get_average_fitness_and_generation(
                base_dir, game_components, bs, cs, sys)
            gen_data[strategy_label].append(avg_generation if avg_generation is not None else 0)
    
    fig, ax = plt.subplots(figsize=(14, 9))
    bar_width = 0.15
    positions = np.arange(n_components)
    
    for idx, (bs, cs, _) in enumerate(strategies):
        strategy_label = f"BS={bs:.1f}, CS={cs:.1f}"
        offset = (idx - n_strategies/2 + 0.5) * bar_width
        color = generate_strategy_color_cs(bs, cs)
        ax.bar(positions + offset, gen_data[strategy_label], bar_width, 
               label=strategy_label, color=color)
    
    ax.plot(positions, baseline_gen_data, color='#FF6600', linewidth=2.5, 
            marker='s', markersize=8, label='BS=1.0 (Baseline)', zorder=10)
    
    avg_gen_per_component = []
    for i in range(n_components):
        values_at_component = [gen_data[f"BS={bs:.1f}, CS={cs:.1f}"][i] for bs, cs, _ in strategies]
        avg_gen_per_component.append(np.mean(values_at_component))
    
    ax.plot(positions, avg_gen_per_component, color='#2CCC2C', linewidth=2.5, 
            marker='o', markersize=8, label='Average', zorder=10)
    
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


def plot_generation_bar_chart_sys(base_dir, strategies, save_path=None):
    """Create grouped bar chart for SYS strategies showing average generation."""
    game_components_range = list(range(20, 101, 10))
    n_strategies = len(strategies)
    n_components = len(game_components_range)
    
    gen_data = {f"BS={bs:.1f}, SYS={sys:.1f}": [] for bs, _, sys in strategies}
    
    baseline_gen_data = []
    print("Processing baseline BS=1.0, CS=0.0, SYS=0.0...")
    for game_components in game_components_range:
        avg_fitness, avg_generation = get_average_fitness_and_generation(
            base_dir, game_components, 1.0, 0.0, 0.0)
        baseline_gen_data.append(avg_generation if avg_generation is not None else 0)
    
    for bs, cs, sys in strategies:
        strategy_label = f"BS={bs:.1f}, SYS={sys:.1f}"
        print(f"Processing {strategy_label}...")
        
        for game_components in game_components_range:
            avg_fitness, avg_generation = get_average_fitness_and_generation(
                base_dir, game_components, bs, cs, sys)
            gen_data[strategy_label].append(avg_generation if avg_generation is not None else 0)
    
    fig, ax = plt.subplots(figsize=(14, 9))
    bar_width = 0.15
    positions = np.arange(n_components)
    
    for idx, (bs, _, sys) in enumerate(strategies):
        strategy_label = f"BS={bs:.1f}, SYS={sys:.1f}"
        offset = (idx - n_strategies/2 + 0.5) * bar_width
        color = generate_strategy_color_sys(bs, sys)
        ax.bar(positions + offset, gen_data[strategy_label], bar_width, 
               label=strategy_label, color=color)
    
    ax.plot(positions, baseline_gen_data, color='#FF6600', linewidth=2.5, 
            marker='s', markersize=8, label='BS=1.0 (Baseline)', zorder=10)
    
    avg_gen_per_component = []
    for i in range(n_components):
        values_at_component = [gen_data[f"BS={bs:.1f}, SYS={sys:.1f}"][i] for bs, _, sys in strategies]
        avg_gen_per_component.append(np.mean(values_at_component))
    
    ax.plot(positions, avg_gen_per_component, color='#2C91CC', linewidth=2.5, 
            marker='o', markersize=8, label='Average', zorder=10)
    
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
    
    cs_strategies = [
        (0.9, 0.1, 0.0),
        (0.8, 0.2, 0.0),
        (0.7, 0.3, 0.0),
        (0.6, 0.4, 0.0),
        (0.5, 0.5, 0.0),
    ]
    
    sys_strategies = [
        (0.9, 0.0, 0.1),
        (0.8, 0.0, 0.2),
        (0.7, 0.0, 0.3),
        (0.6, 0.0, 0.4),
        (0.5, 0.0, 0.5),
    ]
    

    
    # Generate CS strategies bar chart
    print("\nGenerating CS strategies bar chart...")
    cs_save_path = os.path.join(output_dir, "Generation_BarChart_CS.svg")
    plot_generation_bar_chart_cs(base_dir, cs_strategies, save_path=cs_save_path)
    
    # Generate SYS strategies bar chart
    print("\nGenerating SYS strategies bar chart...")
    sys_save_path = os.path.join(output_dir, "Generation_BarChart_SYS.svg")
    plot_generation_bar_chart_sys(base_dir, sys_strategies, save_path=sys_save_path)
    
    print("-" * 60)
    print(f"Successfully generated 2 generation bar charts!")
    print(f"Output directory: {output_dir}")


if __name__ == "__main__":
    main()
