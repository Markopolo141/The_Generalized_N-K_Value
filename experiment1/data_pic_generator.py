import json

with open('data.json',"r") as f:
	data = json.load(f)
	xticks = data['xticks']
	data = data['data']

formatting = [
[1,lambda data,i,j:-data[i][2][j]],
[2,lambda data,i,j:data[i][1][j]],
[3,lambda data,i,j:data[i][0][j]],
[5,lambda data,i,j:data[i][0][j]-data[i][1][j]],
[6,lambda data,i,j:data[i][5][j]],
]

for k,fo in formatting:
	with open("data{}.txt".format(k),"w") as f:
		for i in range(len(data)):
			for j in range(len(xticks)):
				f.write("({},{})".format(xticks[j],fo(data,i,j)))
			f.write("\n\n")


'''for i,d in enumerate(data):
	with open("data{}.txt".format(i),"w") as f:
		for dd in d:
			for j in range(len(dd)):
				f.write("({},{})".format(xticks[j],dd[j]))
			f.write("\n\n")'''
