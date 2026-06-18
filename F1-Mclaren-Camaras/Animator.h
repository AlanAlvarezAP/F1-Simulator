#pragma once

#include "config.h"

// Para realizar animaciones :D
class Animatable{
public:
	virtual void ApplyAnimation(char type,char axis,char local_world,float step)= 0;
	virtual ~Animatable() = default;
};

class Animation_Step{
public:
	Animatable* target;
	char axis,type,local_world;
	float value_total;
	float duration,elapsed;
public:
	Animation_Step(Animatable* targ,float durat,char tp,float val,char ax,char l_w);
	bool finished();
};

class Animator{
public:
	std::vector<std::vector<Animation_Step*>> animations;
public:
	Animator() = default;
	void Add_Animations(std::vector<Animation_Step*> anim,char inv);
	void Execute_animations(float dt);
	void Update_animation(const float &dt);
	
};