# CoincidenceProcessor
Preserving the RatCap software for posterity

This is the coincidence processor that we used for the PET imaging data
of the RatCap PET detector and its derivatives (the Breast Scanner, and the
Plant Scanner). 

The raw data consist of the responses of the detector elememts of the scanner,
plus a time stamp. The responses of the crystals are coarsely, but not strictly
time-sorted. This implements a pool of such "singles" of adjustable depth that 
keeps the contents time-sorted at all times. Singles that havee been processed 
will be dropped from the pool. When the pool reaches a low level of entries, more
are inserted until the ppool reaches the high water mark again (or the input 
stream is exhausted). 

The pprocessing consists of traversing the pool and finding coincidences, entries
with time stamp differences below a certain threshold. Due to the sorted list, one
a time stamp larger than the threshold is encountered, the processing is done -
all other entries can only have still-larger values. 

The pool also supports background estimation by calculating delayed (random coincidences) 
by adding a large offset to one of the time stamps. In this way, no actual coincidences can be'
found, and whatever passes the threshold is a random one. 


