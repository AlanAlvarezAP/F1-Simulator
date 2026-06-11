#pragma once

#include "Shape.h"
#include "Tire.h"

#define W_RADIUS 0.15f
#define W_HEIGHT 0.1f

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
	Tire* t_fl;
    Tire* t_fr;
    Tire* t_rl;
    Tire* t_rr;
    Tire* s_w;
	float steeringAngle = 0.0f; // steering wheel angle
    float rollAngle = 0.0f; // cumulated tire angle
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
	void moveForward(float step);
    void moveBackward(float step);
	void steerLeft(float degrees);
    void steerRight(float degrees);
    void updateWheels();
};