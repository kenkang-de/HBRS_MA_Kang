import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import os
from pathlib import Path


def get_highest_generation_from_csv(csv_path):
    if not os.path.exists(csv_path):
        return None
    
    try:

        df = pd.read_csv(csv_path, converters={'Entry1': str, 'Generation': str})
        
        df = df.dropna(subset=['Entry1'])
        df = df[df['Entry1'].str.strip() != '']
        
        if len(df) == 0:
            return None
        
        # Get the last row with actual data (highest generation)
        last_row = df.iloc[-1]
        generation_number = int(last_row['Generation'])
        
        return generation_number
    except Exception as e:
        print(f"Error reading {csv_path}: {e}")
        return None


def get_all_generations_for_population(base_dir, population_size):

    generation_values = []
    

    for run in range(1, 11):
        run_dir = f"50_{run}"
        pop_dir = f"POP({population_size})"
        csv_path = os.path.join(base_dir, "Data", "Population_noDOC", run_dir, pop_dir, "BalancingLogResults.csv")
        
        generation = get_highest_generation_from_csv(csv_path)
        if generation is not None:
            generation_values.append(generation)
    
    return generation_values


def plot_population_whisker(base_dir, save_path=None):
    population_sizes = [50, 60, 70, 80, 90, 100]
    data_for_plot = []
    labels = []
    
    for pop_size in population_sizes:
        gen_values = get_all_generations_for_population(base_dir, pop_size)
        if gen_values:
            data_for_plot.append(gen_values)
            labels.append(str(pop_size))
            print(f"Population {pop_size}: Count={len(gen_values)}, "
                  f"Mean={np.mean(gen_values):.2f}, Std={np.std(gen_values):.2f}")
        else:
            print(f"Population {pop_size}: No data available")
    
    if not data_for_plot:
        return
    
    # Create the whisker (box) plot
    plt.figure(figsize=(10, 6))
    bp = plt.boxplot(data_for_plot, labels=labels, patch_artist=True, 
                     showmeans=True, meanline=True, widths=0.5)
    
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
    
    plt.xlabel('Chromosome Population Size', fontsize=12)
    plt.ylabel('Highest Generation', fontsize=12)
    plt.title('Highest Generation Distribution by Population Size', fontsize=14)
    plt.grid(True, alpha=0.3, axis='y')
    
    # Add legend
    plt.plot([], [], 'r-', linewidth=2, label='Median')
    plt.plot([], [], 'g-', linewidth=2, label='Mean')
    plt.legend()
    
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, format='svg', bbox_inches='tight')
        print(f"Saved: {save_path}")
        plt.close()
    else:
        plt.show()


def main():
    base_dir = Path(__file__).parent
    
    # Create output directory
    output_dir = os.path.join(base_dir, "GenerationGraph")
    os.makedirs(output_dir, exist_ok=True)
    
    print("Generating population vs generation whisker plot:")
    print("-" * 60)
    
    # Generate plot
    save_path = os.path.join(output_dir, "Population_noDOC_vs_Generation_Whisker.svg")
    plot_population_whisker(base_dir, save_path=save_path)
    
    print("-" * 60)
    print(f"Successfully generated population whisker plot!")
    print(f"Output directory: {output_dir}")


if __name__ == "__main__":
    main()
