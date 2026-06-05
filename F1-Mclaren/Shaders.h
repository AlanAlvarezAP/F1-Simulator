#pragma once

#include "config.h"

class Matrix;

class Shaders{
private:
	unsigned int program_id,vertex_shad_id,fragment_shad_id;
public:
	Shaders();
	void use();
	unsigned int GetProgram();
	void SetColor(const float &r,const float &g,const float &b);
	void SetMatrix(const Matrix& mat);
	void SetView(const Matrix& mat);
	void SetProjection(const Matrix& mat);
};
