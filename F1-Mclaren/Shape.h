#pragma once

#include "config.h"
#include "Shaders.h"
#include "Matrix.h"
#include "Colors.h"
#include "Animator.h"

class ShapeNode;

class World{
public:
	std::vector<float> all_vertices;
	std::vector<unsigned int> all_EBOs;
	Matrix Mat_global;
	Shaders Shader_global;
	ShapeNode* root,*activeSceneNode;
	int globalColorCounter;
public:
	World();
	~World();
	void DrawShape(const Matrix& view,const Matrix& projection);
	std::vector<unsigned int> Add_Batch(std::vector<float>& vectors,std::vector<unsigned int>& indices,unsigned int &offset);
	void print(ShapeNode* rot,int offset=0);
};


class ShapeNode: public Animatable{
public:
	Matrix Mat;
	std::vector<unsigned int> EBOs_range;
	Shaders Shader;
	std::vector<ShapeNode*> children;
	unsigned int primitive,offset;
	RGB color;
	ShapeNode* parent;
	World* world;
	std::string name;
	bool IsDrawable;
	int selected_part;
public:
	ShapeNode(World* world,unsigned int prim,const std::string &nam);
	virtual ~ShapeNode();
	void AddChildren(ShapeNode* son);
	void ModifiedShaderTransform(const char &tpe,const float &first_val,float second_val,char axis);
	void ModifiedShaderColor(const float &r,const float &g,const float &b);
	void DrawShape(const Matrix& parent,const Matrix& view,const Matrix& projection);
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

