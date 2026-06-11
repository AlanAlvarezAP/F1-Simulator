#pragma once
#include "Shape.h"

#define MAX_SIZE 150000


class FaceVertex{
public:
	class FaceIndex{
	public:
		int v,vt,vn;
	};
public:
    FaceIndex first;
    FaceIndex second;
    FaceIndex third;
};

class Mesh{
public:
	std::string name;
	std::string mtl;
	std::vector<FaceVertex> faces;
	unsigned int ebo_start;
};

class Llanta: public ShapeNode{
public:
	Llanta(World* world,std::string nombre);
	void Generate() override;
	void DrawGeometry(const Matrix& parent) override;
};

class Timon: public ShapeNode{
public:
	Timon(World* world,std::string nombre);
	void Generate() override;
	void DrawGeometry(const Matrix& parent) override;
};

class Chassis: public ShapeNode{
public:
	Chassis(World* world,std::string nombre);
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
	std::string Optimize_Parser(const std::string &line);
	FaceVertex Optimize_Parser_Face(const std::string &line);
	Point Optimize_Parser_Numeric(const std::string &line,const int offset);
	std::vector<unsigned int> Obtained_EBos(const std::vector<FaceVertex>& faces,unsigned int base);
	void Generate() override;
	void printMenu() override;
	
};