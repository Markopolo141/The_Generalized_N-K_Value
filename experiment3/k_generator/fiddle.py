

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


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

mat=[
[0.700000,	-0.800000,	-1.900000,	1.300000,	-0.400000,	1.500000,	0.000000,	0.000000],
[1.000000,	0.000000,	1.000000,	1.000000,	1.000000,	0.000000,	1.000000,	-1.000000],
[0.000000,	0.000000,	0.000000,	0.000000,	0.000000,	1.000000,	0.000000,	1.000000],
[0.000000,	1.000000,	0.000000,	0.000000,	0.000000,	0.000000,	1.000000,	0.000000]
]
mat2 = [
0,
45.000000,
32.000000,
77.000000
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
	pivots = []
	for i in range(len(mat[0])-1):
		pivots.append([0,])
		minv = float("inf")
		for j in range(1,len(mat)):
			pivots[-1].append(0)
			if mat[j][i]>0:
				v = mat[j][len(mat[0])-1]/mat[j][i]
				if abs(minv-v)<0.00001:
					pivots[-1][-1]=1
				elif v<minv:
					minv=v
					pivots[-1] = [0 for p in pivots[-1]]
					pivots[-1][-1]=1
	for i in range(len(mat[0])-1):
		for j in range(1,len(mat)):
			if mat[j][i]<0 and mat[j][len(mat[0])-1] == 0:
				pivots[i][j]=1
				
	for j,m in enumerate(mat):
		print "   :{}\t{}\t|".format(j,"\t".join(["|{0}{1:.1f}{2}".format(bcolors.WARNING if (i<len(mat[0])-1 and pivots[i][j]==1) else "",mm,bcolors.ENDC if (i<len(mat[0])-1 and pivots[i][j]==1) else "") for i,mm in enumerate(m)]))
	#r2 = int(raw_input("which target_row: "))
	c = parse_int("which column: ")
	r = parse_int("which source_row: ")
	normalize_row(r,c)
	#add_row(r,-mat[r2][c],r2)
	for i in range(eqns):
		if i!=r:
			add_row(r,-mat[i][c],i)


