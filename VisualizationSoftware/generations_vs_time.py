import matplotlib.pyplot as plt
import sys

# Generations and Time
generations = []
times = []

if len(sys.argv) != 3:
    print("Invalid arguments!")
    exit()

filename = sys.argv[1]
savefile = sys.argv[2]

# Function to read generations vs time from benchmark.cpp save file
def readFile(filename):
    global generations
    global times
    generations = []
    times = []

    with open(filename, 'r') as file:
        for line in file:
            line = line.split()
            generations.append(int(line[0]))
            times.append(float(line[1]))

# Setup plot
plt.figure(figsize=(16,12))
plt.title("Generations vs Time")
plt.xlabel("Generations")
plt.ylabel("Time")

# Get data from file
readFile(filename)

# Plot data
plt.plot(generations, times, color='b')

# Save figure
plt.savefig(savefile)
