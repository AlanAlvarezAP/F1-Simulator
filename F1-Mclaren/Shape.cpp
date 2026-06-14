#include "Shape.h"

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


#include <fstream>
#include <sstream>
#include <iostream>


World::World(){
	root = new ShapeNode(this, GL_TRIANGLES, "ROOT");
	activeSceneNode=nullptr;
	globalColorCounter=0;
}
World::~World(){
	delete root;
}

void World::DrawShape(const Matrix& view,const Matrix& projection){
    Matrix identity;
    root->DrawShape(identity,view,projection);
}

std::vector<unsigned int> World::Add_Batch(std::vector<float>& vectors,std::vector<unsigned int>& indices,unsigned int& offset){
	unsigned int inicio=all_EBOs.size();
	offset=inicio;
	all_vertices.insert(all_vertices.end(),vectors.begin(),vectors.end());
	all_EBOs.insert(all_EBOs.end(),indices.begin(),indices.end());
	return std::vector<unsigned int>(all_EBOs.begin()+inicio,all_EBOs.end());
	
}


void World::print(ShapeNode* rot,int offset){
	if(!rot || (int)rot->children.size() <= 0){
		return;
	}
	
	if(root == rot){
		std::cout << " DATOS GENERALES " << std::endl;
		std::cout << " ALL VERTICES -> " << this->all_vertices.size() << std::endl;
		std::cout << " ALL INDICES -> " << this->all_EBOs.size() << std::endl;
	}
	std::string spaces;
	for (int i = 0; i < offset; i++){
        spaces+="    ";
	}
	std::cout << spaces << "THIS -> " << rot->name << " and size " << rot->EBOs_range.size() << " and offset of " << rot->offset << std::endl;
	for(int i=0;i<rot->children.size();i++){
		std::cout << spaces << "|-----Val -> " << rot->children[i]->name << " and size " << rot->children[i]->EBOs_range.size() << " and offset of " << rot->children[i]->offset << " Index == " << i << std::endl;
	}
	for(auto p:rot->children){
		print(p,offset+1);
	}
}

ShapeNode::ShapeNode(World* world,unsigned int prim,const std::string &nam){
	this->world=world;
	this->primitive=prim;
	this->parent=nullptr;
	this->name=nam;
	this->offset=0;
	this->IsDrawable=false;
	this->selected_part=-1;
}

ShapeNode::~ShapeNode(){
	for (auto son:children){
		delete son;
	}
}

void ShapeNode::AddChildren(ShapeNode* son){
	son->parent=this;
	children.push_back(son);
}

void ShapeNode::ModifiedShaderTransform(const char &tpe,const float &first_val,float second_val=0.0f,char axis='z'){
	this->Mat.UpdateView(tpe,first_val,second_val,axis);
	this->Shader.SetMatrix(this->Mat);
}

void ShapeNode::ModifiedShaderColor(const float &r,const float &g,const float &b){
	color.r=r;
	color.g=g;
	color.b=b;
	this->Shader.SetColor(r,g,b);
}

void ShapeNode::DrawShape(const Matrix& parent,const Matrix& view,const Matrix& projection){
	Matrix global = parent*this->Mat;
	Shader.SetView(view);
	Shader.SetProjection(projection);
	if(IsDrawable){
		this->DrawGeometry(global);
    }
	
	for(auto son : children){
        son->DrawShape(global,view,projection);
	}
}

Matrix ShapeNode::GetWorldMatrix(){
    if (parent)
        return parent->GetWorldMatrix() * Mat;
    return Mat;
}

Point ShapeNode::GetWorldPosition() {
    Matrix w = GetWorldMatrix();
    return Point{w.matrix[12], w.matrix[13], w.matrix[14]};
}

void ShapeNode::SelectNextChild(){
    if(children.empty()){
        selected_part = -1;
        std::cout << "Sin hijos entonces Editando TODO" << std::endl;
        return;
    }

    selected_part++;

    if(selected_part >= (int)children.size()){
        selected_part = -1;
        std::cout << "Editando TODO el objeto" << std::endl;
    } else {
        std::cout << "Editando hijo: " << selected_part << " (" << children[selected_part]->name << ")" << std::endl;
    }
}

