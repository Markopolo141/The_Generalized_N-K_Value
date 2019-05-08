cd ./k_generator
./compile.sh
cd ..
python network_generator.py 90 100 1
python bounds_solver.py 1.json test_out.json 1 0.01 10
