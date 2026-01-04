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


def get_rms_deviation_from_baseline_cs(base_dir, game_components, bs_compare, cs_compare):

    sys = 0.0
    bs_baseline = 1.0
    cs_baseline = 0.0
    
    dir_name = str(game_components // 2)
    baseline_dir = f"BS({bs_baseline:.1f})CS({cs_baseline:.1f})SYS({sys:.1f})"
    compare_dir = f"BS({bs_compare:.1f})CS({cs_compare:.1f})SYS({sys:.1f})"
    
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


def get_rms_deviation_from_baseline_sys(base_dir, game_components, bs_compare, sys_compare):

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


def plot_average_rms_comparison(base_dir, save_path=None):
    game_components_range = list(range(20, 101, 10))
    
    # Define CS and SYS strategies
    cs_strategies = [(0.9, 0.1), (0.8, 0.2), (0.7, 0.3), (0.6, 0.4), (0.5, 0.5)]
    sys_strategies = [(0.9, 0.1), (0.8, 0.2), (0.7, 0.3), (0.6, 0.4), (0.5, 0.5)]
    
    avg_rms_cs = []
    for game_components in game_components_range:
        rms_values = []
        for bs, cs in cs_strategies:
            rms = get_rms_deviation_from_baseline_cs(base_dir, game_components, bs, cs)
            if rms is not None:
                rms_values.append(rms)
        avg_rms_cs.append(np.mean(rms_values) if rms_values else 0)
    
    # Calculate average RMS for SYS strategies
    avg_rms_sys = []
    for game_components in game_components_range:
        rms_values = []
        for bs, sys in sys_strategies:
            rms = get_rms_deviation_from_baseline_sys(base_dir, game_components, bs, sys)
            if rms is not None:
                rms_values.append(rms)
        avg_rms_sys.append(np.mean(rms_values) if rms_values else 0)
    
    # Create plot
    fig, ax = plt.subplots(figsize=(12, 7))
    
    positions = np.arange(len(game_components_range))
    
    # Plot CS average line
    ax.plot(positions, avg_rms_cs, color='#2CCC2C', linewidth=2.5, 
            marker='o', markersize=8, label='Average CS Strategies', zorder=10)
    
    # Plot SYS average line
    ax.plot(positions, avg_rms_sys, color='#2C91CC', linewidth=2.5, 
            marker='s', markersize=8, label='Average SYS Strategies', zorder=10)
    
    # Configure plot
    ax.set_xlabel('Number of Game Components', fontsize=12)
    ax.set_ylabel('Average RMS Deviation of Fitness Gap', fontsize=12)
    ax.set_title('', fontsize=14)
    ax.set_xticks(positions)
    ax.set_xticklabels(game_components_range)
    ax.legend(loc='best', fontsize=11)
    ax.grid(True, alpha=0.3)
    
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
    
    # Generate the comparison plot
    save_path = os.path.join(base_dir, "RMS_Average_CS_vs_SYS.svg")
    plot_average_rms_comparison(base_dir, save_path=save_path)
    


if __name__ == "__main__":
    main()
