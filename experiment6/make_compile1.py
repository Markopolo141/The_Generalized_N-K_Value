
with open("compile1.sh","w") as f:
	f.write("cd ./k_generator\n");
	f.write("./compile.sh\n");
	f.write("cd ..\n");
	for i in [90]:#range(10,100,10):
		f.write("echo \"*** {} bus networks ***\"\n".format(i))
		for j in [1]:#range(1,40):
			f.write("python network_generator.py {} {} {}\n".format(i,(int)(i*6.0/5),1))
			f.write("python lmp.py {}.json lmp_out.json\n".format(1))
			f.write("python bounds_solver.py {}.json gnk_out.json 8 0.01 0.2 10\n".format(1))
