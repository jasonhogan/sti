from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

slowAnalogOut=dev('Slow Analog Out', 'ep-timing1.stanford.edu', 4)

myTrigger = ch(slowAnalogOut, 7)

setvar('triggerAmp',1)

def myTriggerFunction(Start):

    event(myTrigger, Start,0)
    event(myTrigger, Start + 10*ms,triggerAmp)
    event(myTrigger, Start + 20*ms, 0)

    return Start


t0=10*us

time = t0
time = myTriggerFunction(time)