import sys
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.animation import PillowWriter
import numpy as np

# Initialize globals
x_data = np.empty(1)
y_data = np.empty(1)
x_fitness = np.empty(1)
y_fitness = np.empty(1)
fitness = np.empty(1)
maxfit = 1
fitspace = 0.1
jitter = 0.2

if len(sys.argv) != 8:
    print("Invalid Number of Arguments")
    exit()

loaddirectory = sys.argv[1]
savefile = sys.argv[2]
xlower = 0
xupper = int(sys.argv[3])
ylower = 0
yupper = int(sys.argv[4])
generations = int(sys.argv[5])
fps = int(sys.argv[6])
fitnessfile = sys.argv[7]

def getFitness(filename):
    global x_fitness
    global y_fitness
    global fitness
    global maxfit
    global fitspace
    with open(filename, 'r') as file:
        line = file.readline().split()
        x = np.arange(- 1.0 / 3.0, float(line[0]) - 1.0 / 3.0, 1.0 / 3.0)
        y = np.arange(- 1.0 / 3.0, float(line[1]) - 1.0 / 3.0, 1.0 / 3.0)
        maxfit = int(line[2])
        fitspace = float(line[3])
        x_fitness, y_fitness = np.meshgrid(x, y)
        fitness = np.zeros(x_fitness.shape)
        for i in range(fitness.shape[0]):
            if i % 3 == 0:
                line = file.readline().split()
            for j in range(fitness.shape[1]):
                fitness[i][j] = float(line[j // 3])
        '''for i in range(fitness.shape[0]):
            if i % 3 == 0:
                print('-'*fitness.shape[1]*3)
            for j in range(fitness.shape[1]):
                if j % 3 == 0:
                    print('|', end=' ')
                print(int(fitness[i][j]), end=' ')
            print('|')
        print('-'*fitness.shape[1]*3)
        for i in range(fitness.shape[1]):
            print(fitness[i])'''

def readFile(filename):
    global x_data
    global y_data
    global jitter
    with open(filename, 'r') as file:
        line = file.readline().split();
        gen = int(line[1])
        line = file.readline();
        line = file.readline();
        x_data = np.zeros(gen)
        y_data = np.zeros(gen)
        i = 0
        for line in file:
            line = line.split()
            x_data[i] = int(line[0])
            y_data[i] = int(line[1])
            i += 1

    # Add jitter to data
    x_data = x_data + np.random.normal(0, jitter, x_data.shape) 
    y_data = y_data + np.random.normal(0, jitter, y_data.shape) 

# Get fitness map
getFitness(fitnessfile)

# Get figure and axis
fig, ax = plt.subplots(figsize=(16,12))

# Setup axis plot
ax.set_xlabel("X Gene")
ax.set_ylabel("Y Gene")
title = ax.set_title("Generation=0")
ax.set_xlim(xlower, xupper - 1)
ax.set_ylim(ylower, yupper - 1)
ax.set_xticks(np.arange(xlower, xupper, 1))
ax.set_yticks(np.arange(ylower, yupper, 1))
ax.grid(True)
ax.invert_yaxis()

# Plot contour lines
lvls = np.arange(0, maxfit + 1, fitspace)
contour = ax.contour(x_fitness, y_fitness, fitness, levels = lvls, colors='black')
ax.clabel(contour, inline=True, fontsize=8)

# Fill contour zones
contourf = ax.contourf(x_fitness, y_fitness, fitness, levels = lvls, cmap = 'viridis', extend='both', alpha = 0.5)

# Add a colorbar
cbar = plt.colorbar(contourf, ax=ax)
cbar.set_label('Height')

# Initial scatter
filename = f"{loaddirectory}gen_0.txt"
readFile(filename)
scat = ax.scatter(x_data, y_data, marker = 'o', color = 'r')

# Update function for animated graph, loads each population from file
def animate(frame):
    global x_data
    global y_data
    filename = f"{loaddirectory}/gen_{frame + 1}.txt"
    readFile(filename)
    if (frame + 1) % 100 == 0:
        print(f"Scatter plot frame {frame + 1} complete!")
    scat.set_offsets(np.c_[x_data, y_data])
    title.set_text(f"Generation={frame + 1}")

# Create animation
ani = FuncAnimation(fig, # Figure to animate on
                    animate, # Function to select next frame
                    repeat=True, # Repeat GIF
                    frames=generations, # One frame per generation
                    interval=50, # Doesn't matter, gets overwritten by PillowWriter
                    blit = False) # No blit allowed, causes error with animation

# Save as GIF
writer = PillowWriter(fps=fps,
                        metadata=dict(artist='Me'),
                        bitrate=1800)
ani.save(savefile, writer=writer)
