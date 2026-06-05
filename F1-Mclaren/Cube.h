#pragma once

#include "Shape.h"

class Face {
public:
    Point a,b,c,d;

    RGB triColor;
    RGB lineColor;
    RGB pointColor;

    unsigned int triangle_start;
    unsigned int lines_Start;
    unsigned int points_Start;

    Face();
};


class Cube: public ShapeNode{
public:
	Point center;
	unsigned int sector_Start,lines_Start,points_Start;
	Face faces[6];
	std::vector<RGB> faceColors;
	int type;
public:
	Cube(World* world, const Point& cent, std::vector<RGB> colors={ColorTable[LIMA],ColorTable[VERDE],ColorTable[BLANCO],ColorTable[MORADO],ColorTable[NARANJA],ColorTable[DORADO]}, int tp=0, std::string name="Cube");
	void Generate() override;
	void DrawGeometry(const Matrix& parent) override;
	void printMenu() override;
	void handleKey(int key, int mods,char CURRENT_AXIS) override;
};