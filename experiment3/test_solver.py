from random import shuffle,random,seed
import click
import json
import time
from math import sqrt
from minimax_solver import setup, calc_maxmin_minmax, spruik_solver

N = None

def v(s):
	ii = 0
	for ss in s:
		ii |= 1<<ss;
	#return 0.5*calc_maxmin_minmax(ii)
	return calc_maxmin_minmax(ii)

#def v(s):
#	return sqrt(sum(s))


def worker(number,data,ppc):
	seed(32000)
	m = 100
	setup(ppc)
	listi = list(range(N))
	shuffle(listi)
	#spruik_solver()
	for ii in range(3):
		#print "hello"
		output = [0.0]
		for i in range(1,N):#N+1):
			#if (int(random()*4)==0):
			#	spruik_solver()
			vv = listi[0:i]
			#print "ZOGZOGZOG {} {}".format(v(vv),vv)
			#v(vv)
			#print "{}\t".format(v(vv)),
			output.append(int(v(vv)))
		print output
		#print ""




@click.command()
@click.argument('input_file', type=click.File('rb'))
def run(input_file):
	global N
	ppc = json.load(input_file)
	N = len(ppc['bus'])
	worker(0,[[]],ppc)
	
		
if __name__ == '__main__':
    run()




