#!/usr/bin/python
import timing

# Overwrite one variable
timing._overloads['MOT_Current'] = 2

# Run experiment script
timing.include("experiment.py")

# Output results
print "events          = ", timing._events
print "event_pos       = ", timing._event_pos
print "measurements    = ", timing._measurements
print "measurement_pos = ", timing._measurement_pos
print "files_used      = ", timing._files_used
print "var_pos         = ", timing._var_pos
