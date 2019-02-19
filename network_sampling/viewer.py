import json
import pdb
import click



@click.command()
@click.argument('data_file', type=click.File('rb'))
def run(data_file):
	data = json.load(data_file)
	data_file.close()
	for d in data:
		print "({})".format(",".join([str(dd) for dd in d]))


if __name__ == '__main__':
    run()

