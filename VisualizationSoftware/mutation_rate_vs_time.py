import matplotlib.pyplot as plt
import sys

# mutation_rate and Time
mutation_rates = []
times = []

if len(sys.argv) != 3:
    print("Invalid arguments!")
    exit()

filename = sys.argv[1]
savefile = sys.argv[2]

# Function to read Mutation Rate vs time from benchmark.cpp save file
def readFile(filename):
    global mutation_rates
    global times
    mutation_rates = []
    times = []

    with open(filename, 'r') as file:
        for line in file:
            line = line.split()
            mutation_rates.append(float(line[0]))
            times.append(float(line[1]))

# Setup plot
plt.figure(figsize=(16,12))
plt.title("Mutation Rate vs Time")
plt.xlabel("Mutation Rate")
plt.ylabel("Time")

# Get data from file
readFile(filename)

# Plot data
plt.plot(mutation_rates, times, color='b')

# Save figure
plt.savefig(savefile)
