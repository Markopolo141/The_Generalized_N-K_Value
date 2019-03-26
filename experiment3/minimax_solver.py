import sympy
from sympy import diff, symbols
import numpy as np
from numpy.linalg import pinv
import pdb
import json







def float_gcd(doubles, pseudo_zero, inverting=True):
	if inverting:
		invert = 1.0 if doubles[0]>=0 else -1.0
	else:
		invert = 1.0
	doubles = [abs(d) for d in doubles]
	doubles = sorted(doubles)
	if len(doubles)>1:
		while doubles[-2] >= pseudo_zero:
			doubles[-1] = doubles[-1] - int(doubles[-1] / doubles[-2]) * doubles[-2]
			doubles = sorted(doubles)
	return int(invert/doubles[-1])

def get_matrix_formulation(raw_branches, busses):
	branches = [(int(a[0]),int(a[1]),a[4],a[5]) for a in raw_branches]
	busses = [(int(b[0]),b[2]) for b in busses]
	m = np.zeros([len(busses),len(busses)])
	for i,b in enumerate(busses):
		for o,br in enumerate(branches):
			impedance = max(br[2],0.0000001)
			if b[0]==br[0]:
				m[i,br[0]-1] += impedance
				m[i,br[1]-1] -= impedance
			elif b[0]==br[1]:
				m[i,br[0]-1] -= impedance
				m[i,br[1]-1] += impedance
	m_delete = np.delete(m, 0, 1)
	m_inverse = pinv(m_delete)
	mm = np.append([[0 for i in range(len(busses))]], m_inverse, axis=0)
	mmm = np.identity(len(busses)) - np.matrix(m_delete)*np.matrix(m_inverse)
	mmm = np.asarray(mmm)
	mmm_strings = []
	constraints = []
	for i in range(len(busses)):
		terms = mmm[i,]
		normaliser =  float_gcd(terms.tolist(),0.000001)
		terms = (terms*normaliser).round(6).astype(int)
		if str(terms) not in mmm_strings:
			constraints.append(terms.tolist() +[0,0])
			mmm_strings.append(str(terms))
	for br in branches:
		impedance = max(br[2],0.0000001)
		terms = (mm[br[0]-1,] - mm[br[1]-1,])*impedance
		normaliser = float_gcd([t for t in terms] + [br[3]], 0.000001, False)
		terms = (terms*normaliser).round(6).astype(int)
		br3 = int(round(br[3]*normaliser,6))
		#br3 = br[3]
		constraints.append((-terms).tolist() + [br3,-1])
		constraints.append(terms.tolist() + [br3,-1])
	return constraints



def get_cost_functions(busses,gens,gen_costs):
	expressions = [0 for b in busses]
	variable_defs = [{"upper":b[2],"lower":b[2]} for i,b in enumerate(busses)]
	for i,g in enumerate(gens):
		g0 = int(g[0])
		bus_index = 0
		for o,b in enumerate(busses):
			if b[0] == g0:
				bus_index = o
				break
		else:
			raise Exception("generator on non-existant bus")
		variable_defs[bus_index]['upper'] += g[8]
		expressions[bus_index] = gen_costs[i][4]
		variable_defs[bus_index]['lower'] += g[9]
	return expressions, variable_defs






#with open("0.json","r") as f:
#	ppc = json.load(f)
#
#costs,single_defs = get_cost_functions(ppc['bus'],ppc['gen'],ppc['gencost'])
#invert_power_flow = [1 if a['lower']==0 else -1 for a in single_defs]
#constraints = get_matrix_formulation(ppc['branch'],ppc['bus'])
#for i,v in enumerate(invert_power_flow):
#	costs[i] *= v
#	for c in constraints:
#		c[i] *= v
#	if v==-1:
#		single_defs[i]['lower'],single_defs[i]['upper'] = single_defs[i]['upper'],-single_defs[i]['lower']
#single_constraints = [[0 if ii!=i else 1 for ii in range(len(costs))]+[single_defs[i]['upper'],-1] for i in range(len(costs))]
#print costs
#print single_defs
#print single_constraints
#print constraints
#
#import sys
#sys.exit(0)


import bilevel_solver

def setup(ppc):
	costs,single_defs = get_cost_functions(ppc['bus'],ppc['gen'],ppc['gencost'])
	invert_power_flow = [1 if a['lower']==0 else -1 for a in single_defs]
	constraints = get_matrix_formulation(ppc['branch'],ppc['bus'])
	for i,v in enumerate(invert_power_flow):
		costs[i] *= v
		for c in constraints:
			c[i] *= v
		if v==-1:
			single_defs[i]['lower'],single_defs[i]['upper'] = single_defs[i]['upper'],-single_defs[i]['lower']
	single_constraints = [[0 if ii!=i else 1 for ii in range(len(costs))]+[single_defs[i]['upper'],-1] for i in range(len(costs))]
	bilevel_solver.setup_solver(constraints+single_constraints,costs)

maxmin_minmax_call_count = 0
def calc_maxmin_minmax(i):
	global maxmin_minmax_call_count
	maxmin_minmax_call_count += 1
	if (maxmin_minmax_call_count%5==0):
		bilevel_solver.spruik()
	return bilevel_solver.solve(i)


def spruik_solver():
	return bilevel_solver.spruik()





