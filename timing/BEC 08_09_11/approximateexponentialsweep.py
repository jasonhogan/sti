from math import exp

def approximateExponentialSweep(dt, fStart, fStop, numberOfSteps, tcFactor = 1) :
    commandList = []
    tc = dt / exp(tcFactor)
    tStep = dt / numberOfSteps
    f0 = (fStart * exp( -dt / tc ) - fStop) / (exp( -dt / tc ) - 1)
    for i in range(0, numberOfSteps) :
        fInitial = (fStart - f0) * exp( - i * tStep / tc) + f0
        fFinal = (fStart - f0) * exp( - (i+1) * tStep / tc) + f0
        commandList.append((fInitial, fFinal, tStep))
        

#        event(starkShiftingAOM, tStart + i*tStep, ((fInitial, fFinal, tStep - 10*ms), 100, 0))
#        event(starkShiftingAOM, tStart + (i+1)*tStep - 1*ms, (fFinal, 99, 0))
#        event(starkShiftingAOM, tStart + (i+1)*tStep - 500*us, (fFinal, 100, 0))

    return commandList


def approximateExponentialSweepHP(startTime, dt, fStart, fStop, numberOfSteps, tcFactor = 1) :
    cutTime = startTime
    tc = dt / exp(tcFactor)
    tStep = dt / numberOfSteps
    f0 = (fStart * exp( -dt / tc ) - fStop) / (exp( -dt / tc ) - 1)
    for i in range(-1, numberOfSteps-1) :
        fInitial = (fStart - f0) * exp( - i * tStep / tc) + f0
        fFinal = (fStart - f0) * exp( - (i+1) * tStep / tc) + f0
        event(rfKnifeFrequency, cutTime, resFreq - fFinal*1e6)
        cutTime = cutTime + (i+2)*tStep
        
    return cutTime

def approximateExponentialSweep2Extended(dt, fStart, fStop, fLimit, numberOfSteps, dtStop) :
    base=(fStop - fLimit) / (fStart - fLimit)
    FreqAtTstop=(fStart - fLimit) * base**(dtStop/dt) + fLimit
    return approximateExponentialSweep2(dtStop, fStart, FreqAtTstop, fLimit, numberOfSteps)

def approximateExponentialSweep2(dt, fStart, fStop, fLimit, numberOfSteps) :
    commandList = []
    base=(fStop - fLimit) / (fStart - fLimit)
    tStep = dt / numberOfSteps
    for i in range(0, numberOfSteps) :
        fInitial = (fStart - fLimit) * ( base**( i * tStep / dt) ) + fLimit
        fFinal = (fStart - fLimit) * ( base**( (i+1) * tStep / dt) ) + fLimit
        commandList.append((fInitial, fFinal, tStep))

    return commandList
