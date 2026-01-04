import numpy as np
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


def compute_range_statistics(base_dir, bs, cs, sys, game_components_range, label=""):

    # Collect range data
    ranges = []
    game_components_list = []
    
    if label:
        print(f"\n{label}")
    print(f"\nData Collection for BS={bs:.1f}, CS={cs:.1f}, SYS={sys:.1f}")
    print("-" * 70)
    print(f"{'i':<5} {'Game Components':<20} {'Range R_i':<15}")
    print("-" * 70)
    
    for i, game_components in enumerate(game_components_range, start=1):
        avg_lowest, avg_highest = get_average_fitness_range(base_dir, game_components, bs, cs, sys)
        if avg_lowest is not None and avg_highest is not None:
            range_value = avg_highest - avg_lowest
            ranges.append(range_value)
            game_components_list.append(game_components)
            print(f"{i:<5} {game_components:<20} {range_value:.6f}")
        else:
            print(f"{i:<5} {game_components:<20} {'No data':<15}")
    
    if not ranges:
        print("\nNo data available for analysis")
        return None
    
    # 1. Compute average range
    avg_range = np.mean(ranges)
    
    # 2. Compute MAPE
    absolute_percentage_errors = []
    for R_i in ranges:
        ape = abs(R_i - avg_range) / avg_range * 100
        absolute_percentage_errors.append(ape)
    
    mape = np.mean(absolute_percentage_errors)
    
    # Calculate additional statistics
    min_range = min(ranges)
    max_range = max(ranges)
    std_range = np.std(ranges, ddof=1)  # Sample standard deviation
    
    # Min and max percentage deviations
    min_deviation_pct = (min_range - avg_range) / avg_range * 100
    max_deviation_pct = (max_range - avg_range) / avg_range * 100
    
    results = {
        'ranges': ranges,
        'game_components': game_components_list,
        'avg_range': avg_range,
        'mape': mape,
        'min_range': min_range,
        'max_range': max_range,
        'std_range': std_range,
        'min_deviation_pct': min_deviation_pct,
        'max_deviation_pct': max_deviation_pct,
        'absolute_percentage_errors': absolute_percentage_errors
    }
    
    return results


def print_results(results, bs, cs, sys, label=""):

    if results is None:
        return
    
    print("\n" + "=" * 70)
    if label:
        print(f"STATISTICAL ANALYSIS RESULTS - {label}")
    else:
        print("STATISTICAL ANALYSIS RESULTS")
    print("=" * 70)
    print(f"Configuration: BS={bs:.1f}, CS={cs:.1f}, SYS={sys:.1f}")
    print("-" * 70)
    
    print("\n1. Average Range (R̄)")
    print(f"   Formula: R̄ = (1/n) × Σ R_i")
    print(f"   Result: R̄ ≈ {results['avg_range']:.5f}")
    
    print("\n2. Mean Absolute Percentage Error (MAPE)")
    print(f"   Formula: MAPE = (1/n) × Σ |R_i - R̄| / R̄ × 100%")
    print(f"   Result: MAPE ≈ {results['mape']:.1f}%")
    
    print("\n3. Additional Statistics")
    print(f"   Min Range: {results['min_range']:.6f}")
    print(f"   Max Range: {results['max_range']:.6f}")
    print(f"   Std Dev:   {results['std_range']:.6f}")
    print(f"   Range Deviation: {results['min_deviation_pct']:.1f}% to {results['max_deviation_pct']:.1f}%")
    
    print("\n4. Interpretation")
    print(f'   "On average, the range varies by about {results["mape"]:.1f}% from its mean."')
    
    print("\n5. Report Statement")
    print(f'   "The average performance range across component sizes was {results["avg_range"]:.3f}.')
    print(f'   Individual configurations deviated from this average by {results["min_deviation_pct"]:.0f}% to')
    print(f'   {results["max_deviation_pct"]:.0f}%, with a mean absolute deviation of approximately {results["mape"]:.1f}%."')
    
    print("\n" + "=" * 70)


def main():

    # Get the base directory (parent of this script)
    base_dir = Path(__file__).parent
    
    # Configuration: Set your BS, CS, SYS values here
    BS = 1.0
    CS = 0.0
    SYS = 0.0
    
    print("\n" + "=" * 70)
    print("SEGMENTED ANALYSIS: TOTAL, 20-50, AND 50-100")
    print("=" * 70)
    
    # 1. Total range (20-100)
    print("\n" + "#" * 70)
    print("# TOTAL: Game Components 20-100")
    print("#" * 70)
    total_range = range(20, 101, 10)
    results_total = compute_range_statistics(base_dir, BS, CS, SYS, total_range, "TOTAL (20-100)")
    print_results(results_total, BS, CS, SYS, "TOTAL (20-100)")
    
    # 2. Range 20-50
    print("\n\n" + "#" * 70)
    print("# SEGMENT 1: Game Components 20-50")
    print("#" * 70)
    range_20_50 = range(20, 51, 10)
    results_20_50 = compute_range_statistics(base_dir, BS, CS, SYS, range_20_50, "SEGMENT 20-50")
    print_results(results_20_50, BS, CS, SYS, "SEGMENT 20-50")
    
    # 3. Range 50-100
    print("\n\n" + "#" * 70)
    print("# SEGMENT 2: Game Components 50-100")
    print("#" * 70)
    range_50_100 = range(50, 101, 10)
    results_50_100 = compute_range_statistics(base_dir, BS, CS, SYS, range_50_100, "SEGMENT 50-100")
    print_results(results_50_100, BS, CS, SYS, "SEGMENT 50-100")
    
    # Summary comparison
    print("\n\n" + "=" * 70)
    print("SUMMARY COMPARISON")
    print("=" * 70)
    if results_total and results_20_50 and results_50_100:
        print(f"\n{'Metric':<30} {'Total':<15} {'20-50':<15} {'50-100':<15}")
        print("-" * 70)
        print(f"{'Average Range (R̄)':<30} {results_total['avg_range']:.5f}{'':<8} {results_20_50['avg_range']:.5f}{'':<8} {results_50_100['avg_range']:.5f}")
        print(f"{'MAPE (%)':<30} {results_total['mape']:.1f}{'':<12} {results_20_50['mape']:.1f}{'':<12} {results_50_100['mape']:.1f}")
        print(f"{'Min Range':<30} {results_total['min_range']:.6f}{'':<8} {results_20_50['min_range']:.6f}{'':<8} {results_50_100['min_range']:.6f}")
        print(f"{'Max Range':<30} {results_total['max_range']:.6f}{'':<8} {results_20_50['max_range']:.6f}{'':<8} {results_50_100['max_range']:.6f}")
        print(f"{'Std Dev':<30} {results_total['std_range']:.6f}{'':<8} {results_20_50['std_range']:.6f}{'':<8} {results_50_100['std_range']:.6f}")
        print("=" * 70)


if __name__ == "__main__":
    main()
