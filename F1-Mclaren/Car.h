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

class CarBodyGroup : public ShapeNode {
public:
    float& bodyPitch;
    float& bodyRoll;

    CarBodyGroup(World* w, float& p, float& r);
	void Generate() override;
};

class Car: public ShapeNode{
public:
	std::vector<Mesh> meshes;
	std::string mtlib;
	const char* file;
	// for parts
	CarBodyGroup* chassisGroup;
	ShapeNode* timonGroup;
	ShapeNode* wg_fl;
	ShapeNode* wg_fr;
	ShapeNode* wg_rl;
	ShapeNode* wg_rr;
	Point wc_fl, wc_fr, wc_rl, wc_rr;
	// for movement
	float steeringAngle = 0.0f;
	float rollAngle = 0.0f;
	float bodyPitch = 0.0f;
	float bodyRoll = 0.0f;
	float currentSpeed = 0.0f;
	float previousSpeed = 0.0f;
	float steerSpeed = 45.0f;
	float brakeSpeed = 30.0f;
	float maxSpeed = 50.0f;
	float maxReverse = -25.0f;
	float acceleration = 10.0f;
	float deceleration = 2.5f;
	float currentPitch = 0.0f;
	float previousPitch = 0.0f;
	float pitchSpring = 15.0f; // spring rigid
	float visualBalancingFactor = 0.05f; // balacing force
	float limitPitch = 5.0f; // angle
	float currentRoll = 0.0f;
	float previousRoll = 0.0f;
	float rollSpring = 15.0f;
	float visualRollingFactor = -0.05f; // rolling force
	float limitRoll = 5.0f;
public:
	Car(World* world, const char* file_path);
	void Generate() override;
	void printMenu() override;
	void handleKey(int key, int mods, char CURRENT_AXIS) override;
	// for movement
	void moveForward(float step);
	void moveBackward(float step);
	void steerLeft(float degrees);
	void steerRight(float degrees);
	void updateWheels();
	void updatePhysics(float dt, bool inForward, bool inBackward,
		bool inBrake, bool inLeft, bool inRight);
};