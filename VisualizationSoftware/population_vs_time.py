import matplotlib.pyplot as plt
import sys

# Population and Time
populations = []
times = []

if len(sys.argv) != 4:
    print("Invalid arguments!")
    exit()

filename1 = sys.argv[1]
filename2 = sys.argv[2]
savefile = sys.argv[3]

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

# Get data from 1st file
readFile(filename1)

# Plot data
plt.plot(populations, times, color='b')

# Get data from file 2
readFile(filename2)

# Plot data
plt.plot(populations, times, color='r')

# Save figure
plt.savefig(savefile)
