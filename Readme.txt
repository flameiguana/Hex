Gerardo Perez
06/06/13
Homework 5

Play Hex against an AI.


Compilation Instructions:
Run provided Makefile or VS 2012 Project.

The Monte Carlo function is found in Hex::computerMoveMC.

Random simulations are fairly good at generating sensible moves. I was honestly surprised when something I wrote beat me in a game using only statistics. Of course there is a cap to how well it will perform, no matter the number of simulation. I think this is because it assumes that the other player will make random moves as well, which isn't the case. The other player will always try to win. I'm sure Monte Carlo can be combined with other heuristics, although I don't know if this is done in reality. 

My program can comfortably run 25000 simulations per move.

I would have liked to do multithreading for this program, but there is no standard support for it pre-2011.
