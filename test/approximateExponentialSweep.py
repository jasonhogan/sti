from math import exp

def approximateExponentialSweep(tStart, dt, fStart, fStop, numberOfSteps) :
    tc = dt / exp(1)
    tStep = dt / numberOfSteps
    for i in range(0, numberOfSteps) :
        fInitial = (fStart - fStop) * exp( - i * tStep / tc) + fStop
        fFinal = (fStart - fStop) * exp( - (i+1) * tStep / tc) + fStop
        event(starkShiftingAOM, tStart + i*tStep, ((fInitial, fFinal, tStep - 10*ms), 100, 0))
        event(starkShiftingAOM, tStart + (i+1)*tStep - 1*ms, (fFinal, 99, 0))
        event(starkShiftingAOM, tStart + (i+1)*tStep - 500*us, (fFinal, 100, 0))

    return (tStart + dt)



