import matplotlib.pyplot as plt
import sys

# mutation_rate and Time
mutation_rates = []
times = []

if len(sys.argv) != 4:
    print("Invalid arguments!")
    exit()

filename1 = sys.argv[1]
filename2 = sys.argv[2]
savefile = sys.argv[3]

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

# Get data from 1st file
readFile(filename1)

# Plot data
plt.plot(mutation_rates, times, color='b')

# Get data from file 2
readFile(filename2)

# Plot data
plt.plot(mutation_rates, times, color='r')

# Save figure
plt.savefig(savefile)
