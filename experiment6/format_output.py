import json

with open("g.json","r") as f:
	data = json.load(f)
	for i in range(len(data[0])):
		print "({},{})".format(i,data[0][i]),
	print ""
	print ""
	for i in range(len(data[1])):
		print "({},{})".format(i,data[1][i]),
	
