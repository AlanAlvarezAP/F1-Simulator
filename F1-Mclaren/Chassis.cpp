#include "Chassis.h"

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

Face::Face():triangle_start(0), lines_Start(0), points_Start(0) {}

void Chassis::moveForward(float step) {
    // tires turned - car rotate Y local
    if (std::abs(steeringAngle) > 0.01f) {
        float turn_factor = 0.4f; // turned factor
        float chassis_turn = step * steeringAngle * turn_factor;
        this->Mat.UpdateView('d', chassis_turn, 0.0f, 0.0f, 'y', 'L');
    }
    this->Mat.UpdateView('a', step, 0.0f, 0.0f, 'z', 'L');
    // tire rolling
    rollAngle += (step / W_RADIUS) * (180.0f / M_PI);
    updateWheels();
}

void Chassis::moveBackward(float step) {
    if (std::abs(steeringAngle) > 0.01f) {
        float turn_factor = 0.4f; // turned factor
        float chassis_turn = step * steeringAngle * turn_factor;
        this->Mat.UpdateView('f', chassis_turn, 0.0f, 0.0f, 'y', 'L');
    }
    this->Mat.UpdateView('s', step, 0.0f, 0.0f, 'z', 'L');
    rollAngle += (step / W_RADIUS) * (180.0f / M_PI);
    updateWheels();
}

void Chassis::steerLeft(float degrees) {
    steeringAngle += degrees;
    if (steeringAngle > 45.0f)
        steeringAngle = 45.0f; // limit
    updateWheels();
}

void Chassis::steerRight(float degrees) {
    steeringAngle -= degrees;
    if (steeringAngle < -45.0f)
        steeringAngle = -45.0f;
    updateWheels();
}

void Chassis::updateWheels() {
    float offsetX = sx * 0.75f;
    float offsetY = -sy;
    float offsetZ = sz + (W_HEIGHT / 2.0f);

    // front tires rotate & spin - back tires rotate
    t_fl->Mat.Restart_Identity(t_fl->Mat.matrix);
    t_fl->Mat.UpdateView('a', center.x + offsetX, center.y + offsetY, center.z + offsetZ, 'z', 'L');
    t_fl->Mat.UpdateView('d', steeringAngle, 0.0f, 0.0f, 'y', 'L');
    t_fl->Mat.UpdateView('d', rollAngle, 0.0f, 0.0f, 'z', 'L');

    t_fr->Mat.Restart_Identity(t_fr->Mat.matrix);
    t_fr->Mat.UpdateView('a', center.x + offsetX, center.y + offsetY, center.z - offsetZ, 'z', 'L');
    t_fr->Mat.UpdateView('d', steeringAngle, 0.0f, 0.0f, 'y', 'L');
    t_fr->Mat.UpdateView('d', rollAngle, 0.0f, 0.0f, 'z', 'L');

    t_rl->Mat.Restart_Identity(t_rl->Mat.matrix);
    t_rl->Mat.UpdateView('a', center.x - offsetX, center.y + offsetY, center.z + offsetZ, 'z', 'L');
    t_rl->Mat.UpdateView('d', rollAngle, 0.0f, 0.0f, 'z', 'L');

    t_rr->Mat.Restart_Identity(t_rr->Mat.matrix);
    t_rr->Mat.UpdateView('a', center.x - offsetX, center.y + offsetY, center.z - offsetZ, 'z', 'L');
    t_rr->Mat.UpdateView('d', rollAngle, 0.0f, 0.0f, 'z', 'L');

    // steering wheel - x2 faster
    float sw_offsetX = sx * 0.6f;
    float sw_offsetY = sy + 0.05f;
    s_w->Mat.Restart_Identity(s_w->Mat.matrix);
    s_w->Mat.UpdateView('a', center.x + sw_offsetX, center.y + sw_offsetY, center.z, 'z', 'L');
    s_w->Mat.UpdateView('d', steeringAngle * 2.0f, 0.0f, 0.0f, 'z', 'L'); 
}

