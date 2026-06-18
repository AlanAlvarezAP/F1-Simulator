#include "Sphere.h"

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

Sphere::Sphere(World* world, const Point& c, float r, int st, int sl): ShapeNode(world, GL_TRIANGLES, "Sphere"),center(c), radius(r), stacks(st), slices(sl),sector_Start(0), lines_Start(0), points_Start(0) {}


void Sphere::Generate() {

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<unsigned int> lineIdx;
    std::vector<unsigned int> pointIdx;

    unsigned int base = world->all_vertices.size() / 3;

    const float PI = 3.14159265f;

    for(int i = 0; i <= stacks; i++) {
        float phi = PI * i / stacks;

        for(int j = 0; j <= slices; j++) {
            float theta = 2.0f * PI * j / slices;

            float x = center.x + radius * sin(phi) * cos(theta);
            float y = center.y + radius * cos(phi);
            float z = center.z + radius * sin(phi) * sin(theta);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    for(int i = 0; i < stacks; i++) {
        for(int j = 0; j < slices; j++) {

            int a = base + i * (slices + 1) + j;
            int b = base + (i+1) * (slices + 1) + j;
            int c = base + (i+1) * (slices + 1) + (j+1);
            int d = base + (i) * (slices + 1) + (j+1);

            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(c);

            indices.push_back(a);
            indices.push_back(c);
            indices.push_back(d);
        }
    }

    for(int i = 0; i <= stacks; i++) {
        for(int j = 0; j < slices; j++) {
            lineIdx.push_back(base + i * (slices + 1) + j);
            lineIdx.push_back(base + i * (slices + 1) + (j+1));
        }
    }

    for(int j = 0; j <= slices; j++) {
        for(int i = 0; i < stacks; i++) {
            lineIdx.push_back(base + i * (slices + 1) + j);
            lineIdx.push_back(base + (i+1) * (slices + 1) + j);
        }
    }

    for(int i = 0; i <= stacks; i++) {
        for(int j = 0; j <= slices; j++) {
            pointIdx.push_back(base + i * (slices + 1) + j);
        }
    }

    sector_Start = 0;
    lines_Start = indices.size();
    points_Start = lines_Start + lineIdx.size();

    indices.insert(indices.end(), lineIdx.begin(), lineIdx.end());
    indices.insert(indices.end(), pointIdx.begin(), pointIdx.end());

    EBOs_range = world->Add_Batch(vertices, indices, offset);
    IsDrawable = true;

    int &idxColor = world->globalColorCounter;
    const int COLOR_COUNT = ARENA + 1;

    triColors.clear();
    lineColors.clear();
    pointColors.clear();

    int triCount = (indices.size() / 6);
    int lineCount = lineIdx.size() / 2;
    int pointCount = pointIdx.size();

    for(int i = 0; i < triCount; i++){
        triColors.push_back(ColorTable[idxColor++ % COLOR_COUNT]);
	}

    for(int i = 0; i < lineCount; i++){
        lineColors.push_back(ColorTable[idxColor++ % COLOR_COUNT]);
	}

    for(int i = 0; i < pointCount; i++){
        pointColors.push_back(ColorTable[idxColor++ % COLOR_COUNT]);
	}

    world->globalColorCounter = idxColor;
}

void Sphere::DrawGeometry(const Matrix& parent) {

    Shader.use();
    Shader.SetMatrix(parent);

    for(int i = sector_Start, t = 0; i < lines_Start; i += 6, t++) {
        RGB c = triColors[t % triColors.size()];
        Shader.SetColor(c.r, c.g, c.b);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,(void*)((offset + i) * sizeof(unsigned int)));
    }

    for(int i = lines_Start, l = 0; i < points_Start; i += 2, l++) {
        RGB c = lineColors[l % lineColors.size()];
        Shader.SetColor(c.r, c.g, c.b);

        glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT,(void*)((offset + i) * sizeof(unsigned int)));
    }

    for(int i = points_Start, p = 0; i < EBOs_range.size(); i += 1, p++) {
        RGB c = pointColors[p % pointColors.size()];
        Shader.SetColor(c.r, c.g, c.b);

        glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT,(void*)((offset + i) * sizeof(unsigned int)));
    }
}

void Sphere::handleKey(int key, int mods,char CURRENT_AXIS){
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

void Sphere::printMenu(){
	std::cout << "===================================" << std::endl;
    std::cout << "|        Bienvenido a             |" << std::endl;
    std::cout << "|     Simulador de Sphere         |" << std::endl;
    std::cout << "|                                 |" << std::endl;
    std::cout << "|  Q. Generar Sphere (default=4)  |" << std::endl;
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
	std::cout << " AL TERMINAR DE ESCRIBIR LA PARTE O CONFIGURACIÓN DE REBANDAS CONFIRMAR CON ENTER " << std::endl;
}

