from random import shuffle,random
import click
import json
from multiprocessing import Process, Manager
import time
from math import sqrt
from minimax_solver import setup, calc_maxmin_minmax, spruik_solver
import tqdm

N = None



@click.command()
@click.argument('input_file', type=click.File('rb'))
@click.argument('output_file', type=click.File('w'))
def run(input_file, output_file):
	print "---START---"
	global N
	ppc = json.load(input_file)
	N = len(ppc['bus'])
	setup(ppc)
	
	data = []
	for i in tqdm.tqdm(range(2**N)):
		data.append([i,calc_maxmin_minmax(i)])
	output_file.write(json.dumps(data).replace("]","]\n"))
	#	output_file.write("{} {}\n".format(N,time.time()-t))
	output_file.close()

if __name__ == '__main__':
    run()




