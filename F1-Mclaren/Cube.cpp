#include "Cube.h"

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

Face::Face():triangle_start(0), lines_Start(0), points_Start(0) {}


Cube::Cube(World* world, const Point& cent, std::vector<RGB> colors, int tp, std::string name) :
	ShapeNode(world, 0, name), center(cent), sector_Start(0), lines_Start(0), points_Start(0), type(tp),faceColors(colors) {}

void Cube::Generate(){
    float s = 0.2f;

    Point v[8];

    v[0]={center.x-s,center.y-s,center.z-s};
    v[1]={center.x+s,center.y-s,center.z-s};
    v[2]={center.x+s,center.y+s,center.z-s};
    v[3]={center.x-s,center.y+s,center.z-s};

    v[4]={center.x-s,center.y-s,center.z+s};
    v[5]={center.x+s,center.y-s,center.z+s};
    v[6]={center.x+s,center.y+s,center.z+s};
    v[7]={center.x-s,center.y+s,center.z+s};

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

    EBOs_range =world->Add_Batch(vertices,indices,offset);

    IsDrawable = true;
}

void Cube::DrawGeometry(const Matrix& parent)
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

void Cube::handleKey(int key, int mods,char CURRENT_AXIS){
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

void Cube::printMenu(){
	std::cout << "===================================" << std::endl;
    std::cout << "|        Bienvenido a             |" << std::endl;
    std::cout << "|     Simulador de Cube           |" << std::endl;
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
