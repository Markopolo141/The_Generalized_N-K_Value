try:
	from tqdm import tqdm
	tqdm_enabled=True
except ImportError:
	tqdm_enabled=False



# for a set of x-y datapoints, return the datapoint that is the linear interpolation/extrapolation to a yvalue yval.
#  fails if there are less than two datpoints (obviously) but optionally more than two.
# eg. interpolate([(2, 1), (4, 2), (8, 0)],3)
#      = (6, 3)
def interpolate(dataxy, yval,min_points=2,extrapolation=True,high_ordering=True):
	if len(dataxy)<min_points:
		return None
	index1 = 0
	index2 = 1
	value1 = dataxy[0][1] - yval
	value2 = dataxy[1][1] - yval
	if value1<value2:
		value1,index1,value2,index2 = value2,index2,value1,index1
	if value1==0 and value2==0:
		if high_ordering:
			zero_index = max(index1,index2)
		else:
			zero_index = min(index1,index2)
	elif value1==0:
		zero_index = index1
	elif value2==0:
		zero_index = index2
	else:
		zero_index = -1
	zero_value = None if zero_index==-1 else dataxy[zero_index][0]
	for i in range(2,len(dataxy)):
		dxy = dataxy[i]
		v = dxy[1] - yval
		if zero_index < 0:
			if v>0:
				if value1<0:
					value1,index1,value2,index2 = v,i,value1,index1
				elif v<value1:
					value1,index1 = v,i
				else:
					continue
			elif v<0:
				if value2>0:
					value1,index1,value2,index2 = value2,index2,v,i
				elif v>value2:
					value2,index2 = v,i
				else:
					continue
			elif v==0:
				zero_index = i
				zero_value = dxy[0]
				continue
			if value1<value2:
				value1,index1,value2,index2 = value2,index2,value1,index1
		elif v==0:
			if (high_ordering and dxy[0]>zero_value) or ((not high_ordering) and dxy[0]<zero_value):
				zero_index = i
				zero_value = dxy[0]
	if zero_index>=0:
		return zero_value,yval
	if (extrapolation is not True) and (value2*value1>0):
		return None
	p2 = dataxy[index1]
	p1 = dataxy[index2]
	if p2[1]-p1[1] !=0:
		f = (yval-p1[1])/(p2[1]-p1[1])
		return f*(p2[0]-p1[0]) + p1[0],yval
	else:
		return (p2[0]+p1[0])*1.0/2,yval

# takes a list of data points, and for all datapoint linearly interpolates (optionally extrapolates) the values on the 'value_col'th to be the numbers of 'values', 
# offsetting as nessisary values from source_col.
# eg. data_interpolate([[0,1,2],[0,2,4],[0,0,8],[3,0,0],[3,1,1]],[3,5,8],1,2)
#      = [[0, 3, 6], [0, 5, 10], [0, 8, 16], [3, 3, 3], [3, 5, 5], [3, 8, 8]]
def data_interpolate(data, values, value_col=1,source_col=0,min_points=2,extrapolation=True,high_ordering=True,debug=False,kulling_singles=True):
	assert len(data)>0
	assert value_col!=source_col
	coord_cols = list(range(len(data[0])))
	coord_cols.pop(max(value_col,source_col))
	coord_cols.pop(min(value_col,source_col))
	coord_pairs = [[] for i in range(len(coord_cols))] if len(coord_cols)>0 else None
	coord_points = []
	coord_point_insertion_index = 0
	if debug:
		print " -segregating data:"
		if tqdm_enabled:
			iterator = tqdm(data)
		else:
			iterator = data
	else:
		iterator = data
	for d in iterator:
		temp_coord_indices = [True for i in range(len(coord_points))]
		has_match = len(coord_points)>0
		for ii,i in enumerate(coord_cols):
			temp_coord_indices2 = []
			has_match = False
			for tt,t in enumerate(temp_coord_indices):
				match = (t and (d[i]==coord_pairs[ii][tt]))
				temp_coord_indices2.append(match)
				has_match = match or has_match
				if match:
					coord_point_insertion_index = tt
			temp_coord_indices = temp_coord_indices2
			if not has_match:
				break
		if has_match:
			coord_points[coord_point_insertion_index].append((d[source_col],d[value_col]))
		else:
			coord_points.append([(d[source_col],d[value_col])])
			for ii,i in enumerate(coord_cols):
				coord_pairs[ii].append(d[i])
	new_coord_points = []
	if debug:
		print " -interpolating segregated data:"
		if tqdm_enabled:
			iterator = tqdm(coord_points)
		else:
			iterator = coord_points
	else:
		iterator = coord_points
	for c in iterator:
		n = []
		for v in values:
			nn = interpolate(c,v,min_points=min_points,extrapolation=extrapolation,high_ordering=high_ordering)
			if nn is not None:
				n.append(nn)
		new_coord_points.append(n)
	coord_pairs = zip(*coord_pairs) if coord_pairs is not None else [[]]
	new_data = []
	if debug:
		print " -desegregating data:"
		if tqdm_enabled:
			iterator = tqdm(enumerate(coord_pairs))
		else:
			iterator = enumerate(coord_pairs)
	else:
		iterator = enumerate(coord_pairs)
	for i,c in iterator:
		if (not kulling_singles) or len(new_coord_points[i])>1:
			for cp in new_coord_points[i]:
				a = list(c)
				if source_col<value_col:
					a.insert(source_col,cp[0])
					a.insert(value_col,cp[1])
				else:
					a.insert(value_col,cp[1])
					a.insert(source_col,cp[0])
				new_data.append(a)
	return new_data
