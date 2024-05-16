import matplotlib.pyplot as plt
import sys

# Tournament Sizes and Time
tournament_sizes = []
times = []

if len(sys.argv) != 3:
    print("Invalid arguments!")
    exit()

filename = sys.argv[1]
savefile = sys.argv[2]

# Function to read tournament sizes vs time from benchmark.cpp save file
def readFile(filename):
    global tournament_sizes
    global times
    tournament_sizes = []
    times = []

    with open(filename, 'r') as file:
        for line in file:
            line = line.split()
            tournament_sizes.append(int(line[0]))
            times.append(float(line[1]))

# Setup plot
plt.figure(figsize=(16,12))
plt.title("Tournament Size vs Time")
plt.xlabel("tournament_size")
plt.ylabel("Time")

# Get data from file
readFile(filename)

# Plot data
plt.plot(tournament_sizes, times, color='b')

# Save figure
plt.savefig(savefile)
