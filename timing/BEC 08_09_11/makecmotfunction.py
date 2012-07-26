
### This wrapper function for MOT() provides a common location for documenting the MOT parameters. This function should be used by diagnostic files and actual experiments to ensure consistency.

include('motFunction.py')
include('quadCoilControlCircuit.py')

setvar('MOTLoadTime',  10*s ) #6*s
#setvar('varCMOTCurrent', 8)

setvar('MOTcurrent',5.6)    #8
#setvar('actualMOTcurrent',5.6)    #8
setvar('CMOTcurrent', 7)    #35 or 23

def makeCMOT(startTime) :
    
    time = MOT(startTime, tClearTime=100*ms, cMOT = True, dtMOTLoad=MOTLoadTime, dtSweepToCMOT = 30*ms, cmotQuadCoilCurrent = 6.5, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = MOTcurrent, dtCMOT = 1*ms, powerReduction = 1, CMOTFrequency = 180, dtNoRepump = 20*ms, repumpAttenuatorVoltage = 1, cmotCurrentRampRate = 1)

    ## Right now MOT() is broken, so the field ramp is done here...
#    quadsDoneTime=setQuadrupoleCurrent(time - 10*ms, CMOTcurrent, applyCurrentRamp = True, usePrecharge = False, startingCurrent = MOTcurrent, rampRate = 2.0)
#    setQuadrupoleCurrent(time - 10*ms, CMOTcurrent, applyCurrentRamp = True, usePrecharge = False, startingCurrent = MOTcurrent, rampRate = 2.0)

#    rampQuadrupoleCurrent(startTime = time - 19.9*ms, endTime = time, startCurrent = MOTcurrent, endCurrent = CMOTcurrent, numberOfSteps = 21)


########
    setvar('dtQuadStart', 9.99*ms)
    rampQuadrupoleCurrent(startTime = time -10*ms - dtQuadStart, endTime = time-0.51*ms, startCurrent = MOTcurrent, endCurrent = CMOTcurrent, numberOfSteps = 21)




#    time = time - 10*ms + 15*ms        # Strange delay; historical artifact...

#    print "quadsDoneTime " + str(quadsDoneTime/ms) +" time " + str(time/ms) 

#    return max(quadsDoneTime, time)
    return time
