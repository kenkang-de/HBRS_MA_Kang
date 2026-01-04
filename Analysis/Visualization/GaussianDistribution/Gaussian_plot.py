import numpy as np
import matplotlib.pyplot as plt


μ = 0.0       
σ = 1.2      

x = np.linspace(-5, 5, 1000)


pdf = (1 / (σ * np.sqrt(2 * np.pi))) * np.exp(-0.5 * ((x - μ) / σ) ** 2)

plt.plot(x, pdf)
plt.title("Gaussian Mutation Distribution (μ=0, σ=1.2)")
plt.xlabel("Mutation Δ (change in gene value)")
plt.ylabel("Probability density")
plt.grid(True)
plt.savefig("gaussian_distribution.svg", format='svg', bbox_inches='tight')
plt.show()
