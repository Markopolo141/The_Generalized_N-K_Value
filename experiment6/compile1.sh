cd ./k_generator
./compile.sh
cd ..
echo "*** 90 bus networks ***"
python network_generator.py 60 80 1
python lmp.py 1.json lmp_out.json
python bounds_solver.py 1.json gnk_out.json 8 0.02 0.2 10 newnew
python bounds_solver.py 1.json gnk_out.json 8 0.02 0.2 10 new
python bounds_solver.py 1.json gnk_out.json 8 0.02 0.2 10 old
