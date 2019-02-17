import os
import click

@click.command()
@click.argument('f')
def run(f):
	print "--- Generating Network"
	os.system("python network_generator.py 7 9 {}".format(f))
	print "--- Printing Network"
	os.system("neato -Tpng {0}.dot > {0}.png".format(f))
	print "--- Solving Network"
	os.system("python solver.py {}.json solved{}out.json --inds_data=inds{}.json".format(f,f,f))
	print "--- Solving Network via simple statistics"
	os.system("python stat_solver.py {}.json stat{}out.json 5 50 1 20 --inds_data=inds{}.json".format(f,f,f))
	print "--- Solving Network via stratified statistics"
	os.system("python bounds_solver.py {}.json bound{}out.json 312 1456 13 60 --inds_data=inds{}.json".format(f,f,f))
	print "--- Collating all statistical outcomes"
	os.system("python data_collator.py solved{}out.json stat{}out.json bound{}out.json final{}out.json".format(f,f,f,f))


if __name__ == '__main__':
    run()
