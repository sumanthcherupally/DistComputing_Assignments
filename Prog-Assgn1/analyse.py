import numpy as np

file = open("var-log.txt")
lines = file.read()
l = []
k = {}
lines = lines.split("\n")
for i in range(5):
    k[i] = []

for e in lines:
    # l = e.split(":")
    y = e.split(":")
    k[int(y[0])-1].append(int(y[2]))
for i in range(5):
    print(np.var(k[i]))