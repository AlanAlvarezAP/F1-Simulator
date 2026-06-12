#pragma once
#include "Shape.h"

#define MAX_SIZE 150000


class FaceVertex{
public:
	class FaceIndex{
	public:
		int v,vt,vn;
	public:
		bool operator==(const FaceIndex& other) const{
			return (v==other.v) && (vt==other.vt) && (vn==other.vn);
		}
	};
public:
    FaceIndex first;
    FaceIndex second;
    FaceIndex third;
	
};

class FaceIndexHash {
public:
    size_t operator()(const FaceVertex::FaceIndex& f) const {
        size_t h1 = std::hash<int>()(f.v);
        size_t h2 = std::hash<int>()(f.vt);
        size_t h3 = std::hash<int>()(f.vn);

        return h1^(h2<<1)^(h3<<2);
    }
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
	std::string Optimize_Parser(const std::string &line);
	FaceVertex Optimize_Parser_Face(const std::string &line);
	Point Optimize_Parser_Numeric(const std::string &line,const int offset);
	std::vector<unsigned int> Update_EBos_Vertex(std::vector<float>& send,std::vector<float> &vertices,std::vector<float> &UVs,std::unordered_map<FaceVertex::FaceIndex,unsigned int,FaceIndexHash>& check_repeat,const std::vector<FaceVertex>& faces);
	void Generate() override;
	void printMenu() override;
	
};