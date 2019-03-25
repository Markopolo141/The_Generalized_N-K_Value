import os
import click

@click.command()
@click.argument('f')
def run(f):
	print "--- Generating Network"
	os.system("python network_generator.py 11 13 {}".format(f))
	print "--- Printing Network"
	os.system("neato -Tpng {0}.dot > {0}.png".format(f))
	print "--- Solving Network"
	os.system("python solver.py {}.json solved{}out.json --inds_data=inds{}.json".format(f,f,f))
	print "--- Solving Network via simple statistics"
	os.system("python stat_solver.py {}.json stat{}out.json 20 1020 5 70 --inds_data=inds{}.json".format(f,f,f))
	print "--- Solving Network via stratified statistics:BURGESS"
	os.system("python bounds_solver.py burgess {}.json bound_burgess{}out.json 209 2800 20 100 --inds_data=inds{}.json".format(f,f,f))
	print "--- Solving Network via stratified statistics:SIMPLE"
	os.system("python bounds_solver.py simple {}.json bound_simple{}out.json 209 3800 20 100 --inds_data=inds{}.json".format(f,f,f))
	print "--- Solving Network via stratified statistics:MALEKI"
	os.system("python bounds_solver.py maleki {}.json bound_maleki{}out.json 209 3400 20 100 --inds_data=inds{}.json".format(f,f,f))
	print "--- Solving Network via stratified statistics:CASTRO"
	os.system("python bounds_solver.py castro {}.json bound_castro{}out.json 209 3400 20 100 --inds_data=inds{}.json".format(f,f,f))
	print "--- Solving Network via stratified statistics:APPROSHAPLEY"
	os.system("python bounds_solver.py approshapley {}.json bound_approshapley{}out.json 10 3400 20 100 --inds_data=inds{}.json".format(f,f,f))
	print "--- Collating all statistical outcomes"
	os.system("python data_renderer.py solved{}out.json stat{}out.json ./processed/stat{}.json".format(f,f,f))
	os.system("python data_renderer.py solved{}out.json bound_burgess{}out.json ./processed/burgess{}.json".format(f,f,f))
	os.system("python data_renderer.py solved{}out.json bound_simple{}out.json ./processed/simple{}.json".format(f,f,f))
	os.system("python data_renderer.py solved{}out.json bound_maleki{}out.json ./processed/maleki{}.json".format(f,f,f))
	os.system("python data_renderer.py solved{}out.json bound_castro{}out.json ./processed/castro{}.json".format(f,f,f))
	os.system("python data_renderer.py solved{}out.json bound_approshapley{}out.json ./processed/approshapley{}.json".format(f,f,f))


if __name__ == '__main__':
    run()
