import json
from random import random, choice, randint, shuffle
from math import floor
import pdb
import click

import click

@click.command()
@click.argument('n', type=int)
@click.argument('k', type=int)
@click.argument('name')
def run(n,k,name):
	no_links = list(range(n))
	no_links_index = list(range(n))
	shuffle(no_links_index)
	while sum(no_links)>k:
		m = max(no_links)
		for i in range(n):
			if no_links[no_links_index[i]]>m-1:
				no_links[no_links_index[i]]=m-1
				break

	bx = []
	by = []
	links = []

	def add_link(x,y,i):
		min_d2 = None
		min_d2_index = None
		for o in range(i):
			if (o,i) not in links:
				d2 = (x-bx[o])**2 + (y-by[o])**2
				if min_d2 is None or d2 < min_d2:
					min_d2 = d2
					min_d2_index = o
		if min_d2 is not None:
			links.append((min_d2_index,i))
			return 1
		return 0
	for i in range(n):
		x = random()
		y = random()
		for z in range(no_links[i]):
			add_link(x,y,i)
		bx.append(x)
		by.append(y)
	for i in range(len(links)):
		links[i] = (links[i][0]+1,links[i][1]+1)

	with open("{}.dot".format(name),"w") as f:
		f.write("graph graphname {\n");
		for l in links:
			f.write("    p{} -- p{};\n".format(l[0],l[1]))
		f.write("}");


	ppc = {"bus":[],"gen":[],"branch":[],"gencost":[]}
	b = []
	for l in links:
		if l[0] not in b:
			b.append(l[0])
		if l[1] not in b:
			b.append(l[1])
	for bb in b:
		ppc['bus'].append([bb,-1,0])
		if random()>0.8:
			ppc['gen'].append([bb, -1,-1,-1,-1,-1,-1,-1, randint(10,201), 0])
		else:
			ppc['gen'].append([bb, -1,-1,-1,-1,-1,-1,-1, 0, -randint(10,201)])
		ppc['gencost'].append([2, 0, 0, 2, randint(0,20)*0.1+0.1, 0])
	for l in links:
		#ppc['branch'].append([l[0], l[1], -1,-1,  randint(0,60)*0.1+0.1, randint(20,301)])
		ppc['branch'].append([l[0], l[1], -1,-1,  1, randint(20,301)])
	with open("{}.json".format(name),"w") as f:
		f.write(json.dumps(ppc))

if __name__ == '__main__':
    run()
