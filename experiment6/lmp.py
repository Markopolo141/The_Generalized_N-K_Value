import pdb
import json
import click
from sympy import symbols
from pyscipopt import Model
import pyscipopt


def create_model(debug=False):
	if debug:
		print "from pyscipopt import Model"
		print "import pyscipopt"
		print "model=Model()"
		print "model.setPresolve(pyscipopt.SCIP_PARAMSETTING.OFF)"
		print "model.disablePropagation()"
	model = Model()
	model.setPresolve(pyscipopt.SCIP_PARAMSETTING.OFF)
	model.disablePropagation()
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
	c = model.addVar("c", ub=1e20, lb=-1e20, vtype = 'C')
	model.setObjective(c, "maximize")
	model.addCons(eval(str(expression), variables) >= c)
	if debug:
		print "c = model.addVar(\"c\", ub=1e20, lb=-1e20, vtype = 'C')"
		print "model.setObjective(c, \"maximize\")"
		print "model.addCons({} >= c)".format(str(expression))
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
	#defs_names = [symbols(d['name']) for d in defs]
	#power_constraints = [[p.coeff(n) for n in defs_names] for p in power_constraints]
	#angle_constraints = [[p.coeff(n) for n in defs_names] for p in angle_constraints]
	#pdb.set_trace()
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
	sol = model_optimise(model,debug=debug,maximizing=True)
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
	dollar_values = [-dual_variables[i]*power_variables[i] for i in range(len(dual_variables))]
	consumption_utilities = [costs[i].subs({symbols(power_names[i]):power_variables[i],}) for i in range(len(power_names))]
	#print "dollar_values = {}".format(dollar_values)
	#print "consumption_utilities = {}".format(consumption_utilities)
	return [dollar_values[i] + consumption_utilities[i] for i in range(len(dollar_values))]




@click.command()
@click.argument('input_file', type=click.File('rb'))
@click.argument('output_file', type=click.File('a'))
def run(input_file,output_file):
	print "---START---"
	global N
	ppc = json.load(input_file)
	N = len(ppc['bus'])
	input_file.close()
	r = [float(r) for r in calc_LMP(ppc,False)]
	#print r
	mag_r = sum([rr**2 for rr in r])
	if mag_r>0:
		output_file.write("{} {}\n".format(N,r))
	output_file.close()

if __name__ == '__main__':
    run()









