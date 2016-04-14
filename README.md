# sigmadelta_compiler

This is a small tool to generate optimized functions for 2nd Order SigmaDelta-Modulators.
It supports output as generic C code as well as ARM optimized assembly.
Calculation of 1 Bit takes 6 cpu cylces on arm (without prologue and epilogue).
On dual and partial dual issue CPUs (like A7 and A53) - only 5 cycles are expected (although not tested yet)

for compilation just run "make"

if you have octave (+ octave-signal package) installed you can run "make evaluate" to evalute the spectral power density

