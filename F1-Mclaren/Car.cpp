#include "Car.h"

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

Llanta::Llanta(World* world,std::string nombre,std::string name_mtl):ShapeNode(world,0,nombre),mtl(name_mtl){}
void Llanta::Generate(){
    IsDrawable = true;
}
void Llanta::DrawGeometry(const Matrix& parent){
    Shader.use();
    Shader.SetMatrix(parent);
	Shader.SetUseTexture(true);
	Shader.SetTexture();

    Shader.SetColor(1.0f, 1.0f, 1.0f);

	glDrawElements(GL_TRIANGLES, EBOs_range.size(), GL_UNSIGNED_INT, (void*)(offset * sizeof(unsigned int)));
}
Timon::Timon(World* world,std::string nombre,std::string name_mtl):ShapeNode(world,0,nombre),mtl(name_mtl){}
void Timon::Generate(){
    IsDrawable = true;
}
void Timon::DrawGeometry(const Matrix& parent){
    Shader.use();
    Shader.SetMatrix(parent);
	Shader.SetUseTexture(true);
	Shader.SetTexture();

    Shader.SetColor(1.0f, 1.0f, 1.0f);

	glDrawElements(GL_TRIANGLES, EBOs_range.size(), GL_UNSIGNED_INT, (void*)(offset * sizeof(unsigned int)));
}
Chassis::Chassis(World* world,std::string nombre,std::string name_mtl):ShapeNode(world,0,nombre),mtl(name_mtl){}
void Chassis::Generate(){
    IsDrawable = true;
}
void Chassis::DrawGeometry(const Matrix& parent){
    Shader.use();
    Shader.SetMatrix(parent);
	Shader.SetUseTexture(true);
	Shader.SetTexture();

    Shader.SetColor(1.0f, 1.0f, 1.0f);

	glDrawElements(GL_TRIANGLES, EBOs_range.size(), GL_UNSIGNED_INT, (void*)(offset * sizeof(unsigned int)));
}