void ShapeNode::handleKey(int key, int mods,char CURRENT_AXIS){
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

void ShapeNode::ApplyAnimation(char type,char axis,char local_world,float step){
	Matrix *mate=&(this->Mat);
	//std::cout << name << " step: " << step << std::endl;
	//std::cout << name << " matrix: " << mate->matrix[12] << std::endl;
	switch(type){
		case 'a':
		case 's':{
			mate->UpdateView(type,
			axis=='x'?step:0,
			axis=='y'?step:0,
			axis=='z'?step:0,
			axis,local_world);
			break;
		}
		case 'd':
		case 'f':{
			mate->UpdateView(type,
			step,
			0.0f,
			0.0f,
			axis,local_world);
			break;
		}
		case 'g':
		case 'h':{
			mate->UpdateView(type,
			1.0f+step,
			1.0f+step,
			1.0f+step,
			axis,local_world);
			break;
		}
		default:{
			break;
		}
	}
}

// -------------- LISTA DE CLASES PARA PARSEAR Y MESH LOGIC -------------

std::string Parser::Optimize_Parser(const std::string &line){
	const char* mov=line.c_str();
	
	while(*mov && *mov != ' '){
		mov++;
	}
	mov++;
	
	return mov;
}

FaceVertex Parser::Optimize_Parser_Face(const std::string &line){
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

Point Parser::Optimize_Parser_Numeric(const std::string &line,const int offset){
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


std::vector<unsigned int> Parser::Update_EBos_Vertex(std::vector<float>& send,std::vector<float> &vertices,std::vector<float> &UVs, std::vector<float> &Normals,std::unordered_map<FaceVertex::FaceIndex,unsigned int,FaceIndexHash>& check_repeat,const std::vector<FaceVertex>& faces,unsigned int &base){
    std::vector<unsigned int> indices;
    indices.reserve(faces.size() * 3);

    for(size_t i = 0; i < faces.size(); i++){
		unsigned int idx= send.size()/5 + base;
		auto face = faces[i];
		
		auto it = check_repeat.find(face.first);
		if(it == check_repeat.end()){
			check_repeat[face.first]=idx;
			send.push_back(vertices[face.first.v*3]);
			send.push_back(vertices[face.first.v*3+1]);
			send.push_back(vertices[face.first.v*3+2]);
			
			send.push_back(UVs[face.first.vt*2]);
			send.push_back(UVs[face.first.vt*2+1]);
			
			send.push_back(Normals[face.first.vn*3]);     // NX
			send.push_back(Normals[face.first.vn*3+1]);   // NY
			send.push_back(Normals[face.first.vn*3+2]);   // NZ
			
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
			
			send.push_back(Normals[face.first.vn*3]);     // NX
			send.push_back(Normals[face.first.vn*3+1]);   // NY
			send.push_back(Normals[face.first.vn*3+2]);   // NZ
			
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

			send.push_back(Normals[face.first.vn*3]);     // NX
			send.push_back(Normals[face.first.vn*3+1]);   // NY
			send.push_back(Normals[face.first.vn*3+2]);   // NZ

			indices.push_back(idx++);
		}else{
			indices.push_back(it_3->second);
		}
    }

    return indices;
}


std::unordered_map<std::string, Material> Parser::ParseMTL(const std::string& path) {
    std::unordered_map<std::string, Material> materials;
    std::ifstream file(path);
    std::string line;

    if (!file.is_open()) {
        std::cout << "ERROR: No se pudo abrir el archivo .mtl en la ruta: " << path << std::endl;
        return materials;
    }

    std::string current_mtl = "";
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "newmtl") {
            ss >> current_mtl;
            materials[current_mtl].name = current_mtl;
        } else if (prefix == "Ka" && !current_mtl.empty()) {
            ss >> materials[current_mtl].Ka[0] >> materials[current_mtl].Ka[1] >> materials[current_mtl].Ka[2]; // Lee r, g, b ambiental
        } else if (prefix == "Kd" && !current_mtl.empty()) {
            ss >> materials[current_mtl].Kd[0] >> materials[current_mtl].Kd[1] >> materials[current_mtl].Kd[2]; // Lee r, g, b difuso
        } else if (prefix == "Ks" && !current_mtl.empty()) {
            ss >> materials[current_mtl].Ks[0] >> materials[current_mtl].Ks[1] >> materials[current_mtl].Ks[2]; // Lee r, g, b especular
        } else if (prefix == "Ns" && !current_mtl.empty()) {
            ss >> materials[current_mtl].Ns; // Lee el factor de brillo
        }
    }
    
    file.close();
    return materials;
}



// -------------- LISTA DE POSIBLES FIGURAS POLIMORFICAS -------------
// Circulo
Circle::Circle(World* world,const Point &cent,const float &rad,const int &num_seg):ShapeNode(world,GL_TRIANGLES,"Circulo"),center(cent),radius(rad),number_segments(num_seg){
	ModifiedShaderColor(ColorTable[RUST].r,ColorTable[RUST].g,ColorTable[RUST].b);
}

void Circle::Generate(){
	std::vector<float> vectores;
	std::vector<unsigned int> indices;
	
	float angle_between=(2*PI)/number_segments;
	
	unsigned int base = this->world->all_vertices.size()/3;
	unsigned int base_tmp=base+1;
	
	vectores.push_back(center.x);
	vectores.push_back(center.y);
	vectores.push_back(center.z);
	
	vectores.push_back(center.x + radius);
	vectores.push_back(center.y);
	vectores.push_back(center.z);
	
	for(float i=1;i<=number_segments;i++){
		float ang = i * angle_between;
		vectores.push_back(center.x+radius*std::cos(ang));
        vectores.push_back(center.y+radius*std::sin(ang));
        vectores.push_back(0.0f);
        
        indices.push_back(base);
        indices.push_back(base_tmp);
        indices.push_back(++base_tmp);
	}
	
	this->EBOs_range = this->world->Add_Batch(vectores, indices,offset);
	
}

void Circle::DrawGeometry(const Matrix& parent){
	this->Shader.use();
	this->Shader.SetMatrix(parent);
	this->Shader.SetColor(color.r, color.g, color.b);
	
	glDrawElements(primitive,EBOs_range.size(),GL_UNSIGNED_INT,(void*)(offset*sizeof(unsigned int)));
}

