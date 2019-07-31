import sympy
from sympy import diff, symbols
from sympy.simplify.simplify import nsimplify
import pdb
import numpy as np
from numpy.linalg import pinv
from pyscipopt import Model
import pyscipopt
import sys
from tqdm import tqdm

def create_model(debug=False):
	if debug:
		print "from pyscipopt import Model"
		print "import pyscipopt"
		print "model=Model()"
		print "model.setPresolve(pyscipopt.SCIP_PARAMSETTING.OFF)"
		#print "model.disablePropagation()"
	model = Model()
	#model.setPresolve(pyscipopt.SCIP_PARAMSETTING.OFF)
	#model.disablePropagation()
	return model

def add_constraint(model, expression, model_variables, debug=False):
	c = model.addCons(eval(str(expression),model_variables))
	if debug:
		print "model.addCons({})".format(str(expression))
	return c

def load_variable(model, name, lb=None,ub=None,  debug=False):
	v = None
	if ub is not None:
		if lb is not None:
			v = model.addVar(name=name, lb=lb, ub=ub, vtype="C")
			if debug:
				print "{0} = model.addVar(\"{0}\", lb={1}, ub={2}, vtype=\"C\")".format(name, lb, ub)
		else:
			v = model.addVar(name=name, ub=ub, vtype="C")
			if debug:
				print "{0} = model.addVar(\"{0}\", ub={1}, vtype=\"C\")".format(name, ub)
	else:
		if lb is not None:
			v = model.addVar(name=name, lb=lb, vtype="C")
			if debug:
				print "{0} = model.addVar(\"{0}\", lb={1}, vtype=\"C\")".format(name, lb)
		else:
			v = model.addVar(name=name, vtype="C")
			if debug:
				print "{0} = model.addVar(\"{0}\", vtype=\"C\")".format(name)
	return v

def load_binary_variable(model, name, debug=False):
	v = model.addVar(name, vtype = 'B')
	if debug:
		print "{0} = model.addVar(\"{0}\", vtype = 'B')".format(name)
	return v

def set_objective(model, expression, variables, debug=False):
	c = model.addVar("c", lb=-1e20, vtype = 'C')
	model.setObjective(c, "minimize")
	model.addCons(eval(str(expression), variables) <= c)
	if debug:
		print "c = model.addVar(\"c\", lb=-1e20, vtype = 'C')"
		print "model.setObjective(c, \"minimize\")"
		print "model.addCons({} <= c)".format(str(expression))
	return c
	#if debug:
	#	print 'model.setObjective({}, "minimize")'.format(str(expression))
	#return model.setObjective(eval(str(expression), variables), "minimize")

def model_optimise(model,maximizing=False,debug=False):
	if debug:
		print "model.optimize()"
		print "sols = model.getSols()"
		print "extreme_sol = sols[0]"
		print "for sol in sols:"
		if maximizing:
			print "\tif model.getSolObjVal(sol) > model.getSolObjVal(extreme_sol):"
		else:
			print "\tif model.getSolObjVal(sol) < model.getSolObjVal(extreme_sol):"
		print "\t\textreme_sol = sol"
		print "return extreme_sol"
	model.optimize()
	status = model.getStatus()
	if status == "optimal" or status == "bestsollimit":
		sols = model.getSols()
		extreme_sol = sols[0]
		for sol in sols:
			if (model.getSolObjVal(sol) > model.getSolObjVal(extreme_sol)) and maximizing:
				extreme_sol = sol
			elif (model.getSolObjVal(sol) < model.getSolObjVal(extreme_sol)) and (not maximizing):
				extreme_sol = sol
		return extreme_sol
	else:
		raise Exception('bam')


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

def integer_scaling(expr):
	#nums = [a for a in expr.atoms() if a.is_number] + [1]
	#return nsimplify(float_gcd(nums, 0.000001, False) * expr, tolerance=1e-4)
	return expr

