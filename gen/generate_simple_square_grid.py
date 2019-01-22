#!/usr/bin/env python3

"""
This script generates a random flow network on a square grid
If drawn on a plane, the source is located in the upper left corner,
the outlet is located in the lower right corner
The graph has a tendency to be right-down oriented, that is each
edge is more likely to point right or down rather than left or up
This increases the probability of the flow existing and being less trivial

Note that the n parameter is the width of the grid, not the amount of vertices,
and the max_edge_cap parameter defines the maximum edge capacity at the source
and outlet.
Other edges' max capacity scales linearly down to the diameter
to increase the probability of the min-cut being less trivial than just the
source or the outlet alone.
"""

import sys
import random
import math

def scaling_factor(x, y, n):
    return abs(0.0 + x + y - n - 2)/(n + 1) + 1.0/(n + 1)

right_down_probability = 0.9
n = int(sys.argv[1])
m = 2 * n * (n - 1)
max_edge_cap = int(sys.argv[2])

print(n, m, 1, n**2)

for i in range(1, n+1):
    for j in range(1, n):
        cap = math.ceil(max_edge_cap * scaling_factor(i, j, n))
        if random.random() < right_down_probability:
            print((i-1) * n + j, (i-1) * n + j + 1, random.randint(0, cap))
        else:
            print((i-1) * n + j + 1, (i-1) * n + j, random.randint(0, cap))

for i in range(1, n):
    for j in range(1, n+1):
        cap = math.ceil(max_edge_cap * scaling_factor(i, j, n))
        if random.random() < right_down_probability:
            print((i-1) * n + j, i * n + j, random.randint(0, cap))
        else:
            print(i * n + j, (i-1) * n + j, random.randint(0, cap))
