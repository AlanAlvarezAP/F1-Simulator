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
	Shader.SetTexture();

    Shader.SetColor(1.0f, 1.0f, 1.0f);

	glDrawElements(GL_TRIANGLES, EBOs_range.size(), GL_UNSIGNED_INT, (void*)(offset * sizeof(unsigned int)));
}

Car::Car(World* world, const char* file_path):ShapeNode(world, 0, "McLaren"), file(file_path) {}
std::string Car::Optimize_Parser(const std::string &line){
	const char* mov=line.c_str();
	
	while(*mov && *mov != ' '){
		mov++;
	}
	mov++;
	
	return mov;
}

FaceVertex Car::Optimize_Parser_Face(const std::string &line){
	FaceVertex face;
	const char* mov=line.c_str() + 2;
	char* end=nullptr;
	
	face.first.v = strtol(mov,&end,10);face.first.v--;mov = end;mov++;
	face.first.vt = strtol(mov,&end,10);face.first.vt--;mov = end;mov++;
	face.first.vn = strtol(mov,&end,10);face.first.vn--;mov = end;mov++;
	
	face.second.v = strtol(mov,&end,10);face.second.v--;mov = end;mov++;
	face.second.vt = strtol(mov,&end,10);face.second.vt--;mov = end;mov++;
	face.second.vn = strtol(mov,&end,10);face.second.vn--;mov = end;mov++;
	
	face.third.v = strtol(mov,&end,10);face.third.v--;mov = end;mov++;
	face.third.vt = strtol(mov,&end,10);face.third.vt--;mov = end;mov++;
	face.third.vn = strtol(mov,&end,10);face.third.vn--;mov = end;
	
	return face;
}

Point Car::Optimize_Parser_Numeric(const std::string &line,const int offset){
	Point tmp;
	const char* mov=line.c_str() + offset;
	char* end=nullptr;
	
	while(*mov == ' '){
		mov++;
	}
	tmp.x = strtof(mov,&end);
	mov=end;
	mov++;
	
	tmp.y = strtof(mov,&end);
	mov=end;
	mov++;
	
	tmp.z = strtof(mov,&end);
	mov=end;
	
	return tmp;
}


