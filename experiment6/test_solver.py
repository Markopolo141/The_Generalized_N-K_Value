import click
import json
from minimax_solver import setup, calc_maxmin_minmax, spruik_solver

N = None


@click.command()
@click.argument('input_file', type=click.File('rb'))
def run(input_file):
	print "---START---"
	global N
	ppc = json.load(input_file)
	N = len(ppc['bus'])
	setup(ppc)
	indsplus = calc_maxmin_minmax((1<<N)-1)
	print "indsplus = {}".format(indsplus)
	input_file.close()

if __name__ == '__main__':
    run()