Car::Car(World* world, const char* file_path):ShapeNode(world, 0, "McLaren"), file(file_path) {}
void Car::Generate(){
	std::ifstream lector(file,std::ios::in);
	if(!lector.is_open()){
		std::cout << "Fallo en abrir el .obj" << std::endl;
		return;
	}
	std::vector<float> vertices;
	std::vector<float> send;
	std::vector<float> UVs;
	std::vector<float> normales;
	std::vector<FaceVertex> faces;
    std::vector<unsigned int> indices;
	std::unordered_map<std::string, Material> mtl_cargados;

	std::unordered_map<FaceVertex::FaceIndex,unsigned int,FaceIndexHash> check_repeat;

	vertices.reserve(MAX_SIZE);
	send.reserve(MAX_SIZE);
	UVs.reserve(MAX_SIZE);
	normales.reserve(MAX_SIZE);
	indices.reserve(MAX_SIZE);
	faces.reserve(MAX_SIZE);

    unsigned int base=world->all_vertices.size()/8;
	
	std::string line;
	Mesh *new_mesh=nullptr;
	//chassisGroup = new ShapeNode(world, 0, "ChassisGroup");
	chassisGroup = new CarBodyGroup(world, bodyPitch, bodyRoll);
	chassisGroup->Generate();
	timonGroup = new ShapeNode(world, 0, "TimonGroup");
	wg_fl = new ShapeNode(world, 0, "WG_FL");
	wg_fr = new ShapeNode(world, 0, "WG_FR");
	wg_rl = new ShapeNode(world, 0, "WG_RL");
	wg_rr = new ShapeNode(world, 0, "WG_RR");
	// cumulators for centers
	Point sum_fl={0,0,0}, sum_fr={0,0,0}, sum_rl={0,0,0}, sum_rr={0,0,0};
	int cnt_fl=0, cnt_fr=0, cnt_rl=0, cnt_rr=0;
	while(std::getline(lector,line)){
		if(line.empty() || line[0] == '#')
			continue;
		char inicio=line[0];
		if(inicio=='m'){
			mtlib=parser.Optimize_Parser(line);
			std::string path_mtl = "../OwnProjects/F1-Mclaren/models/" + mtlib;
            mtl_cargados = parser.ParseMTL(path_mtl);
		}
		else if(inicio=='o'){
			if(new_mesh){
				unsigned int start = indices.size();
				auto tmp = parser.Update_EBos_Vertex(send,vertices,UVs,normales,check_repeat,new_mesh->faces,base);
				ShapeNode* node=nullptr;

				std::string path="../OwnProjects/F1-Mclaren/textures/";
				if(new_mesh->name=="Timon"){
					node=new Timon(world,new_mesh->name,new_mesh->mtl);
					path+=new_mesh->mtl;
					std::cout << "=== CARGANDO TEXTURA ===" << std::endl;
					std::cout << "Mesh: " << new_mesh->name << std::endl;
					std::cout << "MTL file reference: " << new_mesh->mtl << std::endl;
					std::cout << "Full path: " << path << std::endl;
					node->Shader.LoadTexture(path);
					node->material = mtl_cargados[new_mesh->mtl];
				}
				else if(new_mesh->name.substr(0,6)=="Llanta"){
					node=new Llanta(world,new_mesh->name,new_mesh->mtl);
					path+=new_mesh->mtl;
					std::cout << "=== CARGANDO TEXTURA ===" << std::endl;
					std::cout << "Mesh: " << new_mesh->name << std::endl;
					std::cout << "MTL file reference: " << new_mesh->mtl << std::endl;
					std::cout << "Full path: " << path << std::endl;
					node->Shader.LoadTexture(path);
					node->material = mtl_cargados[new_mesh->mtl];
				}
				else{
					node=new Chassis(world,new_mesh->name,new_mesh->mtl);
					path+=new_mesh->mtl;
					std::cout << "=== CARGANDO TEXTURA ===" << std::endl;
					std::cout << "Mesh: " << new_mesh->name << std::endl;
					std::cout << "MTL file reference: " << new_mesh->mtl << std::endl;
					std::cout << "Full path: " << path << std::endl;
					node->Shader.LoadTexture(path);
					node->material = mtl_cargados[new_mesh->mtl];
				}

				node->offset=world->all_EBOs.size()+start;
				node->EBOs_range=tmp;
				node->IsDrawable=true;
				//this->AddChildren(node);
				const std::string& n = new_mesh->name;
				if(n=="Halo"||n=="Discos_traseros"||n=="Interior"||n=="Chasis"||n=="Chasis2"||n=="McLaren")
					chassisGroup->AddChildren(node);
				else if(n=="Timon")
					timonGroup->AddChildren(node);
				else if(n=="Llanta_delantera_izq"||n=="Eje_delantera_izq")
					wg_fl->AddChildren(node);
				else if(n=="Llanta_delantera_der"||n=="Eje_delantera_der")
					wg_fr->AddChildren(node);
				else if(n=="Llanta_trasera_izq"||n=="Eje_trasero_izq")
					wg_rl->AddChildren(node);
				else if(n=="Llanta_trasera_der"||n=="Eje_trasero_der")
					wg_rr->AddChildren(node);

				indices.insert(indices.end(),tmp.begin(),tmp.end());
				delete new_mesh;
			}
			
			new_mesh=new Mesh();
			new_mesh->name=parser.Optimize_Parser(line);
		}
		else if(inicio=='u'){
			new_mesh->mtl=parser.Optimize_Parser(line);
		}
		else if(inicio=='v'){
			Point result;
			if(line[1] == ' '){
				result=parser.Optimize_Parser_Numeric(line,2);
				vertices.push_back(result.x);vertices.push_back(result.y);vertices.push_back(result.z);
				if(new_mesh){
					if(new_mesh->name=="Llanta_delantera_izq"||new_mesh->name=="Eje_delantera_izq"){
						sum_fl.x+=result.x; sum_fl.y+=result.y; sum_fl.z+=result.z; cnt_fl++;
					}else if(new_mesh->name=="Llanta_delantera_der"||new_mesh->name=="Eje_delantera_der"){
						sum_fr.x+=result.x; sum_fr.y+=result.y; sum_fr.z+=result.z; cnt_fr++;
					}else if(new_mesh->name=="Llanta_trasera_izq"||new_mesh->name=="Eje_trasero_izq"){
						sum_rl.x+=result.x; sum_rl.y+=result.y; sum_rl.z+=result.z; cnt_rl++;
					}else if(new_mesh->name=="Llanta_trasera_der"||new_mesh->name=="Eje_trasero_der"){
						sum_rr.x+=result.x; sum_rr.y+=result.y; sum_rr.z+=result.z; cnt_rr++;
					}
				}
			}
			else if(line[1] == 't'){
				result=parser.Optimize_Parser_Numeric(line,3);
				UVs.push_back(result.x);UVs.push_back(result.y);
			}
			else if(line[1] == 'n'){
				result=parser.Optimize_Parser_Numeric(line,3);
				normales.push_back(result.x);normales.push_back(result.y);normales.push_back(result.z);
			}
		}
		else if(inicio=='f')
			new_mesh->faces.push_back(parser.Optimize_Parser_Face(line));
	}
	if(new_mesh){
		unsigned int start = indices.size();
		auto tmp = parser.Update_EBos_Vertex(send,vertices,UVs,normales,check_repeat,new_mesh->faces,base);
		ShapeNode* node=nullptr;
		std::string path="../OwnProjects/F1-Mclaren/textures/";
		if(new_mesh->name=="Timon"){
			node=new Timon(world,new_mesh->name,new_mesh->mtl);
			path+=new_mesh->mtl;
			std::cout << "=== CARGANDO TEXTURA ===" << std::endl;
			std::cout << "Mesh: " << new_mesh->name << std::endl;
			std::cout << "MTL file reference: " << new_mesh->mtl << std::endl;
			std::cout << "Full path: " << path << std::endl;
			node->Shader.LoadTexture(path);
			node->material = mtl_cargados[new_mesh->mtl];
		}
		else if(new_mesh->name=="Mclaren"){
			node=new Chassis(world,new_mesh->name,new_mesh->mtl);
			path+=new_mesh->mtl;
			std::cout << "=== CARGANDO TEXTURA ===" << std::endl;
			std::cout << "Mesh: " << new_mesh->name << std::endl;
			std::cout << "MTL file reference: " << new_mesh->mtl << std::endl;
			std::cout << "Full path: " << path << std::endl;
			node->Shader.LoadTexture(path);
			node->material = mtl_cargados[new_mesh->mtl];
		}
		else{
			node=new Llanta(world,new_mesh->name,new_mesh->mtl);
			path+=new_mesh->mtl;
			std::cout << "=== CARGANDO TEXTURA ===" << std::endl;
			std::cout << "Mesh: " << new_mesh->name << std::endl;
			std::cout << "MTL file reference: " << new_mesh->mtl << std::endl;
			std::cout << "Full path: " << path << std::endl;
			node->Shader.LoadTexture(path);
			node->material = mtl_cargados[new_mesh->mtl];
		}
		node->offset=world->all_EBOs.size()+start;
		node->EBOs_range=tmp;
		node->IsDrawable=true;
		//AddChildren(node);
		const std::string& n = new_mesh->name;
		if(n=="Halo"||n=="Discos_traseros"||n=="Interior"||n=="Chasis"||n=="Chasis2"||n=="McLaren")
			chassisGroup->AddChildren(node);
		else if(n=="Timon")
			timonGroup->AddChildren(node);
		else if(n=="Llanta_delantera_izq"||n=="Eje_delantera_izq")
			wg_fl->AddChildren(node);
		else if(n=="Llanta_delantera_der"||n=="Eje_delantera_der")
			wg_fr->AddChildren(node);
		else if(n=="Llanta_trasera_izq"||n=="Eje_trasero_izq")
			wg_rl->AddChildren(node);
		else if(n=="Llanta_trasera_der"||n=="Eje_trasero_der")
			wg_rr->AddChildren(node);

		indices.insert(indices.end(),tmp.begin(),tmp.end());
		delete new_mesh;
	}
	this->EBOs_range =world->Add_Batch(send,indices,base);
	// calc wheel centers
	wc_fl = {sum_fl.x/cnt_fl, sum_fl.y/cnt_fl, sum_fl.z/cnt_fl};
	wc_fr = {sum_fr.x/cnt_fr, sum_fr.y/cnt_fr, sum_fr.z/cnt_fr};
	wc_rl = {sum_rl.x/cnt_rl, sum_rl.y/cnt_rl, sum_rl.z/cnt_rl};
	wc_rr = {sum_rr.x/cnt_rr, sum_rr.y/cnt_rr, sum_rr.z/cnt_rr};
	this->AddChildren(chassisGroup);
	this->AddChildren(timonGroup);
	this->AddChildren(wg_fl);
	this->AddChildren(wg_fr);
	this->AddChildren(wg_rl);
	this->AddChildren(wg_rr);
	std::cout << "========== RESUMEN ==========" << std::endl;
	std::cout << "Vertices : " << vertices.size()/3 << std::endl;
	std::cout << "UVs      : " << UVs.size()/2 << std::endl;
	std::cout << "Normales : " << normales.size()/3 << std::endl;
	std::cout << "=============================" << std::endl;
}

