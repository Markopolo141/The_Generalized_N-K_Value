
import click
from math import sqrt

@click.command()
@click.argument('input_file1', type=click.File('rb'))
@click.argument('input_file2', type=click.File('rb'))
@click.argument('output_file', type=click.File('w'))
def run(input_file1, input_file2, output_file):
	print "---START---"
	data1 = input_file1.readlines()
	data1 = [d.strip('[]\n ').split(',') for d in data1]
	data1 = [[float(dd) for dd in d] for d in data1 if len(d)>1]
	#mag_data1 = [sqrt(sum([dd**2 for dd in d])) for d in data1]
	input_file1.close()

	data2 = input_file2.readlines()
	data2 = [d.strip('[]\n ').split(',') for d in data2]
	data2 = [[float(dd) for dd in d] for d in data2 if len(d)>1]
	#mag_data2 = [sqrt(sum([dd**2 for dd in d])) for d in data2]
	input_file2.close()

	length = min(len(data1),len(data2))

	#mag_data = [(mag_data1[i]+mag_data2[i])*1.0/2 for i in range(length)]
	mag_data = [sum(data1[i]) for i in range(length)]
	delta_data = [[abs(data1[i][o]-data2[i][o])/mag_data[i] for o in range(len(data1[0]))] for i in range(length) if mag_data[i]!=0]

	delta_data = sorted(sum(delta_data,[]), reverse=True)

	data_points = len(delta_data)
	
	for i,d in enumerate(delta_data):
		output_file.write("({},{})({},{})".format(d,1.0-i*1.0/(data_points+1),d,1.0-(i+1)*1.0/(data_points+1)))

	#print mag_data1
	#print mag_data2
	#print ""
	#print delta_data

	output_file.close()
	print "---DONE---"

if __name__ == '__main__':
    run()
