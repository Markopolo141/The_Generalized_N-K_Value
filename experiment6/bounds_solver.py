from random import shuffle,random
import click
import json
from multiprocessing import Process, Manager
import time
from math import sqrt
from minimax_solver import setup, new_calc_maxmin_minmax, calc_maxmin_minmax, calc_old_maxmin_minmax, spruik_solver
import gmpy2

indsplus = None
N = None
method = None

def v(s):
	global indsplus
	if indsplus is None:
		indsplus = method((1<<N)-1)
	ii = 0
	for ss in s:
		ii |= 1<<ss;
	return 0.5*method(ii) + 0.5*indsplus

def worker(number,data,ppc):
	gmpy2.get_context().precision = 100
	m = 1
	setup(ppc)
	temp_data = [[0 for i in range(N)] for i in range(N)]
	mm = [[gmpy2.mpfr(0.0) for i in range(N)] for ii in range(N)]
	ss = [[0 for i in range(N)] for ii in range(N)]
	v0 = 0.0#v([])
	vector = list(range(N))
	while (True):
		for i in range(0,m,N):
			shuffle(vector)
			prev_v = v0
			spruik_solver()
			for ii,vv in enumerate(vector): #size ii, player vv
				new_v = v(vector[0:ii+1])
				x = new_v-prev_v
				prev_v = new_v
				mm[ii][vv] += x
				mm[N-ii-1][vv] += x
				ss[ii][vv] += 1
				ss[N-ii-1][vv] += 1
		for i in range(N):
			for ii in range(N):
				temp_data[i][ii] = mm[i][ii]/ss[i][ii] if ss[i][ii]>0 else 0#indsplus*random()*2.0/N #random()*1000
		data[number] = [float(sum([temp_data[o][i] for o in range(N)])/N) for i in range(N)]



def proc_start(number, data, ppc):
    p_to_start = Process(target=worker,name="worker{}".format(number),args=(number,data,ppc))
    p_to_start.start()
    return p_to_start
def proc_stop(p_to_stop):
    p_to_stop.terminate()



@click.command()
@click.argument('input_file', type=click.File('rb'))
@click.argument('output_file', type=click.File('a'))
@click.argument('thread_number', type=click.INT)
@click.argument('resolution_finish', type=click.FLOAT)
@click.argument('resolution_iterate', type=click.FLOAT)
@click.argument('repeat_finish', type=click.INT)
@click.argument('mode')
def run(input_file, output_file, thread_number, resolution_finish, resolution_iterate ,repeat_finish, mode):
	print "---START---"
	global N, method
	ppc = json.load(input_file)
	N = len(ppc['bus'])
	assert mode in ['newnew','new', 'old']
	if mode=="newnew":
		method = new_calc_maxmin_minmax
	elif mode=="new":
		method = calc_maxmin_minmax
	elif mode=="old":
		method = calc_old_maxmin_minmax
	
	manager = Manager()
	data = manager.list([[i for ii in range(N)] for i in range(thread_number)])
	
	procs = [proc_start(i,data,ppc) for i in range(thread_number)]
	t = time.time()
	exiting = 0
	try:
		while (exiting <repeat_finish):
			time.sleep(resolution_iterate)
			avg = [sum([d[i] for d in data])*1.0/thread_number for i in range(N)]
			average_delta = sum([sqrt(sum([(d[i]-avg[i])**2 for i in range(N)])) for d in data])*1.0/thread_number
			magnitude = sqrt(sum([avg[i]**2 for i in range(N)]))
			magerror = average_delta*1.0/magnitude if magnitude != 0 else 1
			print "relative magnitude error: {}".format(magerror)
			#print "relative magnitude error: {} {}".format(magerror, data)
			if ((magerror<resolution_finish) or (magnitude==0)):
				exiting += 1
			else:
				exiting = 0
	except KeyboardInterrupt as e:
		pass
	
	for p in procs:
		proc_stop(p)
	#data = [[dd for dd in d] for d in data]
	#output_file.write(json.dumps(data).replace("]","]\n"))
	if (magnitude>0):
		output_file.write("{} {}\n".format(N, avg))
	#	output_file.write("{} {}\n".format(N,time.time()-t))
	output_file.close()

if __name__ == '__main__':
    run()




