
with open("compile2.sh","w") as f:
	f.write("cd ./k_generator\n");
	f.write("./compile.sh\n");
	f.write("cd ..\n");
	for i in range(5,100):
		for j in range(1,3):
			f.write("python network_generator.py {} {} {}\n".format(i,(int)(i*6.0/5),j))
		for j in range(1,3):
			f.write("python timed_bounds_solver.py {}.json test_out2.json 8 180\n".format(j))
