#pragma once
#include "Shape.h"

#define MAX_SIZE 150000


class Llanta: public ShapeNode{
public:
	std::string mtl;
public:
	Llanta(World* world,std::string nombre,std::string name_mtl);
	void Generate() override;
	void DrawGeometry(const Matrix& parent) override;
};

class Timon: public ShapeNode{
public:
	std::string mtl;
public:
	Timon(World* world,std::string nombre,std::string name_mtl);
	void Generate() override;
	void DrawGeometry(const Matrix& parent) override;
};

class Chassis: public ShapeNode{
public:
	std::string mtl;
public:
	Chassis(World* world,std::string nombre,std::string name_mtl);
	void Generate() override;
	void DrawGeometry(const Matrix& parent) override;
};


class Car: public ShapeNode{
public:
	std::vector<Mesh> meshes;
	std::string mtlib;
	const char* file;
public:
	Car(World* world, const char* file_path);
	void Generate() override;
	void printMenu() override;
	
};