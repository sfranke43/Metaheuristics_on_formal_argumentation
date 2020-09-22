# Metaheuristics_on_formal_argumentation
An implementation of Simulated Annealing and Tabu Search on formal argumentation
With increasing framework size the quality of the solution drops significantelly

The chosen neighborhood and maximum iterations can be set by the user
The programs are currently nly able to process .apx files.

The quality of solutions is measured by how "close" they are to being an extension

The nieghborhood of solutions consists of:
MINIMUM_OF_RANDOM_NEIGHBORS: the minimum number of random arguments to be added to the solution
MAXIMUM_OF_RANDOM_NEIGHBORS: the maximum number of randeom arguments to be added to the solution (after a neighbor of quality >0 0.5 is found there are 10 more random arguments to be added, but there have to be at least MINIMUM_OF_RANDOM_NEIGHBORS and at max  MAXIMUM_OF_RANDOM_NEIGHBORS random arguments)
MAXIMUM_OF_DEFENDED_NEIGHBORS: the number of arguments in the neighborhood that are already defended
MAXIMUM_OF_DELETE_NEIGHBORS: the number of arguments in the neighborhood that are to be deleted from the current solution
MAXIMUM_OF_NEIGBORS_THAT_ATTACK_UNDEFENDED_ATTACKS: checkss which attacks are undefended and adds arguments that attack these attackers

To start the program:
      Simulated Annealing (SimAn): ./Siman -p PROBLEM-SEMANTIC -f FILELOCATION  -fo APX -a IF_NEEDED -n MINIMUM_OF_RANDOM_NEIGHBORS MAXIMUM_OF_RANDOM_NEIGHBORS MAXIMUM_OF_DEFENDED_NEIGHBORS MAXIMUM_OF_DELETE_NEIGHBORS MAXIMUM_OF_NEIGBORS_THAT_ATTACK_UNDEFENDED_ATTACKS MAX_ITERATIONS DECREASE_RATE_PER_ITERATION 0 
      
      Tabu Search (TS):
     ./TabuSearch -p PROBLEM-SEMANTIC -f FILELOCATION  -fo APX -a IF_NEEDED -n MINIMUM_OF_RANDOM_NEIGHBORS MAXIMUM_OF_RANDOM_NEIGHBORS MAXIMUM_OF_DEFENDED_NEIGHBORS MAXIMUM_OF_DELETE_NEIGHBORS MAXIMUM_OF_NEIGBORS_THAT_ATTACK_UNDEFENDED_ATTACKS MAX_ITERATIONS TABU_SIZE
