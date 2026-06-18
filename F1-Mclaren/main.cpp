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
char CURRENT_AXIS = 'z';
Cube* cube=nullptr;
Camera* cam=nullptr;
Animator* anim=nullptr;
Car* carro=nullptr;
Car* carro2=nullptr;
Circuit* circuit=nullptr;
bool Target_free=false;
float dt=0.0f,lastX=0.0f,lastY=0.0f;

Camera_Status camMode = TARGETING;
int currentSceneIndex = 5;

bool modoCochesDivididos = false;
bool modoPiloto = false; 

std::vector<Point> routePoints;

// car mov script
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

class CamDirectorStep {
public:
    int mode;
    float duration;
    bool triggerAnim;
};

// cam script
std::queue<CamDirectorStep> directorScript;
float directorTimer = 0.0f;
bool isDirectorPlaying = false;
int currentCameraMode = 0;

void loadDirectorInstructions() {
    while(!directorScript.empty())
		directorScript.pop();
    directorTimer = 0.0f;
    isDirectorPlaying = true;

    directorScript.push({0, 5.0f, false}); 
    directorScript.push({1, 7.0f, false}); 
    directorScript.push({2, 10.0f, false}); 
	directorScript.push({3, 2.0f, false});
    directorScript.push({4, 2.0f, false});
    directorScript.push({5, 2.0f, false});
    directorScript.push({2, 7.5f, false}); 
    directorScript.push({0, 8.0f, false}); 
}

class ColorRGB {
public:
    float r, g, b;
};

// t between 0.0 - 1.0
ColorRGB MezclarColor(ColorRGB color1, ColorRGB color2, float t) {
	// t no out limits
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    ColorRGB resultado;
    resultado.r = color1.r + (color2.r - color1.r) * t;
    resultado.g = color1.g + (color2.g - color1.g) * t;
    resultado.b = color1.b + (color2.b - color1.b) * t;
    
    return resultado;
}

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

