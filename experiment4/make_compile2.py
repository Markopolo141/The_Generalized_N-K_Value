
with open("compile1.sh","w") as f:
	f.write("cd ./k_generator\n");
	f.write("./compile.sh\n");
	f.write("cd ..\n");
	for i in range(59,106):
		for j in range(1,6):
			f.write("python network_generator.py {} {} {}\n".format(i,(int)(i*6.0/5),j))
		for j in range(1,6):
			f.write("python bounds_solver.py {}.json test_out.json 8 0.01 10\n".format(j))
