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
        
        # Get lowest and highest fitness
        lowest_fitness = min(fitness_values)
        highest_fitness = max(fitness_values)
        
        return lowest_fitness, highest_fitness
    except Exception as e:
        print(f"Error reading {csv_path}: {e}")
        return None, None


def get_rms_deviation_from_baseline(base_dir, game_components, bs_compare, sys_compare):


    cs = 0.0
    

    bs_baseline = 1.0
    sys_baseline = 0.0

    dir_name = str(game_components // 2)

    baseline_dir = f"BS({bs_baseline:.1f})CS({cs:.1f})SYS({sys_baseline:.1f})"
    compare_dir = f"BS({bs_compare:.1f})CS({cs:.1f})SYS({sys_compare:.1f})"
    
    baseline_gaps = []
    compare_gaps = []
    

    for run in range(1, 11):
        run_dir = f"{dir_name}_{run}"
        

        baseline_csv = os.path.join(base_dir, "Data", dir_name, run_dir, baseline_dir, "BalancingLogResults.csv")
        lowest_baseline, highest_baseline = get_fitness_range_from_csv(baseline_csv)
        

        compare_csv = os.path.join(base_dir, "Data", dir_name, run_dir, compare_dir, "BalancingLogResults.csv")
        lowest_compare, highest_compare = get_fitness_range_from_csv(compare_csv)
        
        if (lowest_baseline is not None and highest_baseline is not None and 
            lowest_compare is not None and highest_compare is not None):
     
            baseline_gap = highest_baseline - lowest_baseline
            compare_gap = highest_compare - lowest_compare
            
            baseline_gaps.append(baseline_gap)
            compare_gaps.append(compare_gap)
    

    if len(baseline_gaps) >= 2:
        squared_differences = [(b - c) ** 2 for b, c in zip(baseline_gaps, compare_gaps)]
        rms_deviation = np.sqrt(np.mean(squared_differences))
        return rms_deviation
    else:
        return None


def generate_strategy_color(bs, sys):

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


def plot_rms_deviation_bar_chart(base_dir, strategies, save_path=None):
    game_components_range = list(range(20, 101, 10))
    
    n_strategies = len(strategies)
    n_components = len(game_components_range)
    
    rms_data = {f"BS={bs:.1f}, SYS={sys:.1f}": [] for bs, sys in strategies}
    
    for bs, sys in strategies:
        strategy_label = f"BS={bs:.1f}, SYS={sys:.1f}"
        print(f"Processing {strategy_label}...")
        
        for game_components in game_components_range:
            rms = get_rms_deviation_from_baseline(base_dir, game_components, bs, sys)
            rms_data[strategy_label].append(rms if rms is not None else 0)
    
    fig, ax = plt.subplots(figsize=(14, 7))
    
    bar_width = 0.15
    positions = np.arange(n_components)
    
    for idx, (bs, sys) in enumerate(strategies):
        strategy_label = f"BS={bs:.1f}, SYS={sys:.1f}"
        offset = (idx - n_strategies/2 + 0.5) * bar_width
        
        color = generate_strategy_color(bs, sys)
        
        ax.bar(positions + offset, rms_data[strategy_label], bar_width, 
               label=strategy_label, color=color)
    
    avg_rms_per_component = []
    for i in range(n_components):
        values_at_component = [rms_data[f"BS={bs:.1f}, SYS={sys:.1f}"][i] for bs, sys in strategies]
        avg_rms_per_component.append(np.mean(values_at_component))
    
    ax.plot(positions, avg_rms_per_component, color="#2C91CC", linewidth=2.5, 
            marker='o', markersize=8, label='Average', zorder=10)
    
    ax.set_xlabel('Number of Game Components', fontsize=12)
    ax.set_ylabel('RMS Deviation of Fitness Gap', fontsize=12)
    ax.set_title('', fontsize=14)
    ax.set_xticks(positions)
    ax.set_xticklabels(game_components_range)
    ax.legend(loc='best', fontsize=10)
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
    

    strategies = [
        (0.9, 0.1),
        (0.8, 0.2),
        (0.7, 0.3),
        (0.6, 0.4),
        (0.5, 0.5),
    ]
    
    print("Generating RMS Deviation Bar Chart:")
    print("Baseline: BS=1.0, CS=0.0, SYS=0.0")
    print("Compared strategies:")
    for bs, sys in strategies:
        print(f"  BS={bs:.1f}, SYS={sys:.1f}")
    print("-" * 60)
    
    # Generate the bar chart
    save_path = os.path.join(base_dir, "RMS_Deviation_BarChart_SYS.svg")
    plot_rms_deviation_bar_chart(base_dir, strategies, save_path=save_path)
    


if __name__ == "__main__":
    main()
