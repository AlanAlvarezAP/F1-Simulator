#pragma once

#include "Cube.h"
#include "Car.h"
#include "Sphere.h"
#include "Colors.h"
#include "Camera.h"
#include "Circuit.h"

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
	
	static Car* BuildCarScene(World* &mundito) {
		const char* path = "F:/LabCompiler2026_I_CG/glfw-master/OwnProjects/F1-Mclaren-Camaras/models/F1-Mclaren_3.obj";
        Car* car = new Car(mundito, path);
        mundito->root->AddChildren(car);
        car->Generate();

		std::cout << "GEN F1-Mclaren :DDDDD" << std::endl;
        return car;
    }
	static Circuit* BuildCircuitScene(World* &mundito) {
		const char* path = "F:/LabCompiler2026_I_CG/glfw-master/OwnProjects/F1-Mclaren-Camaras/circuit/circuit.obj";
        Circuit* circuit = new Circuit(mundito, path);
        mundito->root->AddChildren(circuit);
        circuit->Generate();

		std::cout << "GEN circuit :DDDDD" << std::endl;
        return circuit;
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