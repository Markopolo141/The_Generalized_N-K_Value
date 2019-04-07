

def RepresentsInt(s):
    try: 
        int(s)
        return True
    except ValueError:
        return False

import pdb
from random import randint
import sys

'''mat = [
[0,0,-1],
[1,0,-0.1],
[0,1,-0.1],
[-0.1,-0.1,1],
[0.5,0.2,1],
[0.2,0.5,1],
]
mat2 = [0,1,1,1,1.5,1.4]'''

mat = [
[0.000000,	-0.476190,	0.000000,	0.000000,	0.952381,	0.000000,	0.000000],
[0.000000,	0.285714,	0.000000,	0.000000,	-0.971429,	1.000000,	0.000000],
[0.000000,	-0.047619,	1.000000,	0.000000,	0.095238,	0.000000,	0.000000],
[0.000000,	0.571429,	0.000000,	1.000000,	-0.942857,	0.000000,	0.000000],
[0.000000,	0.952381,	0.000000,	0.000000,	0.095238,	0.000000,	0.000000],
[1.000000,	-0.476190,	0.000000,	0.000000,	0.952381,	0.000000,	0.000000],
[0.000000,	-0.476190,	0.000000,	0.000000,	0.952381,	0.000000,	1.000000]
]
mat2 = [
0.952381,
0.228571,
1.095238,
0.157143,
0.0,
0.952381,
0.095238
]

adding_constraints = False
eqns = len(mat)
num_1 = len(mat[0])
num_2 = 0
num_s = len(mat)-1 if adding_constraints else 0
if (adding_constraints):
	mat = [mat[i] + [1 if j+1==i else 0 for j in range(len(mat)-1)] + [mat2[i]] for i in range(len(mat))]
else:
	mat = [mat[i] + [mat2[i]] for i in range(len(mat))]

def add_row(input_row, multiplier,added_row):
	for i in range(len(mat[0])):
		mat[added_row][i] += multiplier*mat[input_row][i]

def normalize_row(input_row,column):
	a = mat[input_row][column]
	for i in range(len(mat[0])):
		mat[input_row][i] = mat[input_row][i]*1.0/a

def parse_int(st):
	z = raw_input(st)
	while (z!="q") and (not RepresentsInt(z)):
		z = raw_input(st)
	if z=="q":
		sys.exit(0)
	return int(z)
	

while True:
	print "\t{}".format("\t".join([" _{}_".format(i) for i in range(num_1+num_2+num_s+1)]))
	for i,m in enumerate(mat):
		print "   :{}\t{}\t|".format(i,"\t".join(["|{0:.2f}".format(mm) for mm in m]))
	#r2 = int(raw_input("which target_row: "))
	c = parse_int("which column: ")
	r = parse_int("which source_row: ")
	normalize_row(r,c)
	#add_row(r,-mat[r2][c],r2)
	for i in range(eqns):
		if i!=r:
			add_row(r,-mat[i][c],i)