void Car::moveForward(float step) {
    if (std::abs(steeringAngle) > 0.01f) {
        float chassis_turn = step * steeringAngle * 0.4f; // 0.4 turn factor
        this->Mat.UpdateView('d', chassis_turn, 0.0f, 0.0f, 'y', 'L');
    }
    this->Mat.UpdateView('a', 0.0f, 0.0f, step, 'z', 'L');
    rollAngle += (step / 0.35f) * (180.0f / M_PI); // 0.35 aprox ratio
    updateWheels();
}

void Car::moveBackward(float step) {
    if (std::abs(steeringAngle) > 0.01f) {
        float chassis_turn = step * steeringAngle * 0.4f;
        this->Mat.UpdateView('d', chassis_turn, 0.0f, 0.0f, 'y', 'L');
    }
    this->Mat.UpdateView('s', 0.0f, 0.0f, step, 'z', 'L');
    rollAngle -= (step / 0.35f) * (180.0f / M_PI);
    updateWheels();
}

void Car::steerLeft(float degrees) {
    steeringAngle += degrees;
    if (steeringAngle > maxSteerAngle)
        steeringAngle = maxSteerAngle;
    updateWheels();
}

void Car::steerRight(float degrees) {
    steeringAngle -= degrees;
    if (steeringAngle < -maxSteerAngle)
        steeringAngle = -maxSteerAngle;
    updateWheels();
}