std::vector<unsigned int> Car::Update_EBos_Vertex(std::vector<float>& send,std::vector<float> &vertices,std::vector<float> &UVs,std::unordered_map<FaceVertex::FaceIndex,unsigned int,FaceIndexHash>& check_repeat,const std::vector<FaceVertex>& faces){
    std::vector<unsigned int> indices;
    indices.reserve(faces.size() * 3);

    for(size_t i = 0; i < faces.size(); i++){
		unsigned int idx= send.size()/5;
		auto face = faces[i];
		
		auto it = check_repeat.find(face.first);
		if(it == check_repeat.end()){
			check_repeat[face.first]=idx;
			send.push_back(vertices[face.first.v*3]);
			send.push_back(vertices[face.first.v*3+1]);
			send.push_back(vertices[face.first.v*3+2]);
			
			send.push_back(UVs[face.first.vt*2]);
			send.push_back(UVs[face.first.vt*2+1]);
			
			indices.push_back(idx++);
		}else{
			indices.push_back(it->second);
		}
		
		auto it_2 = check_repeat.find(face.second);
		if(it_2 == check_repeat.end()){
			check_repeat[face.second]=idx;
			send.push_back(vertices[face.second.v*3]);
			send.push_back(vertices[face.second.v*3+1]);
			send.push_back(vertices[face.second.v*3+2]);
			
			send.push_back(UVs[face.second.vt*2]);
			send.push_back(UVs[face.second.vt*2+1]);
			
			indices.push_back(idx++);
		}else{
			indices.push_back(it_2->second);
		}
		
		auto it_3 = check_repeat.find(face.third);
		if(it_3 == check_repeat.end()){
			check_repeat[face.third]=idx;
			send.push_back(vertices[face.third.v*3]);
			send.push_back(vertices[face.third.v*3+1]);
			send.push_back(vertices[face.third.v*3+2]);
			
			send.push_back(UVs[face.third.vt*2]);
			send.push_back(UVs[face.third.vt*2+1]);
			
			indices.push_back(idx++);
		}else{
			indices.push_back(it_3->second);
		}
    }

    return indices;
}

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

	std::unordered_map<FaceVertex::FaceIndex,unsigned int,FaceIndexHash> check_repeat;

	vertices.reserve(MAX_SIZE);
	send.reserve(MAX_SIZE);
	UVs.reserve(MAX_SIZE);
	normales.reserve(MAX_SIZE);
	indices.reserve(MAX_SIZE);
	faces.reserve(MAX_SIZE);

    unsigned int base=world->all_vertices.size()/5;
	
	std::string line;
	Mesh *new_mesh=nullptr;
	while(std::getline(lector,line)){
		if(line.empty() || line[0] == '#'){
			continue;
		}
		char inicio=line[0];
		if(inicio=='m'){
			mtlib=Optimize_Parser(line);
		}
		else if(inicio=='o'){
			if(new_mesh){
				unsigned int start = indices.size();
				auto tmp = Update_EBos_Vertex(send,vertices,UVs,check_repeat,new_mesh->faces);
				ShapeNode* node=nullptr;

				std::string path="F:/Comp Grafica/glfw-master/OwnProjects/F1-Mclaren/textures/";
				if(new_mesh->name=="Timon"){
					node=new Timon(world,new_mesh->name,new_mesh->mtl);
					path+=new_mesh->mtl;
					std::cout << "=== CARGANDO TEXTURA ===" << std::endl;
					std::cout << "Mesh: " << new_mesh->name << std::endl;
					std::cout << "MTL file reference: " << new_mesh->mtl << std::endl;
					std::cout << "Full path: " << path << std::endl;
					node->Shader.LoadTexture(path);
				}
				else if(new_mesh->name.substr(0,6)=="Llanta"){
					node=new Llanta(world,new_mesh->name,new_mesh->mtl);
					path+=new_mesh->mtl;
					std::cout << "=== CARGANDO TEXTURA ===" << std::endl;
					std::cout << "Mesh: " << new_mesh->name << std::endl;
					std::cout << "MTL file reference: " << new_mesh->mtl << std::endl;
					std::cout << "Full path: " << path << std::endl;
					node->Shader.LoadTexture(path);
					
				}
				else{
					node=new Chassis(world,new_mesh->name,new_mesh->mtl);
					path+=new_mesh->mtl;
					std::cout << "=== CARGANDO TEXTURA ===" << std::endl;
					std::cout << "Mesh: " << new_mesh->name << std::endl;
					std::cout << "MTL file reference: " << new_mesh->mtl << std::endl;
					std::cout << "Full path: " << path << std::endl;
					node->Shader.LoadTexture(path);
				}

				node->offset=start;
				node->EBOs_range=tmp;
				node->IsDrawable=true;
				AddChildren(node);

				indices.insert(indices.end(),tmp.begin(),tmp.end());
				delete new_mesh;
			}
			
			new_mesh=new Mesh();
			new_mesh->name=Optimize_Parser(line);
		}
		else if(inicio=='u'){
			new_mesh->mtl=Optimize_Parser(line);
		}
		else if(inicio=='v'){
			Point result;
			if(line[1] == ' '){
				result=Optimize_Parser_Numeric(line,2);
				vertices.push_back(result.x);vertices.push_back(result.y);vertices.push_back(result.z);
			}
			else if(line[1] == 't'){
				result=Optimize_Parser_Numeric(line,3);
				UVs.push_back(result.x);UVs.push_back(result.y);
			}
			else if(line[1] == 'n'){
				result=Optimize_Parser_Numeric(line,3);
				normales.push_back(result.x);normales.push_back(result.y);normales.push_back(result.z);
			}
		}
		else if(inicio=='f'){
			new_mesh->faces.push_back(Optimize_Parser_Face(line));
		}
		
	}
	
	if(new_mesh){
		unsigned int start = indices.size();
		auto tmp = Update_EBos_Vertex(send,vertices,UVs,check_repeat,new_mesh->faces);
		ShapeNode* node=nullptr;

		std::string path="F:/Comp Grafica/glfw-master/OwnProjects/F1-Mclaren/textures/";
		if(new_mesh->name=="Timon"){
			node=new Timon(world,new_mesh->name,new_mesh->mtl);
			path+=new_mesh->mtl;
			std::cout << "=== CARGANDO TEXTURA ===" << std::endl;
			std::cout << "Mesh: " << new_mesh->name << std::endl;
			std::cout << "MTL file reference: " << new_mesh->mtl << std::endl;
			std::cout << "Full path: " << path << std::endl;
			node->Shader.LoadTexture(path);
		}
		else if(new_mesh->name=="Mclaren"){
			node=new Chassis(world,new_mesh->name,new_mesh->mtl);
			path+=new_mesh->mtl;
			std::cout << "=== CARGANDO TEXTURA ===" << std::endl;
			std::cout << "Mesh: " << new_mesh->name << std::endl;
			std::cout << "MTL file reference: " << new_mesh->mtl << std::endl;
			std::cout << "Full path: " << path << std::endl;
			node->Shader.LoadTexture(path);
		}
		else{
			node=new Llanta(world,new_mesh->name,new_mesh->mtl);
			path+=new_mesh->mtl;
			std::cout << "=== CARGANDO TEXTURA ===" << std::endl;
			std::cout << "Mesh: " << new_mesh->name << std::endl;
			std::cout << "MTL file reference: " << new_mesh->mtl << std::endl;
			std::cout << "Full path: " << path << std::endl;
			node->Shader.LoadTexture(path);
		}

		node->offset=start;
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