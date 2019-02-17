import sympy
from sympy import diff, symbols
import numpy as np
from numpy.linalg import pinv
from pyscipopt import Model
import pyscipopt
import pdb

def create_model(debug=False):
	if debug:
		print "from pyscipopt import Model"
		print "import pyscipopt"
		print "model=Model()"
		print "model.setPresolve(pyscipopt.SCIP_PARAMSETTING.OFF)"
		#print "model.disablePropagation()"
	model = Model()
	model.setPresolve(pyscipopt.SCIP_PARAMSETTING.OFF)
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
		raise Exception('bam: {}'.format(status))


def get_matrix_inverse(raw_branches, busses):
	branches = [(int(a[0]),int(a[1]),a[4],a[5]) for a in raw_branches]
	busses = [(int(b[0]),b[2]) for b in busses]
	tight_constraints = []
	constraints = []
	for b in busses:
		constraint = symbols("p{}".format(b[0]))
		for br in branches:
			if b[0]==br[0]:
				constraint += symbols("p{}_{}".format(br[0],br[1]))
			elif b[0]==br[1]:
				constraint -= symbols("p{}_{}".format(br[0],br[1]))
		tight_constraints.append(constraint)
	for br in branches:
		constraints.append(symbols("p{}_{}".format(br[0],br[1]))-br[3])
		constraints.append(-br[3]-symbols("p{}_{}".format(br[0],br[1])))
		tight_constraints.append(symbols("p{}_{}".format(br[0],br[1]))+br[2]*(symbols("t{}".format(br[0]))-symbols("t{}".format(br[1]))))
	defs = [{"name":"t{}".format(b[0]),"upper":1e20,"lower":-1e20} for b in busses]
	defs += [{"name":"p{}_{}".format(br[0],br[1]),"upper":br[3],"lower":-br[3]} for br in branches]
	return constraints, tight_constraints, defs



def get_matrix_inverse2(raw_branches, busses):
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
		variable_defs = [{"name":"p{}".format(b[0]),"upper":b[2],"lower":0} for i,b in enumerate(busses)]
	else:
		variable_defs = [{"name":"p{}".format(b[0]),"upper":b[2],"lower":b[2]} for i,b in enumerate(busses)]
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
		expressions[bus_index] = get_poly((-variable_defs[bus_index]['lower'] + symbols("p{}".format(busses[bus_index][0]))),list(gen_costs[i][4:]))
		variable_defs[bus_index]['lower'] += g[9]
	return expressions, sorted(variable_defs,key=lambda x:x['name'])

def calc_maxmin_minmax(ppc, inds, debug=False, bignum=99999):
	constraints,tight_constraints,defs = get_matrix_inverse(ppc['branch'],ppc['bus'])
	costs,single_defs = get_cost_functions(ppc['bus'],ppc['gen'],ppc['gencost'])
	expression1 = sum([i*costs[ii] for ii,i in enumerate(inds)])
	power_expressions = [d['name'] for d in single_defs if d['name'][0]=='p']
	def inner_run(positive, debug):
		inequality_constraints = constraints[:]
		expression1_symbols_neg = []
		for i,s in enumerate(single_defs):
			if inds[i]==positive*-1:
				expression1_symbols_neg.append(symbols(s['name']))
				inequality_constraints.append(s['lower']-symbols(s['name']))
				inequality_constraints.append(symbols(s['name'])-s['upper'])
		
		expression2 = (expression1 + 
			-1*positive*np.dot(inequality_constraints, [symbols("l{}".format(i)) for i,aa in enumerate(inequality_constraints)]) + 
			np.dot(tight_constraints, [symbols("lt{}".format(i)) for i,aa in enumerate(tight_constraints)]))
		grads = [diff(expression2,sym) for sym in expression1_symbols_neg]
		
		model = create_model(debug=debug)
		if not debug:
			model.hideOutput()
		model_variables = {}
		for s in single_defs+defs:
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
			add_constraint(model, "{}==0".format(str(g)), model_variables, debug=debug)
		for g in tight_constraints:
			add_constraint(model, "{}==0".format(str(g)), model_variables, debug=debug)
		for i,g in enumerate(inequality_constraints):
			name1 = "l{}".format(i)
			name2 = "Zl{}".format(i)
			if g in constraints and len(g.free_symbols)>0:
				add_constraint(model, "{}<=0".format(str(g)), model_variables, debug=debug)
			add_constraint(model, "{}<=0".format(str((1-symbols(name2))*-bignum - g)), model_variables, debug=debug)
			add_constraint(model, "{}-{}<=0".format(name1,str(symbols(name2)*bignum)), model_variables, debug=debug)
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
	
	def inner_positive(debug):
		model = create_model(debug=debug)
		if not debug:
			model.hideOutput()
		model_variables = {}
		for s in single_defs+defs:
			model_variables[s['name']] = load_variable(model, s['name'], lb=s['lower'], ub=s['upper'], debug=debug)
		
		set_objective(model, expression1, model_variables, debug=debug)
		for g in tight_constraints:
			add_constraint(model, "{}==0".format(str(g)), model_variables, debug=debug)
		for i,g in enumerate(constraints):
			if len(g.free_symbols)>0:
				add_constraint(model, "{}<=0".format(str(g)), model_variables, debug=debug)
		sol = model_optimise(model,debug=debug,maximizing=False)
		
		del model_variables['__builtins__']
		model_variables_sympy = {symbols(name):model.getSolVal(sol,value) for name,value in model_variables.iteritems()}
		substituted_costs = [float(c.subs(model_variables_sympy)) for c in costs]
		if debug:
			k = model_variables.keys()
			k = sorted(k)
			for key in k:
				print key, round(model.getSolVal(sol,model_variables[key]),5)
			print model.getSolObjVal(sol), substituted_costs
		power_dictionary = {p:round(model.getSolVal(sol,model_variables[p]),5) for p in power_expressions}
		return model.getSolObjVal(sol), substituted_costs, power_dictionary
	if 1 not in inds:
		A, Ac, AAc = inner_positive(debug)
		return A, Ac, {},AAc
	elif -1 not in inds:
		expression1 *= -1
		A, Ac, AAc = inner_positive(debug)
		return -A,[-a for a in Ac],AAc,{}
	else:
		A, Ac, AAc = inner_run(1, debug)
		B, Bc, BBc = inner_run(-1, debug)
		return -0.5*A + -0.5*B, [-0.5*Ac[i]-0.5*Bc[i] for i in range(len(Ac))], AAc, BBc


