#pragma once

#include "config.h"
#include "Shaders.h"
#include "Matrix.h"
#include "Colors.h"
#include "Animator.h"

class ShapeNode;

// -------------- LISTA DE CLASES PARA PARSEAR Y MESH LOGIC -------------

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
// -------------- FIN PARSER Y MESH LOGIC -------------

struct Material {
    std::string name;
    float Ka[3] = {0.2f, 0.2f, 0.2f}; // Color Ambiente por defecto
    float Kd[3] = {0.8f, 0.8f, 0.8f}; // Color Difuso por defecto
    float Ks[3] = {1.0f, 1.0f, 1.0f}; // Color Especular por defecto
    float Ns = 32.0f;                 // Brillo por defecto
};


class Parser{
public:
	std::string Optimize_Parser(const std::string &line);
	FaceVertex Optimize_Parser_Face(const std::string &line);
	Point Optimize_Parser_Numeric(const std::string &line,const int offset);
	std::vector<unsigned int> Update_EBos_Vertex(std::vector<float>& send,std::vector<float> &vertices,std::vector<float> &UVs,std::vector<float> &Normals, std::unordered_map<FaceVertex::FaceIndex,unsigned int,FaceIndexHash>& check_repeat,const std::vector<FaceVertex>& faces,unsigned int& base);	
	std::unordered_map<std::string, Material> ParseMTL(const std::string& path);
};



class World{
public:
	std::vector<float> all_vertices;
	std::vector<unsigned int> all_EBOs;
	Matrix Mat_global;
	Shaders Shader_global;
	ShapeNode* root,*activeSceneNode;
	int globalColorCounter;
	
	Point lightPos;
public:
	World();
	~World();
	void DrawShape(const Matrix& view,const Matrix& projection, const Point& camPos);
	std::vector<unsigned int> Add_Batch(std::vector<float>& vectors,std::vector<unsigned int>& indices,unsigned int &offset);
	void print(ShapeNode* rot,int offset=0);
};


class ShapeNode: public Animatable{
public:
	Matrix Mat;
	std::vector<unsigned int> EBOs_range;
	Shaders Shader;
	std::vector<ShapeNode*> children;
	Material material;
	unsigned int primitive,offset;
	RGB color;
	ShapeNode* parent;
	World* world;
	Parser parser;
	std::string name;
	bool IsDrawable;
	int selected_part;
	Matrix worldMatrix;
	bool dirty = true;
public:
	ShapeNode(World* world,unsigned int prim,const std::string &nam);
	virtual ~ShapeNode();
	void AddChildren(ShapeNode* son);
	void ModifiedShaderTransform(const char &tpe,const float &first_val,float second_val,char axis);
	void ModifiedShaderColor(const float &r,const float &g,const float &b);
	void DrawShape(const Matrix& parent, const Matrix& view, const Matrix& projection, const Point& camPos);
	void SelectNextChild();
	Matrix GetWorldMatrix();
	Point GetWorldPosition();
	virtual void handleKey(int key, int mods,char CURRENT_AXIS);
	virtual void printMenu() {}
	virtual void Generate() {}
	virtual void DrawGeometry(const Matrix& parent){}
	void ApplyAnimation(char type,char axis,char local_world,float step) override;
};




// -------------- LISTA DE POSIBLES FIGURAS POLIMORFICAS -------------
// Circulo
class Circle: public ShapeNode{
public:
	float radius;
	int number_segments;
	Point center;
public:
	Circle(World* world,const Point &cent,const float &rad,const int &num_seg);
	void Generate() override;
	void DrawGeometry(const Matrix& parent) override;
};



// PIZZA

