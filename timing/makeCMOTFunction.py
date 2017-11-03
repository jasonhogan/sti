
### This wrapper function for MOT() provides a common location for documenting the MOT parameters. This function should be used by diagnostic files and actual experiments to ensure consistency.

include('motFunction.py')
include('quadCoilControlCircuit.py')
#include('experimentalParameters.py')

#setvar('varCMOTCurrent', 8)

setvar('MOTcurrent', 6.7) #6.7   #5.53  #7.83
#setvar('actualMOTcurrent',5.6)    #8
setvar('CMOTcurrent', 0)    #7   35 or 23
#setvar('CMOTFreq', 175) #175
#setvar('cMOT87Detuning', 35)#25 #in MHz; positive is red #46 for 87 only
#setvar('cMOT85Detuning', 20) #40 #46#in MHz; positive is red

def makeCMOT(startTime, Rb87 = True, Rb85 = False) :
    
    time = MOT(startTime, rb87 = Rb87, rb85 = Rb85, tClearTime=100*ms, cMOT = True, dtMOTLoad=MOTLoadTime, dtSweepToCMOT = 30*ms, cmotQuadCoilCurrent = 6.5, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = MOTcurrent, dtCMOT = 1*ms, powerReduction = 1.0, CMOT87Frequency = dds87Resonance + cMOT87Detuning,  CMOT85Frequency = dds85Resonance + cMOT85Detuning/4, dtNoRepump = 10*ms, repumpAttenuatorVoltage = 1, cmotCurrentRampRate = 1)

#    time = MOT(startTime, tClearTime=100*ms, cMOT = True, dtMOTLoad=MOTLoadTime, dtSweepToCMOT = 30*ms, cmotQuadCoilCurrent = 6.5, dtMolasses = 0*ms, rapidOff = False, motQuadCoilCurrent = MOTcurrent, dtCMOT = 1*ms, powerReduction = 1.0, CMOTFrequency = CMOTFreq, dtNoRepump = 1*20*ms, repumpAttenuatorVoltage = 1, cmotCurrentRampRate = 1)


    ## Right now MOT() is broken, so the field ramp is done here...
#    quadsDoneTime=setQuadrupoleCurrent(time - 10*ms, CMOTcurrent, applyCurrentRamp = True, usePrecharge = False, startingCurrent = MOTcurrent, rampRate = 2.0)
#    setQuadrupoleCurrent(time - 10*ms, CMOTcurrent, applyCurrentRamp = True, usePrecharge = False, startingCurrent = MOTcurrent, rampRate = 2.0)

#    rampQuadrupoleCurrent(startTime = time - 19.9*ms, endTime = time, startCurrent = MOTcurrent, endCurrent = CMOTcurrent, numberOfSteps = 21)


########
 #   setvar('dtQuadStart', 9.99*ms)
    dtQuadStart = 1*9.99*ms 
    rampQuadrupoleCurrent(startTime = time -10*ms - dtQuadStart, endTime = time-0.51*ms, startCurrent = MOTcurrent, endCurrent = CMOTcurrent, numberOfSteps = 21)
#    rampQuadrupoleCurrent(startTime = time -0.51*ms, endTime = time-0.50*ms, startCurrent = MOTcurrent, endCurrent = 0, numberOfSteps = 2)
#    event( ch(dds,1),time-0.50*ms,(260,100,0) )

#    time += 0.4*ms
#    event( ch(dds,1),time,(175,100,0) )
#    time = time - 10*ms + 15*ms        # Strange delay; historical artifact...

#    print "quadsDoneTime " + str(quadsDoneTime/ms) +" time " + str(time/ms) 

#    return max(quadsDoneTime, time)

    return time

 
