import json
import pdb
import click
from utils import *



@click.command()
@click.argument('exact_data', type=click.File('rb'))
@click.argument('stat_data', type=click.File('rb'))
@click.argument('bound_data', type=click.File('rb'))
@click.argument('output_data', type=click.File('wb'))
def run(exact_data, stat_data, bound_data, output_data):
	exact = json.load(exact_data)
	stat = json.load(stat_data)
	bound = json.load(bound_data)
	exact_data.close()
	stat_data.close()
	bound_data.close()
	
	#normalising all the data
	for a in [stat,bound]:
		for b in a:
			for c in b:
				c[-1] = [d*1.0/exact[0] for d in c[-1]]
	exact[-1] = [c*1.0/exact[0] for c in exact[-1]]

	#taking the absolute error of all the data
	for a in [stat,bound]:
		for b in a:
			for c in b:
				c[-1] = [abs(d-exact[-1][i]) for i,d in enumerate(c[-1])]

	#taking the average of optimisations and absolute errors
	for a in [stat,bound]:
		for b in range(len(a)):
			data = [sum([c[0] for c in a[b]])/len(a[b])]
			data.append(
				sum([sum([c[-1][i] for c in a[b]])*1.0/len(a[b]) for i in range(len(a[b][0][-1]))])*1.0/len(a[b][0][-1])
			)
			a[b] = data

	#interpolate stat data to bound data and print
	stat = data_interpolate(stat, [b[0] for b in bound], 0,1)

	#format and ouptput processed data
	final_data = [[bound[i][0],stat[i][1],bound[i][1]] for i in range(len(stat))]
	output_data.open()
	json.dump(final_data,output_data)
	output_data.close()



if __name__ == '__main__':
    run()

