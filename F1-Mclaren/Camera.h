#pragma once

#include "Matrix.h"
#include "Animator.h"

const float YAW= -270.0f;
const float PITCH= 0.0f;
const float SPEED = 100.0f;
const float SENSITIVITY= 0.001f;
const float ZOOM= 45.0f;

#define TRANSFORM PI/180

enum Camera_Mov{
	FORWARD,
	BACKWARD,
	RIGHT,
	LEFT
};

enum Camera_Status{
	FREE,
	TARGETING
};

class Matrix;


class Camera: public Animatable{
public:
	Point Position;
	Point Front;
	Point OwnUp;
	Point Left;
	Point WorldUp;
	Point Target;
	
	float angle_yaw,angle_pitch;
	float mov_speed,mouse_sensi,fov;
	
public:
	Camera(const Point &pos = {0.0f,0.0f,2.0f},const Point& target ={0.0f,0.0f,0.0f},const Point& wUP={0.0f,1.0f,0.0f},Camera_Status stat=TARGETING);
	
	float Normalize(const Point& val1) const;
	float dot(const Point& a, const Point& b);
	Point CrossProduct(const Point& val1,const Point& val2);
	Matrix GetLookAt();
	Matrix GetProjection(float width, float height, float nearP, float farP);
	void ProcessKeyboard(Camera_Mov dir,float dt);
	void ProcessMouse(float xoff,float yoff,bool firstMov=true);
	void ProcessScroll(float yoff);
	void UpdateCam(Camera_Status stat,const Point& new_target={0.0f,0.0f,0.0f});
	void DebugOrthonormalTest();
	void ApplyAnimation(char type,char axis,char local_world,float step) override;
};