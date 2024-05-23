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
    cval = (x - 50)**2 + (y - 50)**2
    if cval >= 400 and cval <= 900:
        return 10
    return 1

fitness_map = []
max_val = 0.0
for i in range(ysize):
    row = []
    for j in range(xsize):
        row.append(fitness_function(i, j))
        if max_val < fitness_function(i, j):
            max_val = float(fitness_function(i, j))
    fitness_map.append(row)

with open(filename, 'w') as file:
    file.write(f"{xsize} {ysize} {max_val} {max_val / 10.0}\n")
    for i in range(ysize):
        for j in range(xsize):
            file.write(f"{fitness_map[i][j]} ")
        file.write('\n')