void Car::updateWheels() {
	wg_fl->Mat.Restart_Identity(wg_fl->Mat.matrix);
    wg_fl->Mat.UpdateView('a', wc_fl.x, wc_fl.y, wc_fl.z, 'z', 'L');
    wg_fl->Mat.UpdateView('d', steeringAngle, 0.0f, 0.0f, 'y', 'L');
    wg_fl->Mat.UpdateView('d', rollAngle, 0.0f, 0.0f, 'x', 'L');
    wg_fl->Mat.UpdateView('s', wc_fl.x, wc_fl.y, wc_fl.z, 'z', 'L');

    wg_fr->Mat.Restart_Identity(wg_fr->Mat.matrix);
    wg_fr->Mat.UpdateView('a', wc_fr.x, wc_fr.y, wc_fr.z, 'z', 'L');
    wg_fr->Mat.UpdateView('d', steeringAngle, 0.0f, 0.0f, 'y', 'L');
    wg_fr->Mat.UpdateView('d', rollAngle, 0.0f, 0.0f, 'x', 'L');
    wg_fr->Mat.UpdateView('s', wc_fr.x, wc_fr.y, wc_fr.z, 'z', 'L');

    wg_rl->Mat.Restart_Identity(wg_rl->Mat.matrix);
    wg_rl->Mat.UpdateView('a', wc_rl.x, wc_rl.y, wc_rl.z, 'z', 'L');
    wg_rl->Mat.UpdateView('d', rollAngle, 0.0f, 0.0f, 'x', 'L');
    wg_rl->Mat.UpdateView('s', wc_rl.x, wc_rl.y, wc_rl.z, 'z', 'L');

    wg_rr->Mat.Restart_Identity(wg_rr->Mat.matrix);
    wg_rr->Mat.UpdateView('a', wc_rr.x, wc_rr.y, wc_rr.z, 'z', 'L');
    wg_rr->Mat.UpdateView('d', rollAngle, 0.0f, 0.0f, 'x', 'L');
    wg_rr->Mat.UpdateView('s', wc_rr.x, wc_rr.y, wc_rr.z, 'z', 'L');

    timonGroup->Mat.Restart_Identity(timonGroup->Mat.matrix);
    timonGroup->Mat.UpdateView('f', steeringAngle * 1.5f, 0.0f, 0.0f, 'z', 'L');
}

