#pragma once

#include "Car.h"
#include "Colors.h"
#include "Camera.h"
#include "Circuit.h"

class Builder{
public:
	static Car* BuildCarScene(World* &mundito) {
		const char* path = "C:/Users/tanuki/Documents/ucsp/LabCompiler/glfw-master/OwnProjects/F1-Mclaren/models/F1-Mclaren_3.obj";
		//const char* path = "F:/Comp Grafica/glfw-master/OwnProjects/F1-Mclaren/models/F1-Mclaren_3.obj";
        Car* car = new Car(mundito, path);
        mundito->root->AddChildren(car);
        car->Generate();

		std::cout << "GEN F1-Mclaren :DDDDD" << std::endl;
        return car;
    }

	static Circuit* BuildCircuitScene(World* &mundito) {
		const char* path = "C:/Users/tanuki/Documents/ucsp/LabCompiler/glfw-master/OwnProjects/F1-Mclaren/circuit/circuit.obj";
		//const char* path = "F:/Comp Grafica/glfw-master/OwnProjects/F1-Mclaren/circuit/circuit.obj";
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