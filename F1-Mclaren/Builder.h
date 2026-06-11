#pragma once

#include "Chassis.h"
#include "Tire.h"
#include "Colors.h"
#include "Camera.h"

class Builder{
public:

	static Chassis* BuildChassisScene(World* &mundito, const Point& center = {0.0f, 0.0f, 0.0f}, const float sx = 0.5f, const float sy = 0.1f, const float sz = 0.2f) {

        Chassis* chassis = new Chassis(mundito, center, sx, sy, sz);
        mundito->root->AddChildren(chassis);
        chassis->Generate();
		
        std::cout << "GEN TEST CAR" << std::endl;

        return chassis;
    }
	
	static Camera* BuildCamera(){
		Camera* cama=new Camera();
		std::cout << "GEN Camera" << std::endl;
		return cama;
	}
	
	static Animator* BuildAnimator(){
		Animator* anim=new Animator();
		std::cout << "GEN Animator" << std::endl;
		return anim;
	}
};