void Car::updatePhysics(float dt, bool inForward, bool inBackward, bool inBrake, bool inLeft, bool inRight) {
    previousSpeed = currentSpeed;
    if (inBrake) {
        if (currentSpeed > 0.0f) {
            currentSpeed -= brakeSpeed * dt;
            if (currentSpeed < 0.0f)
                currentSpeed = 0.0f;
        } else if (currentSpeed < 0.0f) {
            currentSpeed += brakeSpeed * dt;
            if (currentSpeed > 0.0f)
                currentSpeed = 0.0f;
        }
    }
    if (inForward) {
        currentSpeed += acceleration * dt;
        if (currentSpeed > maxSpeed)
            currentSpeed = maxSpeed;
    } else if (inBackward) {
        currentSpeed -= acceleration * dt;
        if (currentSpeed < maxReverse)
            currentSpeed = maxReverse;
    } else {
        // friction
        if (currentSpeed > 0.0f) {
            currentSpeed -= deceleration * dt;
            if (currentSpeed < 0.0f)
                currentSpeed = 0.0f;
        } else if (currentSpeed < 0.0f) {
            currentSpeed += deceleration * dt;
            if (currentSpeed > 0.0f)
                currentSpeed = 0.0f;
        }
    }
    if (std::abs(currentSpeed) > 0.001f) {
        if (currentSpeed > 0.0f)
            moveForward(currentSpeed * dt);
        else
            moveBackward(-currentSpeed * dt); // - * - = +
    }
    float instantAcceleration = (currentSpeed - previousSpeed) / dt;
    // balancing
    float targetPitch = instantAcceleration * visualBalancingFactor;
    if (targetPitch > limitPitch)
        targetPitch = limitPitch;
    if (targetPitch < -limitPitch)
        targetPitch = -limitPitch;
    currentPitch += (targetPitch - currentPitch) * pitchSpring * dt;
    // rolling
    float targetRoll = currentSpeed * steeringAngle * visualRollingFactor;
    if (targetRoll > limitRoll)
        targetRoll = limitRoll;
    if (targetRoll < -limitRoll)
        targetRoll = -limitRoll;
    currentRoll += (targetRoll - currentRoll) * rollSpring * dt;
    // to local vars
    bodyPitch = currentPitch;
    bodyRoll = currentRoll;
	// to chassis
	chassisGroup->Mat.Restart_Identity(chassisGroup->Mat.matrix);
    if (std::abs(bodyRoll) > 0.001f)
        chassisGroup->Mat.UpdateView('d', bodyRoll, 0.0f, 0.0f, 'z', 'L');
    if (std::abs(bodyPitch) > 0.001f)
        chassisGroup->Mat.UpdateView('f', bodyPitch, 0.0f, 0.0f, 'x', 'L');
    // saving
    previousPitch = currentPitch;
    previousRoll = currentRoll;
    // steering
    if (inLeft)
        steerLeft(steerSpeed * dt);
    if (inRight)
        steerRight(steerSpeed * dt);
	if (!scriptControlsSteering) {
		if (!inLeft && !inRight) {
			if (steeringAngle > 0.0f) {
				steeringAngle -= returnSteerSpeed * dt;
				if (steeringAngle < 0.0f)
					steeringAngle = 0.0f;
			} else if (steeringAngle < 0.0f) {
				steeringAngle += returnSteerSpeed * dt;
				if (steeringAngle > 0.0f)
					steeringAngle = 0.0f;
			}
			updateWheels();
		}
	} else
		steeringAngle = currentSteerAngle;
}

