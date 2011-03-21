ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

variables.append('scaleFactor')

rampUp = 10
hold = 10
rampDown = 10

#experiments.append([float(0)])

for i in range(0, rampUp) :
    experiments.append([float(i)/rampUp])

for i in range(0, hold):
    experiments.append([float(1)])

for i in range (rampDown-1, -1, -1):
    experiments.append([float(i)/rampDown])