void alinearCarro() {
	Animation_Step* movCarX= new Animation_Step(carro,0.001f,'a',-3.0f,'x','W');
	Animation_Step* movCarY= new Animation_Step(carro,0.001f,'a',-1.0f,'y','W');
	anim->Add_Animations(std::vector<Animation_Step*>{movCarX,movCarY},'N');
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
	Animation_Step* rotateCam = new Animation_Step(cube, 4.0f, 'd', 360.0f, 'y', 'W');
	anim->Add_Animations(std::vector<Animation_Step*>{rotateCam}, 'N');
}

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
		case GLFW_KEY_R:{
			alinearCarro();
			break;
		}
		case GLFW_KEY_1:{
			if (!carro->isScriptPlaying) {
                loadCarIntructions(carro);
				loadDirectorInstructions();
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
		case GLFW_KEY_U: {
		    modoPiloto = !modoPiloto;
		    if (modoPiloto)
				modoCochesDivididos = false;
		    std::cout << "Modo Piloto: " << (modoPiloto ? "ACTIVADO" : "DESACTIVADO") << std::endl;
		    break;
		}
		case GLFW_KEY_O: {
		    modoCochesDivididos = !modoCochesDivididos;
		    if (modoCochesDivididos)
				modoPiloto = false;
		    std::cout << "Modo Camaras Divididas: " << (modoCochesDivididos ? "ACTIVADO" : "DESACTIVADO") << std::endl;
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
	cube = Builder::BuildCubeScene(mundito,{-8.0f,-100.0f,7.0f});
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
	
	float dist = 1.75f; // 0.8 - 3.0
	float altura = 2.75f; // 0.35 - 5.0
	
	// ORBITA DE LA LUZ
	float anguloLuz = atan2(-7.0f, -8.0f); 
	float radioLuz = sqrt((-100.0f) * (-100.0f) + (-100.0f) * (-100.0f));

	// VELOCIDAD
	float velocidadOrbita = 1.0f;
	bool fondo = true;

	// first person
	float altPiloto = 0.5f;
	float distPiloto = -0.15f;

	while(!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		float NowTime=glfwGetTime();
		dt=NowTime-lastTime;
		lastTime=NowTime;
		
		fpsTime += dt;
		fpsFrames++;

		if (fpsTime >= 1.0) {
			double fps = fpsFrames / fpsTime;

			std::string title = "OpenGL - FPS: " + std::to_string((int)fps);
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
		if (isDirectorPlaying && !directorScript.empty()) {
		    CamDirectorStep currentCamStep = directorScript.front();
		    currentCameraMode = currentCamStep.mode;
		    if (currentCamStep.triggerAnim && directorTimer == 0.0f) {
				Point carPos = carro->GetWorldPosition();
        		cam->Position.x = carPos.x + 20.0f;
        		cam->Position.y = carPos.y + 10.0f;
        		cam->Position.z = carPos.z + 10.0f;
        
        		cam->UpdateCam(TARGETING, carPos); 

        		Animation_Step* movCamX = new Animation_Step(cam, currentCamStep.duration, 'a', -40.0f, 'x', 'W');
        
        		anim->Add_Animations(std::vector<Animation_Step*>{movCamX}, 'N');
		    }
		    directorTimer += dt;
		    if (directorTimer >= currentCamStep.duration) {
		        directorTimer = 0.0f;
		        directorScript.pop();
		        if (directorScript.empty()) {
		            isDirectorPlaying = false;
		            currentCameraMode = 0;
		        }
		    }
		}
		carro->updatePhysics(dt, inputForward, inputBackward, inputBrake, inputLeft, inputRight);
		carro2->updatePhysics(dt, inputForward2, inputBackward2, inputBrake2, inputLeft2, inputRight2);
		
		glBindVertexArray(VAO);
		glPointSize(4.0f);
		glLineWidth(4.0f);
		
		glfwPollEvents();
		anim->Execute_animations(dt);
		
		// Para seguir
		Point cubePos = cube->GetWorldPosition();
		anguloLuz -= velocidadOrbita * dt * 0.4;
		
		float yyy = radioLuz * sin(anguloLuz);
		
		if (yyy < 0)
			fondo = false;
		else
			fondo = true;
		
		mundito->lightPos.x = 7.0f;
		mundito->lightPos.y = radioLuz * sin(anguloLuz);
		mundito->lightPos.z = radioLuz * cos(anguloLuz);
		
		orbit();
		
		ColorRGB colorNoche = {0.04f, 0.06f, 0.13f};
		ColorRGB colorDia   = {0.53f, 0.81f, 0.92f};

		float tuValorIluminacion = yyy; // light hight

		float t = tuValorIluminacion / 100.0f; 

		ColorRGB colorFondo = MezclarColor(colorNoche, colorDia, t);

		glClearColor(colorFondo.r, colorFondo.g, colorFondo.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		int windowWidth, windowHeight;
		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

		int halfW = windowWidth / 2;
		int halfH = windowHeight / 2;

		float aspect = (float)halfW / (float)halfH;

		if (!modoCochesDivididos && !modoPiloto) {
		    // normal mode
		    Point target = mundito->activeSceneNode->GetWorldPosition();

			if (isDirectorPlaying) {
				float dirZ_X = carro->Mat.matrix[8];
				float dirZ_Z = carro->Mat.matrix[10];

				switch (currentCameraMode) {
				    case 0: {
						Point posFront = {target.x, target.y + altura, target.z + dist}; 
		    			Camera camFront(posFront, target, {0,1,0}, TARGETING);
		    			glViewport(0, halfH, halfW, halfH);
		    			mundito->DrawShape(camFront.GetLookAt(), camFront.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f), posFront);
		
		    			Point posBack = {target.x, target.y + altura, target.z - dist};
		    			Camera camBack(posBack, target, {0,1,0}, TARGETING);
		    			glViewport(halfW, halfH, halfW, halfH);
		    			mundito->DrawShape(camBack.GetLookAt(), camBack.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f), posBack);
		
		    			Point posLeft = {target.x - dist, target.y + altura, target.z};
		    			Camera camLeft(posLeft, target, {0,1,0}, TARGETING);
		    			glViewport(0, 0, halfW, halfH);
		    			mundito->DrawShape(camLeft.GetLookAt(), camLeft.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f), posLeft);	
		
		    			Point posRight = {target.x + dist, target.y + altura, target.z};
		    			Camera camRight(posRight, target, {0,1,0}, TARGETING);
		    			glViewport(halfW, 0, halfW, halfH);
		    			mundito->DrawShape(camRight.GetLookAt(), camRight.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f), posRight);
				        break;
				    }
				    case 1: {
				        Point posFront = {target.x + (dirZ_X * dist), target.y + altura, target.z + (dirZ_Z * dist)}; 
				        Camera camFront(posFront, target, {0,1,0}, TARGETING);
				        glViewport(halfW, 0, halfW, halfH);
				        mundito->DrawShape(camFront.GetLookAt(), camFront.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f), posFront);

		    			Point posBack = {target.x, target.y + altura, target.z - dist};
		    			Camera camBack(posBack, target, {0,1,0}, TARGETING);
		    			glViewport(0, 0, halfW, halfH);
		    			mundito->DrawShape(camBack.GetLookAt(), camBack.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f), posBack);
		
		    			Point posLeft = {target.x - dist, target.y + altura, target.z};
		    			Camera camLeft(posLeft, target, {0,1,0}, TARGETING);
		    			glViewport(0, halfH, halfW, halfH);
		    			mundito->DrawShape(camLeft.GetLookAt(), camLeft.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f), posLeft);	
		
		    			Point posRight = {target.x + dist, target.y + altura, target.z};
		    			Camera camRight(posRight, target, {0,1,0}, TARGETING);
		    			glViewport(halfW, halfH, halfW, halfH);
		    			mundito->DrawShape(camRight.GetLookAt(), camRight.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f), posRight);
				        break;
				    }
				    case 2: {
				        Point posPiloto = {target.x + (dirZ_X * distPiloto), target.y + altPiloto, target.z + (dirZ_Z * distPiloto)};
				        Point viewTarget = {posPiloto.x + (dirZ_X * 10.0f), posPiloto.y, posPiloto.z + (dirZ_Z * 10.0f)}; 
				        Camera camPiloto(posPiloto, viewTarget, {0,1,0}, TARGETING);
				        glViewport(0, 0, windowWidth, windowHeight); 
				        mundito->DrawShape(camPiloto.GetLookAt(), camPiloto.GetProjection((float)windowWidth, (float)windowHeight, 0.1f, 100.0f), posPiloto);
				        break;
				    }
					case 3: {
    				    Point carPos = carro->GetWorldPosition();
    				    Point posToma1 = {carPos.x + 20.0f, carPos.y + 10.0f, carPos.z};
    				    Camera camToma1(posToma1, carPos, {0,1,0}, TARGETING);
    				    glViewport(0, 0, windowWidth, windowHeight);
    				    mundito->DrawShape(camToma1.GetLookAt(), camToma1.GetProjection((float)windowWidth, (float)windowHeight, 0.1f, 100.0f), posToma1);
    				    break;
    				}
    				case 4: {
    				    Point carPos = carro->GetWorldPosition();
    				    Point posToma2 = {carPos.x + 1.0f, carPos.y + 30.0f, carPos.z}; 
    				    Camera camToma2(posToma2, carPos, {0,1,0}, TARGETING);
    				    glViewport(0, 0, windowWidth, windowHeight);
    				    mundito->DrawShape(camToma2.GetLookAt(), camToma2.GetProjection((float)windowWidth, (float)windowHeight, 0.1f, 100.0f), posToma2);
    				    break;
    				}
    				case 5: {
    				    Point carPos = carro->GetWorldPosition();
    				    float dirZ_X = carro->Mat.matrix[8];
    				    float dirZ_Z = carro->Mat.matrix[10];
					
    				    Point posToma3 = {carPos.x + (dirZ_X * 20.0f), carPos.y + 2.0f, carPos.z + (dirZ_Z * 20.0f)};
    				    Camera camToma3(posToma3, carPos, {0,1,0}, TARGETING);
    				    glViewport(0, 0, windowWidth, windowHeight);
    				    mundito->DrawShape(camToma3.GetLookAt(), camToma3.GetProjection((float)windowWidth, (float)windowHeight, 0.1f, 100.0f), posToma3);
    				    break;
    				}
				}
			} else {
		    	Point posFront = {target.x, target.y + altura, target.z + dist}; 
		    	Camera camFront(posFront, target, {0,1,0}, TARGETING);
		    	glViewport(0, halfH, halfW, halfH);
		    	mundito->DrawShape(camFront.GetLookAt(), camFront.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f), posFront);
		
		    	Point posBack = {target.x, target.y + altura, target.z - dist};
		    	Camera camBack(posBack, target, {0,1,0}, TARGETING);
		    	glViewport(halfW, halfH, halfW, halfH);
		    	mundito->DrawShape(camBack.GetLookAt(), camBack.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f), posBack);
		
		    	Point posLeft = {target.x - dist, target.y + altura, target.z};
		    	Camera camLeft(posLeft, target, {0,1,0}, TARGETING);
		    	glViewport(0, 0, halfW, halfH);
		    	mundito->DrawShape(camLeft.GetLookAt(), camLeft.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f), posLeft);	
		
		    	Point posRight = {target.x + dist, target.y + altura, target.z};
		    	Camera camRight(posRight, target, {0,1,0}, TARGETING);
		    	glViewport(halfW, 0, halfW, halfH);
		    	mundito->DrawShape(camRight.GetLookAt(), camRight.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f), posRight);
			}
		} else if (modoCochesDivididos) {
		    Point target1 = carro->GetWorldPosition();
		    Point target2 = carro2->GetWorldPosition();

			// car 1
		    float dirZ_X1 = carro->Mat.matrix[8];
		    float dirZ_Z1 = carro->Mat.matrix[10];

		    Point posFront1 = {target1.x + (dirZ_X1 * dist), target1.y + altura, target1.z + (dirZ_Z1 * dist)}; 
		    Camera camFront1(posFront1, target1, {0,1,0}, TARGETING);
		    glViewport(0, halfH, halfW, halfH);
		    mundito->DrawShape(camFront1.GetLookAt(), camFront1.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f), posFront1);

		    Point posBack1 = {target1.x - (dirZ_X1 * dist), target1.y + altura, target1.z - (dirZ_Z1 * dist)};
		    Camera camBack1(posBack1, target1, {0,1,0}, TARGETING);
		    glViewport(0, 0, halfW, halfH);
		    mundito->DrawShape(camBack1.GetLookAt(), camBack1.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f), posBack1);

			// car 2
		    float dirZ_X2 = carro2->Mat.matrix[8];
		    float dirZ_Z2 = carro2->Mat.matrix[10];

		    Point posFront2 = {target2.x + (dirZ_X2 * dist), target2.y + altura, target2.z + (dirZ_Z2 * dist)}; 
		    Camera camFront2(posFront2, target2, {0,1,0}, TARGETING);
		    glViewport(halfW, halfH, halfW, halfH);
		    mundito->DrawShape(camFront2.GetLookAt(), camFront2.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f), posFront2);

		    Point posBack2 = {target2.x - (dirZ_X2 * dist), target2.y + altura, target2.z - (dirZ_Z2 * dist)};
		    Camera camBack2(posBack2, target2, {0,1,0}, TARGETING);
		    glViewport(halfW, 0, halfW, halfH);
		    mundito->DrawShape(camBack2.GetLookAt(), camBack2.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f), posBack2);
		} else if (modoPiloto) {
		    Point target1 = carro->GetWorldPosition();
		    Point target2 = carro2->GetWorldPosition();
		
			// car 1
		    float dirZ_X1 = carro->Mat.matrix[8];
		    float dirZ_Z1 = carro->Mat.matrix[10];
		
		    Point posPiloto1 = {target1.x + (dirZ_X1 * distPiloto), target1.y + altPiloto, target1.z + (dirZ_Z1 * distPiloto)};
		    Point viewTarget1 = {posPiloto1.x + (dirZ_X1 * 10.0f), posPiloto1.y, posPiloto1.z + (dirZ_Z1 * 10.0f)}; 
		    Camera camPiloto1(posPiloto1, viewTarget1, {0,1,0}, TARGETING);
		    glViewport(0, halfH, halfW, halfH);
		    mundito->DrawShape(camPiloto1.GetLookAt(), camPiloto1.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f), posPiloto1);
		
		    Point posBack1 = {target1.x - (dirZ_X1 * dist), target1.y + altura, target1.z - (dirZ_Z1 * dist)};
		    Camera camBack1(posBack1, target1, {0,1,0}, TARGETING);
		    glViewport(0, 0, halfW, halfH);
		    mundito->DrawShape(camBack1.GetLookAt(), camBack1.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f), posBack1);
		
			// car 2
		    float dirZ_X2 = carro2->Mat.matrix[8];
		    float dirZ_Z2 = carro2->Mat.matrix[10];
		
		    Point posPiloto2 = {target2.x + (dirZ_X2 * distPiloto), target2.y + altPiloto, target2.z + (dirZ_Z2 * distPiloto)};
		    Point viewTarget2 = {posPiloto2.x + (dirZ_X2 * 10.0f), posPiloto2.y, posPiloto2.z + (dirZ_Z2 * 10.0f)}; 
		    Camera camPiloto2(posPiloto2, viewTarget2, {0,1,0}, TARGETING);
		    glViewport(halfW, halfH, halfW, halfH);
		    mundito->DrawShape(camPiloto2.GetLookAt(), camPiloto2.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f), posPiloto2);
		
		    Point posBack2 = {target2.x - (dirZ_X2 * dist), target2.y + altura, target2.z - (dirZ_Z2 * dist)};
		    Camera camBack2(posBack2, target2, {0,1,0}, TARGETING);
		    glViewport(halfW, 0, halfW, halfH);
		    mundito->DrawShape(camBack2.GetLookAt(), camBack2.GetProjection((float)halfW, (float)halfH, 0.1f, 100.0f), posBack2);
		}

		glViewport(0, 0, windowWidth, windowHeight);

		if (currentCameraMode == 0 || currentCameraMode == 1) {
			glEnable(GL_SCISSOR_TEST);
			// línea vertical
			glScissor(halfW - 1, 0, 2, windowHeight);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// línea horizontal
			glScissor(0, halfH - 1, windowWidth, 2);
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_SCISSOR_TEST);
		}
		
		glBindVertexArray(0);
        glfwSwapBuffers(window);
    }
	//for(auto p : routePoints)
	//	std::cout << "(" << p.x << ", " << p.y << ", " << p.z << " )\n";
	delete mundito;
	return 0;
}