def get_matrix_inverse(raw_branches, busses):
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
	bus_symbols = [symbols("p{}".format(b[0]-1)) for b in busses]
	bus_symbols_array = np.array(bus_symbols)
	mm = np.append([[0 for i in range(len(busses))]], pinv(np.delete(m, 0, 1)), axis=0)
	mmm = np.identity(len(busses)) - np.matrix(np.delete(m, 0, 1))*np.matrix(pinv(np.delete(m, 0, 1)))
	mmm = np.asarray(-mmm)
	mmm_strings = []
	tight_constraints = []
	for i in range(len(busses)):
		terms = mmm[i,]
		normaliser =  float_gcd([t for t in terms],0.0001)
		terms = (terms*normaliser).round(3).astype(int)
		Z = np.dot(terms, bus_symbols_array)
		if str(Z) not in mmm_strings:
			tight_constraints.append(Z)
			mmm_strings.append(str(Z))
	constraints = []
	for br in branches:
		impedance = max(br[2],0.0000001)
		terms = (mm[br[0]-1,] - mm[br[1]-1,])*impedance
		normaliser = float_gcd([t for t in terms] + [br[3]], 0.0001, False)
		terms = (terms*normaliser).round(3).astype(int)
		br3 = int(round(br[3]*normaliser,3))
		if len(tight_constraints)==1:
			constraints.append(-np.dot(terms, bus_symbols_array) - br3 + Z)
			constraints.append(+np.dot(terms, bus_symbols_array) - br3 - Z)
		else:
			constraints.append(-np.dot(terms, bus_symbols_array) - br3)
			constraints.append(+np.dot(terms, bus_symbols_array) - br3)
	return constraints, tight_constraints


def get_cost_functions(busses,gens,gen_costs, consumers_optional=False):
	expressions = [symbols("x")*0 for b in busses]
	if consumers_optional:
		variable_defs = [{"name":"p{}".format(i),"upper":b[2],"lower":0} for i,b in enumerate(busses)]
	else:
		variable_defs = [{"name":"p{}".format(i),"upper":b[2],"lower":b[2]} for i,b in enumerate(busses)]
	def get_poly(symbol,gen_cost):
		gen_cost = gen_cost[::-1]
		expr = symbol*0
		for i,g in enumerate(gen_cost):
			expr += g*symbol**i
		return expr
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
		expressions[bus_index] = get_poly((-variable_defs[bus_index]['lower'] + symbols("p{}".format(bus_index))),list(gen_costs[i][4:]))
		variable_defs[bus_index]['lower'] += g[9]
	return expressions, variable_defs

def get_LMP_constraints(raw_branches,busses,gens,gen_costs):
	branches = [(int(a[0]),int(a[1]),a[4],a[5]) for a in raw_branches]
	busses = [(int(b[0]),b[2]) for b in busses]
	constraints = []
	for b in busses:
		c = symbols("p{}".format(b[0]))# - b[1]
		for br in branches:
			if br[0]==b[0]:
				c += symbols("p{}_{}".format(br[0],br[1]))
			elif br[1]==b[0]:
				c -= symbols("p{}_{}".format(br[0],br[1]))
		constraints.append(c)
	angle_constraints = []
	for br in branches:
		c = symbols("p{}_{}".format(br[0],br[1])) 
		d = max(br[2], 0.0000001)
		e = 0 if br[0] == busses[0][0] else symbols("t{}".format(br[0]))
		f = 0 if br[1] == busses[0][0] else symbols("t{}".format(br[1]))
		angle_constraints.append(c-d*(e-f))
	range_constraints = []
	for br in branches:
		range_constraints.append({"name":"p{}_{}".format(br[0],br[1]), "lower":-br[3],"upper":br[3]})
	for b in busses[1:]:
		range_constraints.append({"name":"t{}".format(b[0]), "lower":-1e20,"upper":1e20})
	
	expressions = [symbols("x")*0 for b in busses]
	variable_defs = [{"name":"p{}".format(b[0]),"upper":b[1],"lower":b[1]} for i,b in enumerate(busses)]
	def get_poly(symbol,gen_cost):
		gen_cost = gen_cost[::-1]
		expr = symbol*0
		for i,g in enumerate(gen_cost):
			expr += g*symbol**i
		return expr
	for i,g in enumerate(gens):
		g0 = int(g[0])
		bus_index = 0
		for o,b in enumerate(busses):
			if b[0] == g0:
				bus_index = o
				break
		else:
			raise Exception("generator on non-existant bus")
		variable_defs[bus_index]['upper'] -= g[9]
		expressions[bus_index] = get_poly((+variable_defs[bus_index]['lower'] - symbols("p{}".format(g0))),list(gen_costs[i][4:]))
		variable_defs[bus_index]['lower'] -= g[8]
	range_constraints += variable_defs
	return constraints, angle_constraints, range_constraints, expressions


