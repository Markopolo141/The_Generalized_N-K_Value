import json

def return_scaling(w,h,zoom):
	w = float(w)
	h = float(h)
	zoom = float(zoom)
	izoom = 1.0/zoom
	return {"figsize":[izoom, izoom*h/w], "dpi":w*zoom}

with open('data.json',"r") as f:
	data = json.load(f)
	xticks = data['xticks']
	data = data['data']

import matplotlib.pyplot as plt
plt.figure(1,**return_scaling(400,300,0.3))
for i in range(len(data)):
	plt.plot(xticks,[-a for a in data[i][2]], label="{}".format(i+1))
#plt.legend(loc=2)
plt.xlabel("$x$")
plt.savefig("z1.png", bbox_inches='tight', pad_inches=0)
plt.figure(2,**return_scaling(400,300,0.3))
for i in range(len(data)):
	plt.plot(xticks,data[i][1], label="{}".format(i+1))
#plt.legend(loc=2)
plt.xlabel("$x$")
plt.savefig("z2.png", bbox_inches='tight', pad_inches=0)
plt.figure(3,**return_scaling(800,600,0.15))
for i in range(len(data)):
	#plt.plot(xticks,[data[i][0][a]-data[i][1][a] for a,z in enumerate(data[i][1])], label="Value-Payment-{}".format(i+1))
	plt.plot(xticks,[data[i][0][a] for a,z in enumerate(data[i][1])], label="{}".format(i+1))
	#plt.plot(xticks,data[i][5], label="LMP-Payment-{}".format(i+1))
plt.legend(loc=2)
plt.xlabel("$x$")
plt.savefig("z3.png", bbox_inches='tight', pad_inches=0)
plt.figure(4,**return_scaling(800,600,0.15))
for i in range(len(data)):
	plt.plot(xticks,data[i][0], label="Value-Utility-{}".format(i+1))
	plt.plot(xticks,[data[i][5][a]+data[i][1][a] for a,z in enumerate(data[i][1])], label="LMP-Utility-{}".format(i+1))
plt.legend(loc=2)
plt.xlabel("$x$")
plt.savefig("z4.png", bbox_inches='tight', pad_inches=0)

plt.figure(5,**return_scaling(800,600,0.15))
for i in range(len(data)):
	plt.plot(xticks,[data[i][0][a]-data[i][1][a] for a,z in enumerate(data[i][1])], label="{}".format(i+1))
plt.legend(loc=2)
plt.xlabel("$x$")
plt.savefig("z5.png", bbox_inches='tight', pad_inches=0)
plt.figure(6,**return_scaling(800,600,0.15))
for i in range(len(data)):
	plt.plot(xticks,data[i][5], label="{}".format(i+1))
plt.legend(loc=2)
plt.xlabel("$x$")
plt.savefig("z6.png", bbox_inches='tight', pad_inches=0)
#plt.show()
