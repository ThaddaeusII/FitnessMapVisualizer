import matplotlib.pyplot as plt
import sys

# Tournament Sizes and Time
tournament_sizes = []
times = []

if len(sys.argv) != 4:
    print("Invalid arguments!")
    exit()

filename1 = sys.argv[1]
filename2 = sys.argv[2]
savefile = sys.argv[3]

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

# Get data from 1st file
readFile(filename1)

# Plot data
plt.plot(tournament_sizes, times, color='b')

# Get data from file 2
readFile(filename2)

# Plot data
plt.plot(tournament_sizes, times, color='r')

# Save figure
plt.savefig(savefile)
