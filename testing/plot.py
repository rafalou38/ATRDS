#!/usr/bin/env python3

import matplotlib.pyplot as plt
import pandas as pd

# Load the data from CSV
data = pd.read_csv("testing/waves.csv")

# Print column names to inspect the structure
print("Columns in CSV:", data.columns)

# Set up a 2x3 subplot grid
fig, axes = plt.subplots(2, 3, figsize=(15, 10))
fig.suptitle("Wave Data")

# Define titles and columns for each subplot
# Updated titles with proper units
titles = [
    "Health Points (hp)", 
    "Health Points per Second (hpps)", 
    "Duration (s)", 
    "Enemies Spawned", 
    "Cumulative Money (argentCumul)"
]
y_labels = [
    "Health Points (hp)", 
    "Health Points per Second (hpps)", 
    "Duration (s)", 
    "Enemies Spawned", 
    "Cumulative Money (argentCumul)"
]
columns = [1, 2, 3, 4, 5]  # Use 1-based indexing if your file includes a header

# Plot each column in a separate subplot
for i, (title, ylabel, col) in enumerate(zip(titles, y_labels, columns)):
    row, col_index = divmod(i, 3)
    # Ensure that the column exists in the dataset
    if col < len(data.columns):
        axes[row, col_index].plot(data.iloc[:, 0], data.iloc[:, col], label=title)
        axes[row, col_index].set_xlabel("Wave")
        axes[row, col_index].set_ylabel(ylabel)
        axes[row, col_index].legend()
    else:
        print(f"Warning: Column index {col} does not exist in the data.")

# Adjust layout to prevent overlap
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.show()
