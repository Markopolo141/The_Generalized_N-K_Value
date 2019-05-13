from random import shuffle,random,seed
import click
import json
import time
import math
import pdb
import sys

N = None

def count_bits(a):
	b = 0
	while (a!=0):
		b += a&1
		a >>= 1
	return b

@click.command()
@click.argument('input_file', type=click.File('rb'))
@click.argument('output_file', type=click.File('a'))
def run(input_file, output_file):
	data = json.load(input_file)
	global N
	N = max([a[0] for a in data])
	N = int(round(math.log(N+1)/math.log(2)))
	
	temp_data = [[0 for i in range(N)] for i in range(N)]
	mm = [[0.0 for i in range(N)] for ii in range(N)]
	ss = [[0 for i in range(N)] for ii in range(N)]
	
	print N
	for d in data:
		a = d[0]
		for i in range(N):
			if ((a>>i)&1)==1:
				c = count_bits(a)
				mm[c-1][i] += d[1]
				ss[c-1][i] += 1
	for i in range(N):
		for ii in range(N):
			temp_data[i][ii] = mm[i][ii]/ss[i][ii] if ss[i][ii]>0 else 0
	out_data = [sum([temp_data[o][i] for o in range(N)])/N for i in range(N)]
	output_file.write(json.dumps(out_data))
	output_file.write("\n")
	output_file.close()

if __name__ == '__main__':
    run()




