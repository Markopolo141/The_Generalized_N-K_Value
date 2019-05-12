with open("compile.sh","w") as f:
	f.write("cd ./bilevel_solver\n")
	f.write("./compile.sh\n")
	f.write("cd ..\n")
	f.write("cd ./mod_bilevel_solver\n")
	f.write("./compile.sh\n")
	f.write("cd ..\n")
	for i in range(1000):
		f.write("python network_generator.py 13 16 1\n")
		f.write("python bounds_solver.py 1.json test_out1.json\n")
		f.write("python mod_bounds_solver.py 1.json test_out2.json\n")
		f.write("python solver.py test_out.json out1.json\n")
		f.write("python solver.py test_out2.json out2.json\n")
print "done"
