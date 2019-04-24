import pdb
from math import factorial,sqrt,floor,log
from random import shuffle
import operator as op
from functools import reduce
from copy import deepcopy as copy

def ncr(n, r):
    r = min(r, n-r)
    numer = reduce(op.mul, range(n, n-r, -1), 1)
    denom = reduce(op.mul, range(1, r+1), 1)
    return numer / denom

def rand_gen_n(nss, n, r):
	if r>ncr(nss,n):
		print "BANG"
		r = ncr(nss,n)
	number_list = list(range(nss))
	inds = []
	inds_strings = []
	while len(inds) < r:
		shuffle(number_list)
		n_list = sorted(number_list[0:n])
		sn_list = str(n_list)
		n_list2 = sorted([a for a in number_list if a not in n_list])
		sn_list2 = str(n_list2)
		if (sn_list not in inds_strings) and (sn_list2 not in inds_strings):
			inds_strings.append(sn_list)
			inds.append([1 if ii in n_list else -1 for ii in range(nss)])
	return inds

def get_inds(nss, array):
	inds = []
	for i,n in enumerate(array):
		inds += rand_gen_n(nss,i,n)
	return inds

def maurer_bound(N,n,y,d):
	if n>N:
		return float("inf")
	N = N*1.0/d
	n = n*1.0/d
	return d*sqrt(min((1-(n-1.0)/N)/n,(1-n*1.0/N)*(1+1.0/n)/n)*log(1.0/y))
def get_sample_numbers(nss,y,total_samples):
	if total_samples*2>2**nss:
		ret = [ncr(nss,i) for i in range(nss/2+1)]
		if nss%2==0:
			ret[-1]/=2
		return ret
	sample_max_numbers = [ncr(nss-1,i-1) for i in range(nss)]
	samples = [1 for i in range(nss/2+1)]
	def bound():
		d = 0
		for i in range(1,nss/2+1):
			d+=maurer_bound(nss*sample_max_numbers[i],i*samples[i],y,nss)
			d+=maurer_bound(nss*sample_max_numbers[nss-i],(nss-i)*samples[i],y,nss)
		return d
	while sum(samples) < total_samples:
		best_sample = 0
		best_decrease = 0
		b = bound()
		for i in range(nss/2+1):
			samples[i] += 1
			decrease = b-bound()
			samples[i] -= 1
			if decrease>best_decrease:
				best_decrease=decrease
				best_sample=i
		samples[best_sample] += 1
	return samples

def generate_indices(nss,samples):
	numbers = get_sample_numbers(nss,0.5,samples)
	#print samples,numbers
	return get_inds(nss,numbers)

def simple_generate_indices(nss,samples):
	s = [0 for i in range(nss/2+1)]
	maxs = [ncr(nss,i) for i in range(nss/2+1)]
	if nss%2==0:
		maxs[-1]/=2
	if samples>sum(maxs):
		return get_inds(nss,maxs)
	i=0
	j=0
	while(sum(s)<samples):
		if (s[i]<maxs[i]):
			s[i]+=1
		i=i+1
		if (i==nss/2 and nss%2==0 and j==1) or i==nss/2+1:
			i=0
			j=1-j
	return get_inds(nss,s)

