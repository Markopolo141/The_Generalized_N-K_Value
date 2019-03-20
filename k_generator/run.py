import bilevel_solver
import time

def print_bin(a):
	ss = ""
	while (a!=0):
		ss = ss+str(a&1)
		a >>= 1
	return ss

def bintogray(bb):
	i = 0
	a = 0
	b = 0
	result = 0
	while (bb != 0):
		a = bb&1
		bb >>= 1
		b = bb&1
		if (a^b):
		#if ((a && !b) || (!a && b)):
			result += 1<<i
		i += 1
	return result;

def count_players(a):
	i = 0
	while (a!=0):
		if (a&1):
			i += 1
		a >>= 1
	return i

#bilevel_solver.setup_solver([[1,2,1,1],[1,2,-1,1],[4,3,5,-1],[4,3,-5,-1],[1,3,4,0],[1,3,-4,0]])
#bilevel_solver.setup_solver([[0.5,0.25,4,-1],[1,3,20,1],[1,1,10,0]],[1,2,3,4])
#print bilevel_solver.setup_solver([[1,2,3,10,-1],[1,0,0,8,-1],[0,1,0,9,-1],[0,0,1,7,-1]],[1,1,-1],0)

#bilevel_solver.setup_solver([[0.5,0.25,4,2,4,-1],[1,3,20,-1,2,1],[1,1,-10,4,3,0]])
#print bilevel_solver.solve(0);
#print bilevel_solver.solve(1);
#print bilevel_solver.solve(2);
#print bilevel_solver.solve(3);


#bilevel_solver.setup_solver([[1,0,0,1,-1],[0,1,0,1,-1],[0,0,1,1,-1]],[1,1,1],0)
#for i in range(8):
#	print i, bilevel_solver.solve(i)

import sys

for i in range(10):
	players = 15
	matrix = [[1 if i==j else 0 for j in range(players)] + [1,-1] for i in range(players)]
	rewards = [1 for i in range(players)]
	bilevel_solver.setup_solver(matrix,rewards,0)

	#print bilevel_solver.solve(15)
	#print bilevel_solver.solve(16)

	t = time.time()
	for i in range(1<<players):
	#for i in list(range(13)) + list(range((1<<players)-13,1<<players)):
		bilevel_solver.solve(i)
		#print "{}\t{}\t{}".format(print_bin(i), bilevel_solver.solve(i),-players+2*count_players(i))
		#sys.stdout.write("_" if bilevel_solver.solve(i)-(-players+2*count_players(i))==0 else "X")
		#sys.stdout.flush()
	print "{} seconds".format(time.time()-t)

#for i in range(20):
#	print "{}\t{}\t{}".format(i,print_bin(i),print_bin(bintogray(i)))

print ""