Chassis::Chassis(World* world, const Point& cent, float sx, float sy, float sz, std::vector<RGB> colors, int tp, std::string name) :
	ShapeNode(world, 0, name), center(cent), sx(sx), sy(sy), sz(sz), sector_Start(0), lines_Start(0), points_Start(0), type(tp),faceColors(colors) {
        float offsetX = sx * 0.75f;
        float offsetY = -sy;
        float offsetZ = sz + (W_HEIGHT/ 2.0f);

        t_fl = new Tire(world, {0.0f, 0.0f, 0.0f}, W_RADIUS, W_HEIGHT, 16, ColorTable[NEGRO], 0, "Tire DI");
        t_fr = new Tire(world, {0.0f, 0.0f, 0.0f}, W_RADIUS, W_HEIGHT, 16, ColorTable[NEGRO], 0, "Tire DD");
        t_rl = new Tire(world, {0.0f, 0.0f, 0.0f}, W_RADIUS, W_HEIGHT, 16, ColorTable[NEGRO], 0, "Tire TI");
        t_rr = new Tire(world, {0.0f, 0.0f, 0.0f}, W_RADIUS, W_HEIGHT, 16, ColorTable[NEGRO], 0, "Tire TD");

        float sw_offsetX = sx * 0.6f;
        float sw_offsetY = sy + 0.05f;
        float sw_radius = 0.1f;
        float sw_height = 0.05f;

        s_w = new Tire(world, {0.0f, 0.0f, 0.0f}, sw_radius, sw_height, 8, ColorTable[CAFE], 0, "Steering wheel");

        t_fl->Mat.UpdateView('a', center.x + offsetX, center.y + offsetY, center.z + offsetZ, 'z', 'L');
        t_fr->Mat.UpdateView('a', center.x + offsetX, center.y + offsetY, center.z - offsetZ, 'z', 'L');
        t_rl->Mat.UpdateView('a', center.x - offsetX, center.y + offsetY, center.z + offsetZ, 'z', 'L');
        t_rr->Mat.UpdateView('a', center.x - offsetX, center.y + offsetY, center.z - offsetZ, 'z', 'L');
        s_w->Mat.UpdateView('a', center.x + sw_offsetX, center.y + sw_offsetY, center.z, 'z', 'L');
        //s_w->Mat.UpdateView('d', 90.0f, 'z', 'L');

        this->AddChildren(t_fl);
        this->AddChildren(t_fr);
        this->AddChildren(t_rl);
        this->AddChildren(t_rr);
        this->AddChildren(s_w);
    }

void Chassis::Generate(){
    //float s = 0.2f;
    Point v[8];
    v[0] = {center.x - sx, center.y - sy, center.z - sz};
    v[1] = {center.x + sx, center.y - sy, center.z - sz};
    v[2] = {center.x + sx, center.y + sy, center.z - sz};
    v[3] = {center.x - sx, center.y + sy, center.z - sz};

    v[4] = {center.x - sx, center.y - sy, center.z + sz};
    v[5] = {center.x + sx, center.y - sy, center.z + sz};
    v[6] = {center.x + sx, center.y + sy, center.z + sz};
    v[7] = {center.x - sx, center.y + sy, center.z + sz};

    int facesIdx[6][4]={
        {0,1,2,3},
        {4,7,6,5},
        {0,4,5,1},
        {3,2,6,7},
        {0,3,7,4},
        {1,5,6,2}
    };

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    unsigned int base =world->all_vertices.size()/3;

    for(int i=0;i<6;i++){
        Face& f = faces[i];

        f.a=v[facesIdx[i][0]];
        f.b=v[facesIdx[i][1]];
        f.c=v[facesIdx[i][2]];
        f.d=v[facesIdx[i][3]];

        f.triColor = faceColors[i];
        f.lineColor = ColorTable[NEGRO];
        f.pointColor = ColorTable[NEGRO];

        vertices.insert(vertices.end(),{f.a.x,f.a.y,f.a.z,f.b.x,f.b.y,f.b.z,f.c.x,f.c.y,f.c.z,f.d.x,f.d.y,f.d.z});

        f.triangle_start = indices.size();

        indices.insert(indices.end(),{base,base+1,base+2,base,base+2,base+3});

        f.lines_Start = indices.size();

        indices.insert(indices.end(),{base,base+1,base+1,base+2,base+2,base+3,base+3,base});

        f.points_Start = indices.size();

        indices.insert(indices.end(),{base,base+1,base+2,base+3});

        base += 4;
    }

    EBOs_range = world->Add_Batch(vertices,indices,offset);
    IsDrawable = true;

    t_fl->Generate();
    t_fr->Generate();
    t_rl->Generate();
    t_rr->Generate();
    s_w->Generate();
}

void Chassis::DrawGeometry(const Matrix& parent)
{
    Shader.use();
    Shader.SetMatrix(parent);

    for(int i=0;i<6;i++){
        Face& f = faces[i];

        Shader.SetColor(f.triColor.r,f.triColor.g,f.triColor.b);

        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,(void*)((offset + f.triangle_start)* sizeof(unsigned int)));
    }

    for(int i=0;i<6;i++){
        Face& f = faces[i];

        Shader.SetColor(f.lineColor.r,f.lineColor.g,f.lineColor.b);

        for(int j=0;j<8;j+=2){
            glDrawElements(GL_LINES,2,GL_UNSIGNED_INT,(void*)((offset +f.lines_Start +j)* sizeof(unsigned int)));
        }
    }

    /*
    for(int i=0;i<6;i++){
        Face& f = faces[i];

        Shader.SetColor(f.pointColor.r,f.pointColor.g,f.pointColor.b);

        for(int p=0;p<4;p++){
            glDrawElements(GL_POINTS,1,GL_UNSIGNED_INT,(void*)((offset +f.points_Start +p)* sizeof(unsigned int))
            );
        }
    }
    */
}

void Chassis::handleKey(int key, int mods,char CURRENT_AXIS){
	ShapeNode* target = this;

    if(selected_part != -1){
        target = children[selected_part];
    }
	
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

void Chassis::printMenu(){
	std::cout << "===================================" << std::endl;
    std::cout << "|        Bienvenido a             |" << std::endl;
    std::cout << "|     Simulador de Carro          |" << std::endl;
    std::cout << "|                                 |" << std::endl;
    std::cout << "|  Q. Generar Cube (default=4)    |" << std::endl;
    std::cout << "|  W. Configurar                  |" << std::endl;
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