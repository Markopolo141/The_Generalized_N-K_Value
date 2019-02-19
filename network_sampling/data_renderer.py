import json
import pdb
import click



@click.command()
@click.argument('exact_data', type=click.File('rb'))
@click.argument('stat_data', type=click.File('rb'))
@click.argument('output_data', type=click.File('wb'))
def run(exact_data, stat_data,output_data):
	exact = json.load(exact_data)
	stat = json.load(stat_data)
	exact_data.close()
	stat_data.close()
	
	#normalising all the data
	for b in stat:
		for c in b:
			c[-1] = [d*1.0/exact[0] for d in c[-1]]
	exact[-1] = [c*1.0/exact[0] for c in exact[-1]]

	#taking the absolute error of all the data
	for b in stat:
		for c in b:
			c[-1] = [abs(d-exact[-1][i]) for i,d in enumerate(c[-1])]

	#taking the average of optimisations and absolute errors
	for b in range(len(stat)):
		data = [sum([c[0] for c in stat[b]])/len(stat[b])]
		data.append(
			sum([sum([c[-1][i] for c in stat[b]])*1.0/len(stat[b]) for i in range(len(stat[b][0][-1]))])*1.0/len(stat[b][0][-1])
		)
		stat[b] = data

	#output processed data
	output_data.open()
	json.dump(stat,output_data)
	output_data.close()



if __name__ == '__main__':
    run()

