import json
from simple_index_generator import generate_indices,simple_generate_indices
import tqdm
import pdb
import click
from reduced_solver_wrapper import make_calculator,shared, mem_inds



@click.command()
@click.argument('input_file', type=click.File('rb'))
@click.argument('output_file', type=click.File('wb'))
@click.argument('sample_start', type=click.INT)
@click.argument('sample_finish', type=click.INT)
@click.argument('sample_step', type=click.INT)
@click.argument('repeats', type=click.INT)
@click.option('--inds_data', type=click.File('rb'))
def run(input_file, output_file, sample_start, sample_finish, sample_step, repeats,inds_data):
	if inds_data is not None:
		inds_data_ = json.load(inds_data)
		inds_data.close()
		for k in inds_data_.keys():
			mem_inds[k] = inds_data_[k]
	ppc = json.load(input_file)
	shared.setConst(ppc=ppc)
	shared.setConst(debug=False)
	shared.setConst(bignum=99999)
	nss = len(ppc['bus'])
	data = []
	#for depth in range(1,66):#tqdm.tqdm(range(1,66)):
	for depth in range(sample_start,sample_finish,sample_step):
		data.append([])
		for ii in range(repeats):
			calculate_inds, calc = make_calculator(ppc)
			mem_inds['_counter_inds']=[]
			indices = simple_generate_indices(nss, depth)
			#indices = generate_indices(nss, depth)
			calculate_inds(indices)
			cc = calc()
			print 2*len(mem_inds['_counter_inds'])-1,depth,cc[0]
			data[-1].append((2*len(mem_inds['_counter_inds'])-1,depth,cc[0]))
	output_file.write(json.dumps(data).replace("]","]\n"))
	output_file.close()
		
if __name__ == '__main__':
    run()
	



