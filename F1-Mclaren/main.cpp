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
Camera* cam=nullptr;
Animator* anim=nullptr;
Car* carro=nullptr;
Car* carro2=nullptr;
Circuit* circuit=nullptr;
bool Target_free=false;
float dt=0.0f,lastX=0.0f,lastY=0.0f;

Camera_Status camMode = TARGETING;
int currentSceneIndex = 5;

std::vector<Point> routePoints;

//------------- ANIMATION ---------------//

void loadCarIntructions(Car* carro) {
    while(!carro->animScript.empty())
		carro->animScript.pop();
    carro->currentStepTimer = 0.0f;
    carro->isScriptPlaying = true;

    carro->addScriptStep(true, false, 4.75f, 25.0f, 0.0f);
    carro->addScriptStep(true, false, 1.15f, 15.0f, -12.0f);
    carro->addScriptStep(true, false, 2.2f, 30.0f, 0.0f);
    carro->addScriptStep(true, false, 2.75f, 25.0f, 4.75f);
    carro->addScriptStep(true, false, 1.4f, 30.0f, 0.0f);
    carro->addScriptStep(true, false, 0.6f, 20.0f, 15.0f);
    carro->addScriptStep(true, false, 0.5f, 30.0f, 0.0f);
    carro->addScriptStep(true, false, 1.0f, 20.0f, -20.0f);
    carro->addScriptStep(true, false, 1.5f, 30.0f, 0.0f);
    carro->addScriptStep(true, false, 1.15f, 25.0f, -8.0f);
    carro->addScriptStep(true, false, 1.5f, 30.0f, 0.0f);
    carro->addScriptStep(true, false, 0.3f, 25.0f, -15.0f);
    carro->addScriptStep(true, false, 1.6f, 30.0f, 0.0f);
    carro->addScriptStep(true, false, 0.5f, 30.0f, -5.0f);
    carro->addScriptStep(true, false, 0.15f, 30.0f, 0.0f);
    carro->addScriptStep(true, false, 0.5f, 30.0f, 5.0f);
    carro->addScriptStep(true, false, 1.5f, 30.0f, 0.0f);
    carro->addScriptStep(true, false, 0.45f, 30.0f, 15.0f);
    carro->addScriptStep(true, false, 0.1f, 30.0f, 5.0f);
    carro->addScriptStep(true, false, 2.0f, 30.0f, 0.0f);
    carro->addScriptStep(true, false, 1.0f, 15.0f, -20.0f);
    carro->addScriptStep(true, false, 0.2f, 30.0f, 0.0f);
    carro->addScriptStep(true, false, 0.5f, 30.0f, -8.0f);
    carro->addScriptStep(true, false, 4.55f, 30.0f, 0.0f);
    carro->addScriptStep(true, false, 0.45f, 20.0f, 20.0f);
    carro->addScriptStep(true, false, 0.1f, 30.0f, 10.0f);
    carro->addScriptStep(true, false, 1.5f, 30.0f, 0.0f);
    carro->addScriptStep(true, false, 0.3f, 30.0f, 12.0f);
    carro->addScriptStep(true, false, 0.2f, 30.0f, 0.0f);
    carro->addScriptStep(true, false, 0.3f, 30.0f, -12.0f);
    carro->addScriptStep(true, false, 0.8f, 30.0f, 0.0f);
    carro->addScriptStep(true, false, 0.45f, 25.0f, -20.0f);
    carro->addScriptStep(true, false, 0.9f, 30.0f, 0.0f);
    carro->addScriptStep(true, false, 0.45f, 25.0f, -20.0f);
    carro->addScriptStep(true, false, 0.1f, 30.0f, 5.0f);
    carro->addScriptStep(true, false, 3.5f, 30.0f, 0.0f);
    carro->addScriptStep(false, true, 1.0f, 0.0f, 0.0f);
}

//------------- SECCION DE TESTS ---------------//
void alinear(){
	Animation_Step* movCarX2= new Animation_Step(carro2,0.001f,'a',12.0f,'x','W');
	Animation_Step* movCarY2= new Animation_Step(carro2,0.001f,'a',-1.0f,'y','W');
	Animation_Step* movCarZ2= new Animation_Step(carro2,0.001f,'a',-10.0f,'z','W');
	Animation_Step* movCarX= new Animation_Step(carro,0.001f,'a',-3.0f,'x','W');
	Animation_Step* movCarY= new Animation_Step(carro,0.001f,'a',-1.0f,'y','W');
	Animation_Step* movCamX= new Animation_Step(cam,0.001f,'a',-3.0f,'x','W');
	anim->Add_Animations(std::vector<Animation_Step*>{
		movCarX,movCarY,movCamX,movCarX2,movCarY2,movCarZ2
	},'N');
}

