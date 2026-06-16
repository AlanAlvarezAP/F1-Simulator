/*
	Pasos basicos para OpenGL
	1. Inicializar GLFW y el perfil a usar
	2. Configurar Ventana para hacer contexto esa ventana
	3. Cargar GLAD
	4. Colocar Callbacks
	5. Bucle principal de rendering 
	5.1 Swapear buffers
	5.2 Buscar eventos
*/

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include <iostream>
#include <string>

#include <queue>

#include "Shape.h"
#include "Builder.h"

World* mundito=nullptr;
GLuint VAO,VBO,EBO;
unsigned int NUM_REBANADAS=4,SELECT_REBANDA=0;
char CURRENT_AXIS = 'z';
Cube* cube=nullptr;
Sphere* sphere = nullptr;
Camera* cam=nullptr;
Animator* anim=nullptr;
Car* carro=nullptr;
Circuit* circuit=nullptr;
bool Target_free=false;
float dt=0.0f,lastX=0.0f,lastY=0.0f;

Camera_Status camMode = TARGETING;
int currentSceneIndex = 5;


//------------- SECCION DE TESTS ---------------//
void alinear(){
	Animation_Step* movCarX= new Animation_Step(carro,0.001f,'a',-3.0f,'x','W');
	Animation_Step* movCarY= new Animation_Step(carro,0.001f,'a',-1.0f,'y','W');
	Animation_Step* movCamX= new Animation_Step(cam,0.001f,'a',-3.0f,'x','W');
	anim->Add_Animations(std::vector<Animation_Step*>{movCarX,movCarY,movCamX}, 'N');
}

void tests_anim(){

	// CUIDADO CON DOBLE RELEASE
	// TEST CAMARA
	//Animation_Step* moveRobot = new Animation_Step(robot, 4.0f, 'a', 10.0f, 'x', 'W');

	// rotar la cámara 90 grados en yaw en 4 segundos
	Animation_Step* rotateCam = new Animation_Step(cam, 4.0f, 'o', 360.0f, 'y', 'W');
	Animation_Step* rotateCam1 = new Animation_Step(cam, 4.0f, 'o', 360.0f, 'y', 'W');

	// rotar la cámara 90 grados en yaw en 4 segundos
	Animation_Step* rotateCam2 = new Animation_Step(cam, 4.0f, 'o', 360.0f, 'x', 'W');

	//Pequeño movimiento para ver Z
	Animation_Step* movCam1= new Animation_Step(cam,4.0f,'a',1.0f,'x','W');
	Animation_Step* rotateCam3 = new Animation_Step(cam, 4.0f, 'o', 360.0f, 'z', 'W');

	// zoom suave
	Animation_Step* zoomCam = new Animation_Step(cam, 4.0f, 'g', -40.0f, 'z', 'W');

	anim->Add_Animations(std::vector<Animation_Step*>{rotateCam}, 'S');
	//anim->Add_Animations(std::vector<Animation_Step*>{moveRobot,rotateCam1}, 'S');
	anim->Add_Animations(std::vector<Animation_Step*>{rotateCam2}, 'S');
	anim->Add_Animations(std::vector<Animation_Step*>{movCam1,rotateCam3}, 'S');
}


void orbit(){


	// rotar la cámara 90 grados en yaw en 4 segundos
	Animation_Step* rotateCam = new Animation_Step(cube, 4.0f, 'd', 360.0f, 'y', 'W');

	anim->Add_Animations(std::vector<Animation_Step*>{rotateCam}, 'N');
}

//------------ FIN TESTS xd -------------------//



