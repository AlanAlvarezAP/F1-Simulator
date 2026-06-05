#pragma once

#include "Cube.h"
#include "Sphere.h"
#include "Colors.h"
#include "Camera.h"

class Builder{
public:
	static Cube* BuildCubeScene(World* &mundito, const Point& center = {0.0f, 0.0f, 0.0f}) {

        Cube* cube = new Cube(mundito, center,{ColorTable[NEGRO],ColorTable[VERDE],ColorTable[BLANCO],ColorTable[NEGRO],ColorTable[NARANJA],ColorTable[NEGRO]});
        mundito->root->AddChildren(cube);
        cube->Generate();

        std::cout << "GEN CUBE" << std::endl;

        return cube;
    }
	
	static Sphere* BuildSphereScene(World* &mundito, float radius) {

        Point centro = {0.0f, 0.0f, 0.0f};
        Sphere* s = new Sphere(mundito, centro, radius, 18, 18);
        mundito->root->AddChildren(s);
        s->Generate();

		std::cout << "GEN SPHERE" << std::endl;
        return s;
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