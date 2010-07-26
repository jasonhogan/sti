from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program

nanoDrive=dev('MCL NanoDrive','ep-timing1.stanford.edu',0)
mirrorPosition=ch(nanoDrive,0)
measMirrorPosition=ch(nanoDrive,1)

# defined variables
setvar('thetaStart', 0)
setvar('phiStart',0)
setvar('zStart',0)
setvar('inc', .1)
setvar('startPosition',0)
setvar('endPosition',30)
setvar('settlingTime', 100*ms)
setvar('dtSetAxes',300*ms)
# Global definitions

t0 = 11*us + 2*ms

for i in range(0,endPosition/inc + 1):
    event(mirrorPosition, t0+i*dtSetAxes, (thetaStart,phiStart,zStart+i*inc))
    meas(measMirrorPosition, t0+i*dtSetAxes+settlingTime)

#set back to zero with a certain buffer
event(mirrorPosition, t0+(endPosition/inc + 1)*dtSetAxes, (thetaStart,phiStart,zStart))
event(mirrorPosition, t0+(endPosition/inc + 2)*dtSetAxes, (thetaStart,phiStart,zStart))

