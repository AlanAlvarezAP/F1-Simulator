#pragma once
#include "Shape.h"

#define MAX_SIZE 150000

class Parts_Circuit: public ShapeNode{
public:
	std::string mtl;
public:
	Parts_Circuit(World* world,std::string nombre,std::string name_mtl,RGB colors);
	void Generate() override;
	void DrawGeometry(const Matrix& parent) override;
};


class Circuit: public ShapeNode{
public:
	std::vector<Mesh> meshes;
	std::string mtlib;
	const char* file;
public:
	Circuit(World* world, const char* file_path);
	void Generate() override;
	void printMenu() override;
	
};