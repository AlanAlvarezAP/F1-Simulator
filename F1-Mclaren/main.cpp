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
Animator* anim=nullptr;
Car* carro=nullptr;
Circuit* circuit=nullptr;
bool Target_free=false;
float dt=0.0f,lastX=0.0f,lastY=0.0f;

Camera_Status camMode = TARGETING;
int currentSceneIndex = 5;


//------------- SECCION DE TESTS ---------------//




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

// 1. Posiciones (X, Y, Z) - Cambiar el 5 por 8
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

// 2. Coordenadas de Textura (U, V) - Cambiar el 5 por 8
glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

// 3. Normales (Nx, Ny, Nz) -> ESTE BLOQUE ES TOTALMENTE NUEVO
glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,mundito->all_EBOs.size()*sizeof(unsigned int),mundito->all_EBOs.data(),GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
glEnableVertexAttribArray(2);
	
	
	glBindVertexArray(0);
	
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

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	mundito = new World();
	anim = Builder::BuildAnimator();
	
	//cube = Builder::BuildCubeScene(mundito,{0.0f,0.0f,0.0f});
	//sphere=Builder::BuildSphereScene(mundito,0.5f);
	carro=Builder::BuildCarScene(mundito);
	circuit=Builder::BuildCircuitScene(mundito);
	
	// Ojo aca cambiar escena inicial :D
	mundito->activeSceneNode= carro;
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
	
	
	float dist = 4.0f;
	float altura = 2.0f;

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

		Camera camFront(
			{target.x, target.y + altura, target.z + dist},
			target,
			{0,1,0},
			TARGETING
		);

		glViewport(0, halfH, halfW, halfH);

		mundito->DrawShape(
			camFront.GetLookAt(),
			camFront.GetProjection(
				(float)halfW,
				(float)halfH,
				0.1f,
				100.0f
			)
		);

		// CAMARA TRASERA

		Camera camBack(
			{target.x, target.y + altura, target.z - dist},
			target,
			{0,1,0},
			TARGETING
		);

		glViewport(
			halfW,
			halfH,
			halfW,
			halfH
		);

		mundito->DrawShape(
			camBack.GetLookAt(),
			camBack.GetProjection(
				(float)halfW,
				(float)halfH,
				0.1f,
				100.0f
			)
		);

		// CAMARA IZQUIERDA

		Camera camLeft(
			{target.x - dist, target.y + altura, target.z},
			target,
			{0,1,0},
			TARGETING
		);

		glViewport(
			0,
			0,
			halfW,
			halfH
		);

		mundito->DrawShape(
			camLeft.GetLookAt(),
			camLeft.GetProjection(
				(float)halfW,
				(float)halfH,
				0.1f,
				100.0f
			)
		);	

		// CAMARA DERECHA

		Camera camRight(
			{target.x + dist, target.y + altura, target.z},
			target,
			{0,1,0},
			TARGETING
		);

		glViewport(
			halfW,
			0,
			halfW,
			halfH
		);

		mundito->DrawShape(
			camRight.GetLookAt(),
			camRight.GetProjection(
				(float)halfW,
				(float)halfH,
				0.1f,
				100.0f
			)
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
