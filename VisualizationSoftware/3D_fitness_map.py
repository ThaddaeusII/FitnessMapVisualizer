import sys
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.animation import FuncAnimation
from matplotlib.animation import PillowWriter
import numpy as np

if len(sys.argv) != 7:
    print("Invalid Number of Arguments")
    exit()

loaddirectory = sys.argv[1]
savefile = sys.argv[2]
xlower = 0
xupper = int(sys.argv[3])
ylower = 0
yupper = int(sys.argv[4])
generations = int(sys.argv[5])
frametime = int(sys.argv[6])

def readFile(filename, x_data, y_data, z_data):
    # Data for x gene, y gene and fitness
    x_data = []
    y_data = []
    z_data = []
    
    with open(filename, 'r') as file:
        # Population size
        line = file.readline().split();
        N = line[1]
    
        # Mutation chance
        line = file.readline().split();
        M = line[1]

        # Generation
        line = file.readline().split();
        G = line[1]
    
        for line in file:
            line = line.split()
            x = int(line[0])
            y = int(line[1])
            fit = float(line[2])

            x_data.append(x)
            y_data.append(y)
            z_data.append(fit)

# Create fitness plane
xx, yy = np.meshgrid(range(xlower, xupper), range(ylower, yupper))

# Fitness function
z1 = np.exp(-0.05 * ((xx - 5)**2 + (yy - 5)**2))
z2 = np.exp(-0.05 * ((xx + 3)**2 + (yy + 3)**2))
zz = z1 + z2

# Setup 3D plot
fig = plt.figure(figsize=(16,12))
ax = fig.add_subplot(111, projection='3d')

# Add labels and title
title = ax.set_title("Population=0")
ax.set_xlabel('X Gene')
ax.set_ylabel('Y Gene')
ax.set_zlabel('Fitness')

# Add scaling
ax.set_xlim3d(xlower, xupper)
ax.set_ylim3d(xlower, xupper)
ax.set_zlim3d(0, 1)

# Initial scatter
filename = f"{loaddirectory}gen_0.txt"
x_data = []
y_data = []
z_data = []
readFile(filename, x_data, y_data, z_data)
ax.plot_surface(xx, yy, zz, alpha=0.2)
scat = ax.scatter(x_data, y_data, z_data, marker = 'o', color = 'r')

# Update function for animated graph, loads each population from file
def animate(frame):
    x_data = []
    y_data = []
    z_data = []
    filename = f"{loaddirectory}/gen_{frame + 1}.txt"
    with open(filename, 'r') as file:
        line = file.readline().split();
        line = file.readline().split();
        line = file.readline().split();
        for line in file:
            line = line.split()
            x = int(line[0])
            y = int(line[1])
            fit = float(line[2])
            x_data.append(x)
            y_data.append(y)
            z_data.append(fit)
    if (frame + 1) % 100 == 0:
        print(f"Scatter plot frame {frame + 1} complete!")
    scat._offsets3d = (x_data, y_data, z_data)
    title.set_text(f"Generation={frame + 1}")

# Create animation
ani = FuncAnimation(fig, animate, repeat=True, frames=generations, interval=frametime, blit = False)

# Save as GIF
writer = PillowWriter(fps=30,
                        metadata=dict(artist='Me'),
                        bitrate=1800)
ani.save(savefile, writer=writer)
