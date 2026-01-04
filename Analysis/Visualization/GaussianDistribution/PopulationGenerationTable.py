import numpy as np
import pandas as pd
import os
from pathlib import Path


def get_highest_generation_from_csv(csv_path):
    if not os.path.exists(csv_path):
        return None
    
    try:
        # Read the CSV file
        df = pd.read_csv(csv_path, converters={'Entry1': str, 'Generation': str})
        
        # Remove rows where Entry1 is NaN or empty
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


def get_highest_fitness_from_csv(csv_path):
    if not os.path.exists(csv_path):
        return None
    
    try:
        # Read the CSV file
        df = pd.read_csv(csv_path, converters={'Entry1': str})
        
        # Remove rows where Entry1 is NaN or empty
        df = df.dropna(subset=['Entry1'])
        df = df[df['Entry1'].str.strip() != '']
        
        if len(df) == 0:
            return None
        
        fitness_values = []
        for entry1_str in df['Entry1']:
            try:
                fitness_value = float(entry1_str.split()[0])
                fitness_values.append(fitness_value)
            except:
                continue
        
        if not fitness_values:
            return None
        
        # Get the highest fitness
        highest_fitness = max(fitness_values)
        
        return highest_fitness
    except Exception as e:
        print(f"Error reading {csv_path}: {e}")
        return None


def get_all_generations_for_population(base_dir, population_size):
    generation_values = []
    fitness_values = []
    
    # Iterate through runs 1 to 10
    for run in range(1, 11):
        run_dir = f"50_{run}"
        pop_dir = f"POP({population_size})"
        csv_path = os.path.join(base_dir, "Data", "Population", run_dir, pop_dir, "BalancingLogResults.csv")
        
        generation = get_highest_generation_from_csv(csv_path)
        fitness = get_highest_fitness_from_csv(csv_path)
        
        if generation is not None:
            generation_values.append(generation)
        if fitness is not None:
            fitness_values.append(fitness)
    
    return generation_values, fitness_values


def generate_population_table(base_dir, save_path=None):
    population_sizes = [50, 60, 70, 80, 90, 100]
    
    # Prepare data
    data = []
    
    print("Processing population data...")
    print("-" * 80)
    
    for pop_size in population_sizes:
        gen_values, fit_values = get_all_generations_for_population(base_dir, pop_size)
        
        if gen_values:
            avg_gen = np.mean(gen_values)
            max_gen = np.max(gen_values)
            min_gen = np.min(gen_values)
            std_gen = np.std(gen_values)
            
            # Calculate fitness statistics
            avg_fitness = np.mean(fit_values) if fit_values else None
            fitness_str = f"{avg_fitness:.6f}" if avg_fitness else "N/A"
            
            data.append({
                'Chromosome Population': pop_size,
                'Average Highest Generation': round(avg_gen, 2),
                'Average Highest Fitness': round(avg_fitness, 6) if avg_fitness else None,
                'Max Generation': max_gen,
                'Min Generation': min_gen,
                'Std Deviation': round(std_gen, 2),
                'Count': len(gen_values)
            })
            
            print(f"Population {pop_size}: Avg Gen={avg_gen:.2f}, Avg Fitness={fitness_str}, Max={max_gen}, Min={min_gen}, Std={std_gen:.2f}")
        else:
            print(f"Population {pop_size}: No data available")
    
    # Create DataFrame
    df = pd.DataFrame(data)
    
    # Save to CSV
    if save_path:
        df.to_csv(save_path, index=False)
        print(f"\nSaved table to: {save_path}")
        print("You can open this CSV in Excel and copy-paste to Word.")
    
    # Print formatted table
    print("\n" + "=" * 80)
    print("TABLE (Copy from console or use CSV file):")
    print("=" * 80)
    print(df.to_string(index=False))
    print("=" * 80)
    
    return df


def main():
    """
    Main function to generate population statistics table.
    """
    # Get the base directory (parent of this script)
    base_dir = Path(__file__).parent
    
    # Create output directory
    output_dir = os.path.join(base_dir, "GenerationGraph")
    os.makedirs(output_dir, exist_ok=True)
    
    print("Generating population statistics table:")
    print("-" * 60)
    
    # Generate table
    save_path = os.path.join(output_dir, "Population_Generation_Table.csv")
    df = generate_population_table(base_dir, save_path=save_path)
    
    print("-" * 60)
    print(f"Successfully generated table!")
    print(f"Output file: {save_path}")


if __name__ == "__main__":
    main()
