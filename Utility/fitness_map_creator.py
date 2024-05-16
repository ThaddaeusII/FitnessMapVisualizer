import sys

if len(sys.argv) < 2:
    print("Make sure you input filename to save to!")
    exit()

# File to save to
filename = sys.argv[1]

# Get size of fitness map
xsize = 10
ysize = 10

if len(sys.argv) == 4:
    xsize = int(sys.argv[2])
    ysize = int(sys.argv[3])

# Change this to get maps
def fitness_function(x, y):
    #xpos = (x - xsize / 2)**2
    #ypos = (y - ysize / 2)**2
    #if (xpos + ypos > 9 and xpos + ypos < 25):
    #    return 1
    return 1 if (x + y == 0) else 0

fitness_map = []
for i in range(ysize):
    row = []
    for j in range(xsize):
        row.append(fitness_function(i, j))
    fitness_map.append(row)

with open(filename, 'w') as file:
    file.write(f"{xsize} {ysize} 5 1\n")
    for i in range(ysize):
        for j in range(xsize):
            file.write(f"{fitness_map[i][j]} ")
        file.write('\n')
