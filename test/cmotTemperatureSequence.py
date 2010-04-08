ms = 1000000.0

variables.append('dtDriftTime')

driftLower = 1*ms
driftUpper = 10*ms
driftRange = driftUpper - driftLower
deltaDrift = 1*ms

numberAverages = 5

for dt in range(0,driftRange/deltaDrift+1) :
    for i in range(0, numberAverages) :
        experiments.append([driftLower+dt*deltaDrift])