import os

prepends = ["stat","simple","bound"]
output_filename = "collated_output"

for s in prepends:
	os.system("python data_averager.py {}.json {}".format(s," ".join(["{}{}.json".format(s,i) for i in range(8)])))

os.system("echo \"\" > {}.csv".format(output_filename))
for s in prepends:
	os.system("printf \"{}\n\" >> {}.csv".format(s,output_filename))
	os.system("python viewer.py {}.json >> {}.csv".format(s,output_filename))
