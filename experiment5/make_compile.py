with open("compile.sh","w") as f:
	f.write("echo \"WARNING: sometimes the solver will hang, have to kill it in that event...\"\n")
	f.write("cd ./bilevel_solver\n")
	f.write("./compile.sh\n")
	f.write("cd ..\n")
	f.write("cd ./mod_bilevel_solver\n")
	f.write("./compile.sh\n")
	f.write("cd ..\n")
	for z in [4,7,10,13]:
		f.write("echo \"STARTING: size {} networks\"\n".format(z))
		for i in range(30):
			f.write("python network_generator.py {} {} 1\n".format(z,z+3))
			f.write("python bounds_solver.py 1.json test_out1.json\n")
			f.write("python mod_bounds_solver.py 1.json test_out2.json\n")
			f.write("python solver.py test_out1.json out1.json\n")
			f.write("python solver.py test_out2.json out2.json\n")
		f.write("mkdir {}_{}\n".format(z,z+3))
		f.write("mv out1.json ./{}_{}/out1.json\n".format(z,z+3))
		f.write("mv out2.json ./{}_{}/out2.json\n".format(z,z+3))
		f.write("python convert_to_graph.py {}_{}/out1.json {}_{}/out2.json graph{}_{}.txt\n\n".format(z,z+3,z,z+3,z,z+3))
	f.write("echo \"---ALL DONE---\"\n".format(z))
print "done"