void Car::handleKey(int key, int mods,char CURRENT_AXIS){
	ShapeNode* target = this;
    if(selected_part != -1)
        target = children[selected_part];
    Matrix* mat = &(target->Mat);
    switch(key){
		case GLFW_KEY_UP:{
			mat->UpdateView('a',0.0f,0.1f,0.0f,CURRENT_AXIS,'W');
			break;
		}
		case GLFW_KEY_DOWN:{
			mat->UpdateView('a',0.0f,-0.1f,0.0f,CURRENT_AXIS,'W');
			break;
		}
		case GLFW_KEY_RIGHT:{
			mat->UpdateView('a',0.1f,0.0f,0.0f,CURRENT_AXIS,'W');
			break;
		}
		case GLFW_KEY_LEFT:{
			mat->UpdateView('a',-0.1f,0.0f,0.0f,CURRENT_AXIS,'W');
			break;
		}
		case GLFW_KEY_PAGE_UP:{
			mat->UpdateView('a',0.0f,0.0f,0.1f,CURRENT_AXIS,'W');
			break;
		}
		case GLFW_KEY_PAGE_DOWN:{
			mat->UpdateView('a',0.0f,0.0f,-0.1f,CURRENT_AXIS,'W');
			break;
		}
		case GLFW_KEY_D:{
			mat->UpdateView('d',10.0f,0.0f,0.0f,CURRENT_AXIS,'W');
			break;
		}
		case GLFW_KEY_F:{
			mat->UpdateView('f',10.0f,0.0f,0.0f,CURRENT_AXIS,'W');
			break;
		}
		case GLFW_KEY_G:{
			mat->UpdateView('g', 1.1f, 1.1f,1.1f,CURRENT_AXIS,'W');
			break;
		}
		case GLFW_KEY_H:{
			mat->UpdateView('g', 0.9f, 0.9f,0.9f,CURRENT_AXIS,'W');
			break;
		}
		default:{
			break;
		}
	}
}

void Car::printMenu(){
	std::cout << "===================================" << std::endl;
    std::cout << "|        Bienvenido a             |" << std::endl;
    std::cout << "|     Simulador de F1             |" << std::endl;
    std::cout << "|                                 |" << std::endl;
    std::cout << "|  R. Seleccion parte (0-N)       |" << std::endl;
	std::cout << "|  d. Rotar (0.1)                 |" << std::endl;
	std::cout << "|  f. Rotar inverso (0.1)         |" << std::endl;
	std::cout << "|  g. Escalar (1.1)               |" << std::endl;
	std::cout << "|  h. Escalar inverso (0.9)       |" << std::endl;
	std::cout << "|  x. Usar eje x                  |" << std::endl;
	std::cout << "|  y. Usar eje y                  |" << std::endl;
	std::cout << "|  z. Usar eje z                  |" << std::endl;
    std::cout << "|  4. Mover arriba (Flecha arr)   |" << std::endl;
    std::cout << "|  5. Mover abajo (Flecha abj)    |" << std::endl;
    std::cout << "|  6. Mover derecha (Flecha der)  |" << std::endl;
	std::cout << "|  7. Mover izquierda (Flecha izq)|" << std::endl;
    std::cout << "|  8. Salir (ESC o CTRL+C)        |" << std::endl;
    std::cout << "===================================" << std::endl;
}

CarBodyGroup::CarBodyGroup(World* w, float& p, float& r)
	: ShapeNode(w, 0, "ChassisGroup"), bodyPitch(p), bodyRoll(r) {}
void CarBodyGroup::Generate(){
    IsDrawable = true;
}

void Car::addScriptStep(bool fwd, bool brk, float duration, float speed, float angle) {
    ScriptStep step;
    step.inputForward = fwd;
    step.inputBrake = brk;
    step.targetDuration = duration;
    step.targetSpeed = speed;
    step.targetSteerAngle = angle;
    animScript.push(step);
}

void Car::updateScriptAnimation(float dt, bool& outFwd, bool& outBrk) {
    if (animScript.empty()) {
        scriptControlsSteering = false;
        isScriptPlaying = false;
        outFwd = false;
        outBrk = false;
        return;
    }
    ScriptStep& current = animScript.front();
    scriptControlsSteering = true;
    currentSteerAngle = current.targetSteerAngle;
    if (currentSpeed < current.targetSpeed - 1.0f) {
        outFwd = true;
        outBrk = false;
    } else if (currentSpeed > current.targetSpeed + 1.0f) {
        outFwd = false;
        outBrk = true;
    } else {
        outFwd = false;
        outBrk = false;
    }
    currentStepTimer += dt;
    if (currentStepTimer >= current.targetDuration) {
        currentStepTimer = 0.0f;
        animScript.pop();
    }
}