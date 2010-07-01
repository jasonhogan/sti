variables.append('piezoVoltage')
variables.append('diodeCurrent')
voltageLower=50.8
voltageUpper=52.9
voltageRange=voltageUpper - voltageLower
deltaVoltage=0.1

currentLower=47.5
currentUpper=47.9
currentRange=currentUpper - currentLower
deltaCurrent = 0.1

for voltage in range(0,voltageRange/deltaVoltage+1) :
    for current in range(0, currentRange/deltaCurrent + 1) :
        experiments.append([voltageLower+voltage*deltaVoltage, currentLower+current*deltaCurrent])