void tests_anim(){
	// CUIDADO CON DOBLE RELEASE
	// TEST CAMARA
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
	anim->Add_Animations(std::vector<Animation_Step*>{rotateCam2}, 'S');
	anim->Add_Animations(std::vector<Animation_Step*>{movCam1,rotateCam3}, 'S');
}

void orbit(){
	// rotar la cámara 90 grados en yaw en 4 segundos
	Animation_Step* rotateCam = new Animation_Step(cam, 4.0f, 'o', 360.0f, 'y', 'W');
	Animation_Step* rotateCam1 = new Animation_Step(cam, 4.0f, 'o', 360.0f, 'y', 'W');

	anim->Add_Animations(std::vector<Animation_Step*>{rotateCam}, 'S');
	anim->Add_Animations(std::vector<Animation_Step*>{rotateCam1}, 'S');
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

	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,mundito->all_EBOs.size()*sizeof(unsigned int),mundito->all_EBOs.data(),GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	
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
    if (camMode == FREE)
        cam->ProcessMouse(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    cam->ProcessScroll((float)yoffset);
}

void key_callback(GLFWwindow* window,int key,int scan,int action,int mods){
	if(action != GLFW_PRESS)
		return;
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
		case GLFW_KEY_M:{
		    Point pos = carro->GetWorldPosition();
		    routePoints.push_back(pos);
		    std::cout << "Saved Point: (" << pos.x << ", " << pos.y << ", " << pos.z << " )\n";
			break;
		}
		case GLFW_KEY_1:{
			if (!carro->isScriptPlaying)
                loadCarIntructions(carro);
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
	if(mundito && mundito->activeSceneNode)
		mundito->activeSceneNode->handleKey(key,mods,CURRENT_AXIS);
}

int main(){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window=glfwCreateWindow(1200,800,"F1 Simulator - 4 Cameras",NULL,NULL);
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
	// calling builder functoins
	carro = Builder::BuildCarScene(mundito);
	carro2 = Builder::BuildCarScene(mundito);
	circuit = Builder::BuildCircuitScene(mundito);
	circuit->Mat.UpdateView('g', 1.4f, 1.0f, 1.4f, 'z', 'L');
	// Ojo aca cambiar escena inicial :D
	mundito->activeSceneNode = carro;
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
	
	float dist = 3.0f; // 0.8
	float altura = 5.0f; // 0.35

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

		bool inputForward = (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS);
		bool inputBackward = (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS);
		bool inputBrake = (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS);
		bool inputLeft = (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS);
		bool inputRight = (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS);

		bool inputForward2 = (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS);
		bool inputBackward2 = (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS);
		bool inputBrake2 = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
		bool inputLeft2 = (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS);
		bool inputRight2 = (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS);

		if (carro->isScriptPlaying) {
            carro->updateScriptAnimation(dt, inputForward, inputBrake);
            inputBackward = false;
        }
		carro->updatePhysics(dt, inputForward, inputBackward, inputBrake, inputLeft, inputRight);
		carro2->updatePhysics(dt, inputForward2, inputBackward2, inputBrake2, inputLeft2, inputRight2);
		
		glBindVertexArray(VAO);
		glPointSize(4.0f);
		glLineWidth(4.0f);
		
		glfwPollEvents();
		anim->Execute_animations(dt);
		
		// Para seguir
		if (camMode == TARGETING) {
			//cam->UpdateCam(TARGETING, mundito->activeSceneNode->GetWorldPosition());
			Point carPos = mundito->activeSceneNode->GetWorldPosition();
   			float dirZ_X = carro->Mat.matrix[8];
   			float dirZ_Y = carro->Mat.matrix[9];
   			float dirZ_Z = carro->Mat.matrix[10];
			float offsetAdelante = 0.2f;
    		float alturaPiloto = 0.2f;
   			float camPosX = carPos.x - (dirZ_X * dist);
   			float camPosY = carPos.y + altura;
   			float camPosZ = carPos.z - (dirZ_Z * dist);
   			cam->Position.x = camPosX;
   			cam->Position.y = camPosY;
   			cam->Position.z = camPosZ;
			/*
			float distanciaMirada = 10.0f; 
			float inclinacionMirada = 0.0f;
    		Point puntoDeFuga;
    		puntoDeFuga.x = camPosX + (dirZ_X * distanciaMirada);
    		puntoDeFuga.y = camPosY + inclinacionMirada;
    		puntoDeFuga.z = camPosZ + (dirZ_Z * distanciaMirada);
   			cam->UpdateCam(TARGETING, puntoDeFuga);
			*/
   			cam->UpdateCam(TARGETING, carPos);
		}
		else
			cam->UpdateCam(FREE);
        mundito->DrawShape(cam->GetLookAt(),cam->GetProjection(800.0f, 800.0f, 0.1f, 1000.0f));
		
		glBindVertexArray(0);
        glfwSwapBuffers(window);
    }
	for(auto p : routePoints)
		std::cout << "(" << p.x << ", " << p.y << ", " << p.z << " )\n";
	delete mundito;
	return 0;
}