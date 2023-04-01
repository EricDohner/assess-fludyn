# assess_fludyn
Fluid dynamics in one dimension, using FCT.

I adapted the operator splitting method from Boris and Book 1971 to use the FCT to solve both the momentum and density continuity equations. In principle one could use FCT to solve the latter and then get the momentum density equation in a form that only depends on v, and solve that equation separately, but it seemed more appropriate (and probably much faster and more accurate) to simply use FCT twice.

Before you start: Make sure there's a subdirectory in assess_fludyn called "outdat."

"Make" compiles everything.

"Make run" runs the program and uses gnuplot to create the animation.
