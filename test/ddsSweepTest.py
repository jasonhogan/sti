from stipy import *
 
ns = 1.0
us = 1000.0
ms = 1000000.0
s = 1000000000.0

# Set description used by program
setvar('desc','''Test experiment.''') 


dds = dev('DDS', 'ep-timing1.stanford.edu', 0)

setvar('ddsFreq',1)


# Define different blocks of the experiment
def MOT(Start):

    
#    event(ch(dds, 2), 1*ms, (51, 100, 0) )


#    event(ch(dds, 2), 1*ms + 1*s, ((51,31, 2*s), 100, 0) )

#    event(ch(dds, 2), 4*s, (31, 100, 0) )

#    event(ch(dds, 2), 4.1*s, ((30,31,0.2*s), 100, 0) )

#    event(ch(dds, 2), 4.5*s, (31, 100, 0) )

#    event(ch(dds, 2), 5*s, ((31,41,2*s), 100, 0) )

    event(ch(dds, 2), 0.1*s, (11, 100, 0) )

    event(ch(dds, 2), 1*s, (((11, 41, 2*s), (41,51, 3*s), (51, 61, 1*s), (61, 71, 5*s), (71, 85, 3*s)), 100, 0) )

#    event(ch(dds, 2), 28*s, (((1,1,1*s) ,(1,2,3)), 100, 0) )

#    event(ch(dds, 2), 11*s, (51, 0, 0) )



    tDDS=Start+55*us

    return Start


# Global definitions

 
t0 = 10*us

time = t0
time = MOT(time)
