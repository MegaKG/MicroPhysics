all:
	g++ TestPhysics.cpp -lm -g -o MicroPhysicsTest -O3 -lOpenCL

graph:
	g++ GraphicalTest.cpp -lm -g -o GraphTest -O3 -lgraph -lOpenCL

pygraph:
	g++ PyGraphTest.cpp -lm -g -o PyGraph -O3 -lOpenCL

gas:
	g++ GasTest.cpp -lm -g -o GasGraph -O3 -lOpenCL
