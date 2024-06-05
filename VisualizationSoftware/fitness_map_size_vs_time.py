import matplotlib.pyplot as plt
import sys

# fitness_map Sizes and Time
fitness_map_sizes = []
times = []

if len(sys.argv) != 4:
    print("Invalid arguments!")
    exit()

filename1 = sys.argv[1]
filename2 = sys.argv[2]
savefile = sys.argv[3]

# Function to read fitness map sizes vs time from benchmark.cpp save file
def readFile(filename):
    global fitness_map_sizes
    global times
    fitness_map_sizes = []
    times = []

    with open(filename, 'r') as file:
        for line in file:
            line = line.split()
            fitness_map_sizes.append(int(line[0]))
            times.append(float(line[1]))

# Setup plot
plt.figure(figsize=(16,12))
plt.title("Fitness Map Size vs Time")
plt.xlabel("Fitness Map Size (# x #)")
plt.ylabel("Time")

# Get data from 1st file
readFile(filename1)

# Plot data
plt.plot(fitness_map_sizes, times, color='b')

# Get data from file 2
readFile(filename2)

# Plot data
plt.plot(fitness_map_sizes, times, color='r')

# Save figure
plt.savefig(savefile)
