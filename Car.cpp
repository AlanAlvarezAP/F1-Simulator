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

	Shader.SetMaterial(material.Ka, material.Kd, material.Ks, material.Ns);

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

	Shader.SetMaterial(material.Ka, material.Kd, material.Ks, material.Ns);
	
	
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

	Shader.SetMaterial(material.Ka, material.Kd, material.Ks, material.Ns);
	

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
	while(std::getline(lector,line)){
		if(line.empty() || line[0] == '#'){
			continue;
		}
		char inicio=line[0];
		if(inicio=='m'){
			mtlib=parser.Optimize_Parser(line);
			std::string path_mtl = "../OwnProjects/F1-Mclaren-Camaras/models/" + mtlib;
            
            mtl_cargados = parser.ParseMTL(path_mtl);
		}
		else if(inicio=='o'){
			if(new_mesh){
				unsigned int start = indices.size();
				auto tmp = parser.Update_EBos_Vertex(send,vertices,UVs,normales,check_repeat,new_mesh->faces,base);
				ShapeNode* node=nullptr;

				std::string path="../OwnProjects/F1-Mclaren-Camaras/textures/";
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
				AddChildren(node);

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
		else if(inicio=='f'){
			new_mesh->faces.push_back(parser.Optimize_Parser_Face(line));
		}
		
	}
	
	if(new_mesh){
		unsigned int start = indices.size();
		auto tmp = parser.Update_EBos_Vertex(send,vertices,UVs,normales,check_repeat,new_mesh->faces,base);
		ShapeNode* node=nullptr;

		std::string path="../OwnProjects/F1-Mclaren-Camaras/textures/";
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
		AddChildren(node);

		indices.insert(indices.end(),tmp.begin(),tmp.end());
		delete new_mesh;
		
	}
	
	this->EBOs_range =world->Add_Batch(send,indices,base);
	std::cout << "========== RESUMEN ==========" << std::endl;
	std::cout << "Vertices : " << vertices.size()/3 << std::endl;
	std::cout << "UVs      : " << UVs.size()/2 << std::endl;
	std::cout << "Normales : " << normales.size()/3 << std::endl;
	std::cout << "=============================" << std::endl;
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