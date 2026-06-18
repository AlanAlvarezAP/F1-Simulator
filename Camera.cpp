#include "Camera.h"

float Camera::Normalize(const Point& val1) const{
	return std::sqrt((val1.x*val1.x)+(val1.y*val1.y)+(val1.z*val1.z));
}
float Camera::dot(const Point& a, const Point& b){
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

Point Camera::CrossProduct (const Point& val1,const Point& val2){
	return Point{(val1.y*val2.z-val1.z*val2.y),(val1.z*val2.x- val1.x*val2.z),(val1.x*val2.y-val1.y*val2.x)};
}

void Camera::UpdateCam(Camera_Status stat,const Point& new_target){
	if(stat==TARGETING){
		this->Target=new_target;
		this->Front=(this->Position-this->Target)/Normalize(this->Position-this->Target);
	}else{
		this->Front.x=std::cos(angle_pitch)*std::cos(angle_yaw);
		this->Front.y=std::sin(angle_pitch);
		this->Front.z=std::cos(angle_pitch)*std::sin(angle_yaw);
		this->Front=this->Front/(Normalize(this->Front));
	}
	
	Point CrossLeft=CrossProduct(this->WorldUp,this->Front);
	this->Left=CrossLeft/Normalize(CrossLeft);
	Point CrossUP=CrossProduct(this->Front,this->Left);
	this->OwnUp=CrossUP/Normalize(CrossUP);
	//DebugOrthonormalTest();
}

Camera::Camera(const Point &pos,const Point& target,const Point& wUP,Camera_Status stat){
	this->Position=pos;
	this->WorldUp=wUP;
	this->angle_yaw=YAW*TRANSFORM;
	this->angle_pitch=PITCH*TRANSFORM;
	this->mov_speed=SPEED;
	this->mouse_sensi=SENSITIVITY;
	this->fov=ZOOM;
	this->Target=target;
	UpdateCam(stat,target);
}

Matrix Camera::GetLookAt(){
    Matrix view_1;

    view_1.matrix[0] = Left.x;
    view_1.matrix[4] = Left.y;
    view_1.matrix[8] = Left.z;

    view_1.matrix[1] = OwnUp.x;
    view_1.matrix[5] = OwnUp.y;
    view_1.matrix[9] = OwnUp.z;

    view_1.matrix[2]  = Front.x;
    view_1.matrix[6]  = Front.y;
    view_1.matrix[10] = Front.z;

    view_1.matrix[12] = -dot(Left, Position);
    view_1.matrix[13] = -dot(OwnUp, Position);
    view_1.matrix[14] = -dot(Front, Position);


    return view_1;
}
void Camera::DebugOrthonormalTest(){
    std::cout << "-----------------------------" << std::endl;

    std::cout << "Norm Front: " << Normalize(Front) << std::endl;
    std::cout << "Norm Right: " << Normalize(Left) << std::endl;
    std::cout << "Norm Up:    " << Normalize(OwnUp) << std::endl;

    std::cout << "F*R: " << dot(Front, Left) << std::endl;
    std::cout << "F*U: " << dot(Front, OwnUp) << std::endl;
    std::cout << "R*U: " << dot(Left, OwnUp) << std::endl;

    Point test{
        Left.y*OwnUp.z - Left.z*OwnUp.y,
        Left.z*OwnUp.x - Left.x*OwnUp.z,
        Left.x*OwnUp.y - Left.y*OwnUp.x
    };

    std::cout << "Cross(U,L) vs F norm: "<< Normalize(Point{test.x-Front.x, test.y-Front.y, test.z-Front.z})<< std::endl;
}

Matrix Camera::GetProjection(float width, float height, float nearP, float farP){
    Matrix proj;

    float aspect = width / height;
    float theta = (fov * PI) / 180.0f;
    float t = std::tan(theta / 2.0f);
	float r = t*aspect;

    float A = nearP / r;
    float B = nearP / t;
    float C = -(farP + nearP) / (farP - nearP);
    float D = -(2.0f * farP * nearP) / (farP - nearP);

    proj.matrix[0] = A;
    proj.matrix[1] = 0.0f;
    proj.matrix[2] = 0.0f;
    proj.matrix[3] = 0.0f;

    proj.matrix[4] = 0.0f;
    proj.matrix[5] = B;
    proj.matrix[6] = 0.0f;
    proj.matrix[7] = 0.0f;

    proj.matrix[8]  = 0.0f;
    proj.matrix[9]  = 0.0f;
    proj.matrix[10] = C;
    proj.matrix[11] = -1.0f;

    proj.matrix[12] = 0.0f;
    proj.matrix[13] = 0.0f;
    proj.matrix[14] = D;
    proj.matrix[15] = 0.0f;

    return proj;
}

void Camera::ProcessKeyboard(Camera_Mov dir,float dt){
	float velocity = mov_speed * dt;
	if (dir == BACKWARD){
		Position = Position + (Front * velocity);
	}
	if (dir == FORWARD){
		Position = Position - (Front * velocity);
	}
	if (dir == LEFT){
		Position = Position - (Left * velocity);
	}
	if (dir == RIGHT){
		Position = Position + (Left * velocity);
	}
	
}
void Camera::ProcessMouse(float xoff,float yoff,bool firstMov){
	xoff*=mouse_sensi;
	yoff*=mouse_sensi;
	
	angle_yaw+=xoff;
	angle_pitch+=yoff;
	
	if (angle_pitch > 89.0f){
        angle_pitch = 89.0f;
	}
    if (angle_pitch < -89.0f){
        angle_pitch = -89.0f;
	}
	
	UpdateCam(FREE);
}
void Camera::ProcessScroll(float yoff){
	fov-=yoff;
}

void Camera::ApplyAnimation(char type,char axis,char local_world,float step){
	switch(type){
		case 'a':{
			if(axis=='x'){
				Position.x += step;
			}
			if(axis=='y'){ 
				Position.y += step;
			}
			if(axis=='z'){
				Position.z += step;
			}
			break;
		}
		case 's':{
			if(axis=='x'){
				Position.x -= step;
			}
			if(axis=='y'){
				Position.y -= step;
			}
			if(axis=='z'){
				Position.z -= step;
			}
			break;
		}
		case 'd':{
			if(axis=='y'){
				angle_yaw += step*TRANSFORM;
			}
			if(axis=='x'){
				angle_pitch += step*TRANSFORM;
			}
			UpdateCam(FREE);
			break;
		}
		case 'f':{
			if(axis=='y'){
				angle_yaw -= step*TRANSFORM;
			}
			if(axis=='x'){
				angle_pitch -= step*TRANSFORM;
			}
			UpdateCam(FREE);
			break;
		}
		case 'g':{
			fov += step;
			break;
		}
		case 'h':{
			fov -= step;
			break;
		}
		case 'o':{
			float rad = step * PI / 180.0f;

			Point offset = Position-this->Target;

			Point p;
			if(axis=='y'){
				p.x = offset.x * cos(rad) + offset.z * sin(rad);
				p.z = -offset.x * sin(rad) + offset.z * cos(rad);
				p.y = offset.y;
			}

			if(axis=='x'){
				p.y = offset.y * cos(rad) - offset.z * sin(rad);
				p.z = offset.y * sin(rad) + offset.z * cos(rad);
				p.x = offset.x;
			}
			if(axis=='z'){
				p.x = offset.x * cos(rad) - offset.y * sin(rad);
				p.y = offset.x * sin(rad) + offset.y * cos(rad);
				p.z = offset.z;
			}
			Position = this->Target + p;

			UpdateCam(TARGETING,this->Target);
			break;
		}
		default:{
			std::cout << "Caso desconocido en camara :( " << std::endl;
			break;
		}
	}
}
