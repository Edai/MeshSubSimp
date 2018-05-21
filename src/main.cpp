#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <cassert>
#include <ctime>
#include "Mesh.hpp"
#include "SimpleMesh.h"

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		std::cout << "USAGE: [.exe] [in.obj] [iteration]" << std::endl;
		return -1;
	}
	int iteration = atoi(argv[2]);
	Mesh *mesh = new Mesh;
	mesh->Load(argv[1]);
	std::string fileName = argv[1];
	clock_t start = clock();
	for (unsigned i = 0; i < iteration; ++i)
	{
		std::cout << "Iteration " << i << std::endl;
		mesh->LoopSubdivisionOneStep();
	}
	clock_t end = clock();
	std::cout << "Time consumed: " << (double)(end - start) / (double)CLOCKS_PER_SEC << std::endl;

	mesh->Save((fileName.substr(0, fileName.rfind(".")) + ".loop.obj").c_str());

	return 0;
}