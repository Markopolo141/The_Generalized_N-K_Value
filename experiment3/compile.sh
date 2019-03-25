cd ./k_generator
./compile.sh
cd ..
python bounds_solver.py 0.json test_out.json 8 0.000000001 30