def calc_LMP(ppc, debug=False):
	power_constraints, angle_constraints, defs, costs = get_LMP_constraints(ppc['branch'],ppc['bus'],ppc['gen'],ppc['gencost'])
	expression1 = sum(costs)
	model = create_model(debug=debug)
	model.setPresolve(pyscipopt.SCIP_PARAMSETTING.OFF)
	model.setHeuristics(pyscipopt.SCIP_PARAMSETTING.OFF)
	model.disablePropagation()
	constraints = []
	if not debug:
		model.hideOutput()
	model_variables = {}
	for s in defs:
		model_variables[s['name']] = load_variable(model, s['name'], lb=s['lower'], ub=s['upper'], debug=debug)
	set_objective(model, expression1, model_variables, debug=debug)
	for g in power_constraints + angle_constraints:
		constraints.append(add_constraint(model, "{}==0".format(str(g)), model_variables, debug=debug))
	sol = model_optimise(model,debug=debug,maximizing=False)
	del model_variables['__builtins__']
	model_variables_sympy = {symbols(name):model.getSolVal(sol,value) for name,value in model_variables.iteritems()}
	substituted_costs = [c.subs(model_variables_sympy) for c in costs]
	if debug:
		k = model_variables.keys()
		k = sorted(k)
		for key in k:
			print key, round(model.getSolVal(sol,model_variables[key]),5)
		print model.getSolObjVal(sol), substituted_costs
		for i,c in enumerate(power_constraints + angle_constraints):
			print c,"\n\t", model.getDualsolLinear(constraints[i])
	power_names = [p['name'] for p in defs if p['name'][0]=='p' and '_' not in p['name']]
	dual_variables = [model.getDualsolLinear(constraints[i]) for i in range(len(power_constraints))]
	power_variables = [model.getSolVal(sol,model_variables[p]) for p in power_names]
	dollar_values = [dual_variables[i]*power_variables[i] for i in range(len(dual_variables))]
	return dual_variables, dollar_values

def calc_maxmin_minmax(ppc, inds, debug=False, bignum=99999):
	constraints,tight_constraints = get_matrix_inverse(ppc['branch'],ppc['bus'])
	costs,single_defs = get_cost_functions(ppc['bus'],ppc['gen'],ppc['gencost'])
	expression1 = sum([i*costs[ii] for ii,i in enumerate(inds)])
	power_expressions = [d['name'] for d in single_defs if d['name'][0]=='p']
	def inner_run(positive, debug):
		inequality_constraints = constraints[:]
		for s in single_defs:
			if inds[int(s['name'][1:])]==positive*-1:
				inequality_constraints.append(s['lower']-symbols(s['name']))
				inequality_constraints.append(symbols(s['name'])-s['upper'])
		
		expression2 = (expression1 + 
			-1*positive*np.dot(inequality_constraints, [symbols("l{}".format(i)) for i,aa in enumerate(inequality_constraints)]) + 
			np.dot(tight_constraints, [symbols("lt{}".format(i)) for i,aa in enumerate(tight_constraints)]))
		expression1_symbols_neg = [symbols("p{}".format(i)) for i,o in enumerate(inds) if o==positive*-1]
		grads = [diff(expression2,sym) for sym in expression1_symbols_neg]
		
		model = create_model(debug=debug)
		if not debug:
			model.hideOutput()
		model_variables = {}
		for s in single_defs:
			model_variables[s['name']] = load_variable(model, s['name'], lb=s['lower'], ub=s['upper'], debug=debug)
		for i in range(len(tight_constraints)):
			name1 = "lt{}".format(i)
			model_variables[name1] = load_variable(model, name1, lb=-1e20, debug=debug)
		for i in range(len(inequality_constraints)):
			name1 = "l{}".format(i)
			name2 = "Zl{}".format(i)
			model_variables[name1] = load_variable(model, name1, debug=debug)
			model_variables[name2] = load_binary_variable(model, name2, debug=debug)
		
		set_objective(model, positive*expression1, model_variables, debug=debug)
		for g in grads:
			add_constraint(model, "{}==0".format(str(integer_scaling(g))), model_variables, debug=debug)
		for g in tight_constraints:
			add_constraint(model, "{}==0".format(str(integer_scaling(g))), model_variables, debug=debug)
		for i,g in enumerate(inequality_constraints):
			name1 = "l{}".format(i)
			name2 = "Zl{}".format(i)
			if g in constraints and len(g.free_symbols)>0:
				add_constraint(model, "{}<=0".format(str(integer_scaling(g))), model_variables, debug=debug)
			add_constraint(model, "{}<=0".format(str(integer_scaling((1-symbols(name2))*-bignum - g))), model_variables, debug=debug)
			add_constraint(model, "{}-{}<=0".format(name1,str(integer_scaling(symbols(name2)*bignum))), model_variables, debug=debug)
		#sol = model_optimise(model,debug=debug,maximizing=True if positive==1 else False)
		sol = model_optimise(model,debug=debug,maximizing=False)
		
		del model_variables['__builtins__']
		model_variables_sympy = {symbols(name):model.getSolVal(sol,value) for name,value in model_variables.iteritems()}
		substituted_costs = [float(c.subs(model_variables_sympy)) for c in costs]
		if debug:
			k = model_variables.keys()
			k = sorted(k)
			for key in k:
				print key, round(model.getSolVal(sol,model_variables[key]),5)
			print model.getSolObjVal(sol)*positive, substituted_costs
		power_dictionary = {p:round(model.getSolVal(sol,model_variables[p]),5) for p in power_expressions}
		return model.getSolObjVal(sol)*positive, substituted_costs, power_dictionary
	if 1 not in inds:
		A, Ac, AAc = inner_run(-1, debug)
		B, Bc, BBc = A, Ac, AAc
	else:
		A, Ac, AAc = inner_run(1, debug)
		B, Bc, BBc = inner_run(-1, debug)
	#print "--{}\t{}\t{}\t{}\t{}\t{}\t{}".format(inds,-A,-B,AAc,BBc,Ac,Bc)
	#print "--{}\t{}\t{}\n----\t{}\n----\t{}".format(inds,-A,-B,AAc,BBc)
	return -0.5*A + -0.5*B, [-0.5*Ac[i]-0.5*Bc[i] for i in range(len(Ac))], AAc, BBc