void general_Menu(){
	std::cout << "===================================" << std::endl;
    std::cout << "|        Bienvenido a             |" << std::endl;
    std::cout << "|     Simulador de Camara         |" << std::endl;
    std::cout << "|                                 |" << std::endl;
    std::cout << "|  1. Empezar animacion (Solo robot)|" << std::endl;
	std::cout << "|  2. Empezar test camara         |" << std::endl;
	std::cout << "|  xyz. Rotar ejes                |" << std::endl;
    std::cout << "|  P. Rotar obj (Respecto mundo)  |" << std::endl;
	std::cout << "|  B. Cambiar modo camara         |" << std::endl;
	std::cout << "|  E. Cambiar modo hijo escena activa|" << std::endl;
	std::cout << "|  Flechas. Mover objeto activo   |" << std::endl;
	std::cout << "|  J. Camara izquierda            |" << std::endl;
	std::cout << "|  I. Camara adelante             |" << std::endl;
	std::cout << "|  K. Camara atras                |" << std::endl;
	std::cout << "|  L. Camara derecha              |" << std::endl;
	std::cout << "|  Q. Orbitar                     |" << std::endl;
    std::cout << "|  ESC/CTRL+C. Salir              |" << std::endl;
    std::cout << "===================================" << std::endl;
}

void framebuffer_size_callback(GLFWwindow* window,int width,int height){
	glViewport(0,0,width,height);
}
void set_Vs(){
	glGenVertexArrays(1,&VAO);
	glGenBuffers(1,&VBO);
	glGenBuffers(1,&EBO);
	
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER,mundito->all_vertices.size()*sizeof(float),mundito->all_vertices.data(),GL_DYNAMIC_DRAW);

glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,mundito->all_EBOs.size()*sizeof(unsigned int),mundito->all_EBOs.data(),GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
glEnableVertexAttribArray(2);
	
	
	glBindVertexArray(0);
	
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

	static bool firstMouse = true;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    if (camMode == FREE) {
        cam->ProcessMouse(xoffset, yoffset);
    }
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    cam->ProcessScroll((float)yoffset);
}

void key_callback(GLFWwindow* window,int key,int scan,int action,int mods){
	
	switch(key){
		case GLFW_KEY_ESCAPE:{
			// std::cout << "ESC presionado saliendo..." << std::endl;
			glfwSetWindowShouldClose(window,GLFW_TRUE);
			break;
		}
		
		case GLFW_KEY_C:{
			if(mods & GLFW_MOD_CONTROL){
				std::cout << "CTRL+C presionado saliendo..." << std::endl;
				glfwSetWindowShouldClose(window,GLFW_TRUE);
			}
			break;
		}
		
		
		case GLFW_KEY_X:{
			CURRENT_AXIS='x';
			std::cout << "Eje actual: X" << std::endl;
			break;
		}
		case GLFW_KEY_Y:{
			CURRENT_AXIS='y';
			std::cout << "Eje actual: Y" << std::endl;
			break;
		}
		case GLFW_KEY_Z:{
			CURRENT_AXIS='z';
			std::cout << "Eje actual: Z" << std::endl;
			break;
		}
		case GLFW_KEY_J:{
			cam->ProcessKeyboard(LEFT, dt);
			break;
		}
		case GLFW_KEY_I:{
			cam->ProcessKeyboard(FORWARD, dt);
			break;
		}
		case GLFW_KEY_K:{
			cam->ProcessKeyboard(BACKWARD, dt);
			break;
		}
		case GLFW_KEY_L:{
			cam->ProcessKeyboard(RIGHT, dt);
			break;
		}
		case GLFW_KEY_P:{
			currentSceneIndex =(currentSceneIndex+1)%mundito->root->children.size();
			mundito->activeSceneNode=mundito->root->children[currentSceneIndex];
			std::cout << "Escena actual: "<< mundito->activeSceneNode->name << " con index " << currentSceneIndex << std::endl;
			break;
		}
		case GLFW_KEY_E:{
            mundito->activeSceneNode->SelectNextChild();
            break;
        }
		case GLFW_KEY_B: {
			if (camMode == FREE){
				// std::cout << "Cambiando a modo TARGETING " << std::endl;
				camMode = TARGETING;
				cam->UpdateCam(TARGETING,mundito->activeSceneNode->GetWorldPosition());
			}
			else{
				// std::cout << "Cambiando a modo FREE " << std::endl;
				camMode = FREE;
				cam->UpdateCam(FREE);
			}
			break;
		}
		case GLFW_KEY_Q: {
			orbit();
			break;
		}
		default:{
			break;
		}
	}
	if(mundito && mundito->activeSceneNode){
		mundito->activeSceneNode->handleKey(key,mods,CURRENT_AXIS);
	}
	
}


