from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

urad = 0.000001

# Set description used by program

nanoDrive=dev('MCL NanoDrive','ep-timing1.stanford.edu',0)
mirrorPosition=ch(nanoDrive,0)
measMirrorPosition=ch(nanoDrive,1)

# defined variables
setvar('thetaStart', 0)
setvar('phiStart',0.02)
setvar('zStart',0)
setvar('dt', 10*ms)
setvar('startAngle',0)
setvar('stepAngle',-1*urad)
setvar('stepZ',15)
setvar('endAngle',0)
setvar('dtSetAxes',300*ms)
# Global definitions

t0 = 11*us + 2*ms

for i in range(1, 30, 2):
    event(mirrorPosition, t0+(i)*dtSetAxes, (stepAngle,phiStart,stepZ))
    event(mirrorPosition, t0+(i+1)*dtSetAxes, (thetaStart,phiStart,stepZ))

