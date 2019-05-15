cd ./k_generator
./compile.sh
cd ..
echo "*** 90 bus networks ***"
python network_generator.py 90 108 1
python lmp.py 1.json lmp_out.json
python bounds_solver.py 1.json gnk_out.json 8 0.01 0.2 10
