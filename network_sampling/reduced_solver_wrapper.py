from reduced_solver import calc_maxmin_minmax
from simple_index_generator import generate_indices,simple_generate_indices
import pdb
import tqdm

scooped = False#True

if scooped:
	from scoop import futures, shared
else:
	class shared_class:
		dict = None
		def __init__(self):
			self.dict = {}
		def getConst(self,key):
			return self.dict[key]
		def setConst(self, **kwargs):
			self.dict.update(kwargs)
	shared = shared_class()

ppc = None

mem_inds = {'_counter_inds':[]}
def wrapper(inds):
	ninds = [-1*i for i in inds]
	str_inds = str(inds)
	str_ninds = str(ninds)
	if (str_inds not in mem_inds['_counter_inds']) and (str_ninds not in mem_inds['_counter_inds']):
		mem_inds['_counter_inds'].append(str_inds)
	if (str_inds not in mem_inds.keys()) and (str_ninds not in mem_inds.keys()):
		ppc = shared.getConst('ppc')
		debug = shared.getConst('debug')
		bignum = shared.getConst('bignum')
		ret = calc_maxmin_minmax(ppc, inds, debug=debug, bignum=bignum)
		mem_inds[str_inds] = ret
		return ret
	else:
		if (str_inds in mem_inds.keys()):
			return mem_inds[str_inds]
		if (str_ninds in mem_inds.keys()):
			z = list(mem_inds[str_ninds])
			z[0] *= -1
			z[2],z[3] = z[3],z[2]
			return z

max_payoffs = None
max_powers = None
def make_calculator(ppc):
	nss = len(ppc['bus'])
	datas = []
	def calculate_inds(inds, debug=False):
		global max_payoffs
		global max_powers
		ret = []
		if scooped:
			returns = list(futures.map(wrapper, inds))
		else:
			returns = []
			if not debug:
				iterator = inds
			else:
				iterator = tqdm.tqdm(inds)
			for i in iterator:
				returns.append(wrapper(i))
		for i,ind in enumerate(inds):
			datas.append((ind[:],returns[i][0]))
			datas.append(([-zzz for zzz in ind],-returns[i][0]))
			ret.append(returns[i][0])
			if 1 not in ind:
				max_payoffs = returns[i][1][:]
				max_powers = (returns[i][2],returns[i][3])
		return ret
	def calc():
		global max_payoffs
		global max_powers
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
	return calculate_inds, calc

