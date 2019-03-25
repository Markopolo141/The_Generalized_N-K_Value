import sys
import json
import math
import pdb
from utils import *

assert len(sys.argv)>2

data = []
data_mins = []
data_maxes = []
for filename in sys.argv[2:]:
	with open(filename,"rb") as f:
		data.append(json.load(f))
data_mins = []
data_maxes = []
for d in data:
	data_mins.append(  min([dd[0] for dd in d]) )
	data_maxes.append( max([dd[0] for dd in d]) )
data_min = int(math.ceil(max(data_mins)))
data_max = int(math.floor(min(data_maxes)))
data_points = list(range(data_min,data_max+1))

for i in range(len(data)):
	data[i] = data_interpolate(data[i],data_points,0,1)


zog = [[data_points[i],sum([d[i][1] for d in data])*1.0/len(data)] for i in range(len(data_points))]

with open(sys.argv[1],"wb") as f:
	json.dump(zog,f)

