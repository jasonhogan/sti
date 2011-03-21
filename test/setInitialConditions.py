#### Set Initial Conditions Function ####

def setInitialConditions(startTime) : 
    event(starkShiftingAOM, startTime, (50,0,0))
    event(probeLightAOM, startTime + 100*us, probeLightOff)             # AOM is off, so no imaging light
    event(opticalPumpingBiasfield, startTime + 10*us, 0) # turn off optical pumping bias field
    event(ddsRfKnife, startTime + 200*us, (180, 0, 0))
    return (startTime + 200*us)