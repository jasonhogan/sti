
### This wrapper function for MOT() provides a common location for documenting the MOT parameters. This function should be used by diagnostic files and actual experiments to ensure consistency.

include('motFunction.py')
include('quadCoilControlCircuit.py')

setvar('MOTLoadTime', 1*s ) #2*s
#setvar('varCMOTCurrent', 8)

setvar('MOTcurrent',7.5)    #8
setvar('CMOTcurrent',35)    #35

def makeCMOT(startTime) :
    
    time = MOT(startTime, tClearTime=100*ms, cMOT = True, dtMOTLoad=MOTLoadTime, dtSweepToCMOT = 20*ms, cmotQuadCoilCurrent = 7.5, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = MOTcurrent, dtCMOT = 1*ms, powerReduction = 1, CMOTFrequency = 180, dtNoRepump = 20*ms, repumpAttenuatorVoltage = 0, cmotCurrentRampRate = 1)

    ## Right now MOT() is broken, so the field ramp is done here...
#    quadsDoneTime=setQuadrupoleCurrent(time - 10*ms, CMOTcurrent, applyCurrentRamp = True, usePrecharge = False, startingCurrent = MOTcurrent, rampRate = 2.0)
#    setQuadrupoleCurrent(time - 10*ms, CMOTcurrent, applyCurrentRamp = True, usePrecharge = False, startingCurrent = MOTcurrent, rampRate = 2.0)

    rampQuadrupoleCurrent(startTime = time - 19.9*ms, endTime = time, startCurrent = MOTcurrent, endCurrent = CMOTcurrent, numberOfSteps = 21)

#    time = time - 10*ms + 15*ms        # Strange delay; historical artifact...

#    print "quadsDoneTime " + str(quadsDoneTime/ms) +" time " + str(time/ms) 

#    return max(quadsDoneTime, time)
    return time
