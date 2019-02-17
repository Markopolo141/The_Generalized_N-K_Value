import json
from simple_index_generator import get_inds,ncr
import tqdm
import pdb
import click
from reduced_solver_wrapper import make_calculator,shared, mem_inds



@click.command()
@click.argument('input_file', type=click.File('rb'))
@click.argument('output_file', type=click.File('wb'))
@click.option('--inds_data', type=click.File('wb'))
def run(input_file, output_file,inds_data):
	ppc = json.load(input_file)
	shared.setConst(ppc=ppc)
	shared.setConst(debug=False)
	shared.setConst(bignum=99999)
	nss = len(ppc['bus'])
	calculate_inds, calc = make_calculator(ppc)

	maxs = [ncr(nss,i) for i in range(nss/2+1)]
	if nss%2==0:
		maxs[-1]/=2
	indices = get_inds(nss,maxs)
	calculate_inds(indices,True)
	cc = calc()
	data = [2*len(mem_inds['_counter_inds'])-1,cc[0]]
	del mem_inds['_counter_inds']
	if inds_data is not None:
		inds_data.open()
		json.dump(mem_inds,inds_data)
		inds_data.close()
	data = [max([m[0] for m in mem_inds.values()])]+data 
	print data
	output_file.write(json.dumps(data))
	output_file.close()
		
if __name__ == '__main__':
    run()
	



