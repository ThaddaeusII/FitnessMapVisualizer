import matplotlib.pyplot as plt
import sys

# Population and Time
populations = []
times = []

if len(sys.argv) != 3:
    print("Invalid arguments!")
    exit()

filename = sys.argv[1]
savefile = sys.argv[2]

# Function to read population vs time from benchmark.cpp save file
def readFile(filename):
    global populations
    global times
    populations = []
    times = []

    with open(filename, 'r') as file:
        for line in file:
            line = line.split()
            populations.append(int(line[0]))
            times.append(float(line[1]))

# Setup plot
plt.figure(figsize=(16,12))
plt.title("Population vs Time")
plt.xlabel("Population")
plt.ylabel("Time")

# Get data from file
readFile(filename)

# Plot data
plt.plot(populations, times, color='b')

# Save figure
plt.savefig(savefile)
