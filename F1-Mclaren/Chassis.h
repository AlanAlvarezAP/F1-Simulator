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


class Chassis: public ShapeNode{
public:
	Point center;
	float sx, sy, sz;
	unsigned int sector_Start,lines_Start,points_Start;
	Face faces[6];
	std::vector<RGB> faceColors;
	int type;
public:
	Chassis(
		World* world, const Point& cent, float sx, float sy, float sz,
		std::vector<RGB> colors={ColorTable[ROJO],ColorTable[ROJO],ColorTable[ROJO],ColorTable[ROJO],ColorTable[VERDE],ColorTable[AZUL]},
		int tp=0, std::string name="Chassis"
	);
	void Generate() override;
	void DrawGeometry(const Matrix& parent) override;
	void printMenu() override;
	void handleKey(int key, int mods,char CURRENT_AXIS) override;
};