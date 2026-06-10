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
		
        float w_radius = 0.15f;
        float w_height = 0.1f;
        float offsetX = sx * 0.75f;
        float offsetY = -sy;
        float offsetZ = sz + (w_height / 2.0f);

        Tire* r_di = new Tire(mundito, {center.x + offsetX, center.y + offsetY, center.z + offsetZ}, w_radius, w_height, 16, ColorTable[NEGRO], 0, "Tire DI");
        Tire* r_dd = new Tire(mundito, {center.x + offsetX, center.y + offsetY, center.z - offsetZ}, w_radius, w_height, 16, ColorTable[NEGRO], 0, "Tire DD");
        Tire* r_ti = new Tire(mundito, {center.x - offsetX, center.y + offsetY, center.z + offsetZ}, w_radius, w_height, 16, ColorTable[NEGRO], 0, "Tire TI");
        Tire* r_td = new Tire(mundito, {center.x - offsetX, center.y + offsetY, center.z - offsetZ}, w_radius, w_height, 16, ColorTable[NEGRO], 0, "Tire TD");

        float sw_offsetX = sx * 0.6f;
        float sw_offsetY = sy + 0.05f;
        float sw_radius = 0.1f;
        float sw_height = 0.05f;

        Tire* s_w = new Tire(mundito, {center.x - sw_offsetX, center.y + sw_offsetY, center.z}, sw_radius, sw_height, 10, ColorTable[CAFE], 0, "Steering wheel");

        r_di->Generate();
        r_dd->Generate();
        r_ti->Generate();
        r_td->Generate();
        s_w->Generate();

        //s_w->Mat.UpdateView('f',90.0f,'z','L');

        chassis->AddChildren(r_di);
        chassis->AddChildren(r_dd);
        chassis->AddChildren(r_ti);
        chassis->AddChildren(r_td);
        chassis->AddChildren(s_w);

        std::cout << "GEN CAR" << std::endl;

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