int main(){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window=glfwCreateWindow(
		1200,
		800,
		"F1 Simulator - 4 Cameras",
		NULL,
		NULL
	);
	if(!window){
		std::cout << "Windows didn't charge" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	if(!gladLoadGL(glfwGetProcAddress)){
		std::cout << "GLAD failed :( " << std::endl;
		return -1;
	}
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	glfwSetKeyCallback(window,key_callback);
	glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	mundito = new World();
	cam = Builder::BuildCamera();
	anim = Builder::BuildAnimator();
	
	cube = Builder::BuildCubeScene(mundito,{2.0f,3.0f,0.0f});
	//sphere=Builder::BuildSphereScene(mundito,0.5f);
	carro=Builder::BuildCarScene(mundito);
	circuit=Builder::BuildCircuitScene(mundito);
	
	// Ojo aca cambiar escena inicial :D
	mundito->activeSceneNode= carro;
	alinear();
	mundito->activeSceneNode->printMenu();
	//general_Menu();

	set_Vs();
	mundito->print(mundito->root);
	glEnable(GL_DEPTH_TEST);
	// Para WIREFRAME
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	float lastTime=glfwGetTime();
	double fpsTime = 0.0;
	int fpsFrames = 0;
	
	
	float dist = 3.0f;
	float altura = 2.5f;
	

	while(!glfwWindowShouldClose(window)){
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		float NowTime=glfwGetTime();
		dt=NowTime-lastTime;
		lastTime=NowTime;
		
		fpsTime += dt;
		fpsFrames++;

		if (fpsTime >= 1.0) {
			double fps = fpsFrames / fpsTime;

			std::string title = "Cubo OpenGL - FPS: " + std::to_string((int)fps);
			glfwSetWindowTitle(window, title.c_str());

			fpsFrames = 0;
			fpsTime = 0.0;
		}
		
		glBindVertexArray(VAO);
		glPointSize(4.0f);
		glLineWidth(4.0f);
		
		
		glfwPollEvents();
		anim->Execute_animations(dt);
		
		
		// Para seguir

		Point target = mundito->activeSceneNode->GetWorldPosition();

		int windowWidth = 1200;
		int windowHeight = 800;

		int halfW = windowWidth / 2;
		int halfH = windowHeight / 2;

		// CAMARA FRONTAL
		Point posFront = {target.x, target.y + altura, target.z + dist}; 
		Camera camFront(posFront, target, {0,1,0}, TARGETING);
		glViewport(0, halfH, halfW, halfH);
		mundito->DrawShape(
			camFront.GetLookAt(),
			camFront.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f),
			posFront 
		);

		// CAMARA TRASERA
		Point posBack = {target.x, target.y + altura, target.z - dist};
		Camera camBack(posBack, target, {0,1,0}, TARGETING);
		glViewport(halfW, halfH, halfW, halfH);
		mundito->DrawShape(
			camBack.GetLookAt(),
			camBack.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f),
			posBack 
		);

		// CAMARA IZQUIERDA
		Point posLeft = {target.x - dist, target.y + altura, target.z};
		Camera camLeft(posLeft, target, {0,1,0}, TARGETING);
		glViewport(0, 0, halfW, halfH);
		mundito->DrawShape(
			camLeft.GetLookAt(),
			camLeft.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f),
			posLeft 
		);	

		// CAMARA DERECHA
		Point posRight = {target.x + dist, target.y + altura, target.z};
		Camera camRight(posRight, target, {0,1,0}, TARGETING);
		glViewport(halfW, 0, halfW, halfH);
		mundito->DrawShape(
			camRight.GetLookAt(),
			camRight.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f),
			posRight 
		);
		glViewport(0, 0, windowWidth, windowHeight);

		glEnable(GL_SCISSOR_TEST);

		// línea vertical
		glScissor(halfW - 1, 0, 2, windowHeight);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		// línea horizontal
		glScissor(0, halfH - 1, windowWidth, 2);
		glClear(GL_COLOR_BUFFER_BIT);

		glDisable(GL_SCISSOR_TEST);
		
		glBindVertexArray(0);
		
        glfwSwapBuffers(window);
        
    }
	delete mundito;
	return 0;
}