def calculate_value(ppc, debug=False, tqdm_show=True, bignum=99999):
	nss = len(ppc['bus'])
	datas = []
	max_payoffs = None
	max_powers = None
	i_range = range(0, 2**(nss-1))
	if tqdm_show:
		i_range = tqdm(i_range)
	for i in i_range:
		ind = i
		inds = []
		counter = 0
		while (ind!=0) or (counter<nss):
			inds.append(2*(ind%2)-1)
			ind = ind >> 1
			counter += 1
		value, payoffs, powers1, powers2 = calc_maxmin_minmax(ppc, inds, debug=debug, bignum=bignum)
		datas.append((inds[:],value))
		#print inds,value,powers1,powers2
		datas.append(([-zzz for zzz in inds],-value))
		if i==0:
			max_payoffs = payoffs[:]
			max_powers = (powers1,powers2)
	#for d in datas:
	#	print d
	values = [0 for i in range(nss)]
	coalition_values = [[0,0] for i in range(nss)]
	for i in range(nss):
		for d in datas:
			if d[0][i]==1:
				index = (sum(d[0])+nss)/2 - 1
				coalition_values[index][0] += d[1]
				coalition_values[index][1] += 1
		values[i] = sum([c[0]/c[1] if c[1]>0 else 0 for c in coalition_values])/nss
		coalition_values = [[0,0] for i in range(nss)]
	return values, max_payoffs, max_powers

def calculate_value_predef_inds(ppc, inds, debug=False, tqdm_show=False, bignum=99999):
	nss = len(ppc['bus'])
	datas = []
	max_payoffs = None
	max_powers = None
	i_range = range(len(inds))
	if tqdm_show:
		i_range = tqdm(i_range)
	for i in i_range:
		ind = inds[i]
		value, payoffs, powers1, powers2 = calc_maxmin_minmax(ppc, ind, debug=debug, bignum=bignum)
		datas.append((ind,value))
		datas.append(([-zzz for zzz in ind],-value))
		if i==0:
			max_payoffs = payoffs[:]
			max_powers = (powers1,powers2)
	for d in datas:
		print d
	pdb.set_trace()
	values = [0 for i in range(nss)]
	coalition_values = [[0,0] for i in range(nss)]
	for i in range(nss):
		for d in datas:
			if d[0][i]==1:
				index = (sum(d[0])+nss)/2 - 1
				coalition_values[index][0] += d[1]
				coalition_values[index][1] += 1
		values[i] = sum([c[0]/c[1] if c[1]>0 else 0 for c in coalition_values])/nss
		coalition_values = [[0,0] for i in range(nss)]
	return values, max_payoffs, max_powers

if __name__ == "__main__":
	#a,b = calc_LMP(case6www(), debug=True)
	#print a,b
	#print sum([a[i]*b[i] for i in range(len(a))])
	from pypower.case6www import case6www
	print calculate_value(case6www(), debug=True, tqdm_show=True, bignum=9999)




