all:
	g++ TestPhysics.cpp -lm -g -o MicroPhysicsTest -O3

graph:
	g++ GraphicalTest.cpp -lm -g -o GraphTest -O3 -lgraph

pygraph:
	g++ PyGraphTest.cpp -lm -g -o PyGraph -O3 

gas:
	g++ GasTest.cpp -lm -g -o GasGraph -O3 