#include "Shaders.h"
#include "Matrix.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

const char* vertex_shader="#version 330 core\n"
"layout (location=0) in vec3 inVec;\n"
"layout (location=1) in vec2 inUVs;\n"
"layout (location=2) in vec3 inNormal;\n"

"out vec2 inOutUV;\n"
"out vec3 FragPos;\n"
"out vec3 Normal;\n"

"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"


"void main(){\n"
"	gl_Position=projection*view*model*vec4(inVec.xyz,1.0f);\n"
"	FragPos = vec3(model * vec4(inVec, 1.0f));\n"

"	Normal = mat3(transpose(inverse(model))) * inNormal;\n"
"	inOutUV=inUVs;\n"
"}\n";

const char* fragment_shader="#version 330 core\n"
"in vec2 inOutUV;\n"
"in vec3 FragPos;\n"\
"in vec3 Normal;\n"
"out vec4 outColor;\n"

// --- Datos del Material (.mtl) ---
"uniform vec3 Ka;\n" // Color Ambiente
"uniform vec3 Kd;\n" // Color Difuso
"uniform vec3 Ks;\n" // Color Especular
"uniform float Ns;\n" // Brillo (Shininess)

// --- Datos de la Luz y Cámara ---
"uniform vec3 lightPos;\n"   // Posición de la luz en el mundo
"uniform vec3 lightColor;\n" // Color de la luz (ej: vec3(1.0))
"uniform vec3 viewPos;\n"    // Posición de la cámara/usuario
"uniform sampler2D ouTexture;\n"
"uniform bool useTexture;\n"

"void main(){\n"
"   vec3 norm = normalize(Normal);\n"
"   vec3 lightDir = normalize(lightPos - FragPos);\n"

    // 1. Componente Ambiental
"   vec3 ambient = Ka * lightColor;\n"

    // 2. Componente Difusa
"   float diff = max(dot(norm, lightDir), 0.0);\n"
"   vec3 diffuse = (diff * Kd) * lightColor;\n"

    // 3. Componente Especular
"   vec3 viewDir = normalize(viewPos - FragPos);\n"
"   vec3 reflectDir = reflect(-lightDir, norm);\n"
"   float spec = pow(max(dot(viewDir, reflectDir), 0.0), Ns);\n"
"   vec3 specular = (spec * Ks) * lightColor;\n"

    // Combinar iluminación
"   vec3 resultLight = ambient + diffuse + specular;\n"

"   if(useTexture){\n"
"       vec4 texColor = texture(ouTexture, inOutUV);\n"
"       outColor = texColor * vec4(resultLight, 1.0);\n"
"	}\n"
"   else{\n"
"       outColor = vec4(resultLight * vec3(1.0), 1.0);\n" // Puedes multiplicar por ouCol si deseas
"	}\n"
"}\n";

Shaders::Shaders(){
	int sucess;
	char infoLog[512];
	
	vertex_shad_id=glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shad_id,1,&vertex_shader,NULL);
	glCompileShader(vertex_shad_id);
	
	glGetShaderiv(vertex_shad_id,GL_COMPILE_STATUS,&sucess);
	if(!sucess){
		glGetShaderInfoLog(vertex_shad_id,512,NULL,infoLog);
		std::cout << "ERROR IN VERTEX SHADER ->" << infoLog << std::endl;
	}
	
	fragment_shad_id=glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shad_id,1,&fragment_shader,NULL);
	glCompileShader(fragment_shad_id);
	
	glGetShaderiv(fragment_shad_id,GL_COMPILE_STATUS,&sucess);
	if(!sucess){
		glGetShaderInfoLog(fragment_shad_id,512,NULL,infoLog);
		std::cout << "ERROR IN FRAGMENT SHADER -> " << infoLog << std::endl;
	}
	
	program_id=glCreateProgram();
	glAttachShader(program_id,vertex_shad_id);
	glAttachShader(program_id,fragment_shad_id);
	glLinkProgram(program_id);
	
	glGetProgramiv(program_id,GL_LINK_STATUS,&sucess);
	if(!sucess){
		glGetProgramInfoLog(program_id,512,NULL,infoLog);
		std::cout << "ERROR IN LINKING SHADERS -> " << infoLog << std::endl;
	}
	
	glDeleteShader(vertex_shad_id);
	glDeleteShader(fragment_shad_id);
}

void Shaders::use(){
	glUseProgram(program_id);
}

unsigned int Shaders::GetProgram(){
	return this->program_id;
}

void Shaders::SetColor(const float &r,const float &g,const float &b){
	int Color_loc=glGetUniformLocation(this->program_id,"ouCol");
	
	if(Color_loc <= -1){
		std::cout << "ERROR or couldn't find the location :( " << std::endl;
		return;
	}
	
	glUniform3f(Color_loc,r,g,b);
}

void Shaders::SetMatrix(const Matrix& mat){
	int Model_loc=glGetUniformLocation(this->program_id,"model");
	
	if(Model_loc <= -1){
		std::cout << "ERROR or couldn't find the model :( " << std::endl;
		return;
	}
	glUniformMatrix4fv(Model_loc,1,GL_FALSE,mat.matrix.data());
}

void Shaders::SetView(const Matrix& mat){
	int Model_loc=glGetUniformLocation(this->program_id,"view");
	
	if(Model_loc <= -1){
		std::cout << "ERROR or couldn't find the view :( " << std::endl;
		return;
	}
	glUniformMatrix4fv(Model_loc,1,GL_FALSE,mat.matrix.data());
}

void Shaders::SetProjection(const Matrix& mat){
	int Model_loc=glGetUniformLocation(this->program_id,"projection");
	
	if(Model_loc <= -1){
		std::cout << "ERROR or couldn't find the projection :( " << std::endl;
		return;
	}
	glUniformMatrix4fv(Model_loc,1,GL_FALSE,mat.matrix.data());
}

void Shaders::LoadTexture(const std::string& path){
    int width, height, channel;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channel, 0);

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (data){
        GLenum format = (channel == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else {
        std::cout << "ERROR failed to load texture: " << path << std::endl;
    }
}

void Shaders::SetTexture(){
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
	int Model_loc=glGetUniformLocation(this->program_id,"ouTexture");
	
	if(Model_loc <= -1){
		std::cout << "ERROR or couldn't find the texture :( " << std::endl;
		return;
	}
	glUniform1i(Model_loc,0);
}

void Shaders::SetUseTexture(bool value){
    int loc = glGetUniformLocation(program_id,"useTexture");

    if(loc == -1)
    {
        std::cout << "Couldn't find useTexture\n";
        return;
    }

    glUniform1i(loc,value);
}

void Shaders::SetMaterial(const float* Ka, const float* Kd, const float* Ks, float Ns) {
    glUniform3fv(glGetUniformLocation(this->program_id, "Ka"), 1, Ka);
    glUniform3fv(glGetUniformLocation(this->program_id, "Kd"), 1, Kd);
    glUniform3fv(glGetUniformLocation(this->program_id, "Ks"), 1, Ks);
    glUniform1f(glGetUniformLocation(this->program_id, "Ns"), Ns);
}

void Shaders::SetLight(const float* pos, const float* color, const float* viewPos) {
    glUniform3fv(glGetUniformLocation(this->program_id, "lightPos"), 1, pos);
    glUniform3fv(glGetUniformLocation(this->program_id, "lightColor"), 1, color);
    glUniform3fv(glGetUniformLocation(this->program_id, "viewPos"), 1, viewPos);
}