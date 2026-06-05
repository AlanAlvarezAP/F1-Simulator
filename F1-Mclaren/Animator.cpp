#include "Animator.h"

Animation_Step::Animation_Step(Animatable* targ,float durat,char tp,float val,char ax,char l_w){
	this->target=targ;
	this->axis=ax;
	this->type=tp;
	this->duration=durat;
	this->elapsed=0.0f;
	this->value_total=val;
	this->local_world=l_w;
}

bool Animation_Step::finished(){
	return this->elapsed >= this->duration;
}


void Animator::Update_animation(const float &dt){
	for(int i=0;i<animations[0].size();i++){
		Animation_Step* anim=animations[0][i];
		if(anim->finished()){
			continue;
		}
		float remaining = anim->duration - anim->elapsed;
		float real_dt = std::min(dt, remaining);
		float step=(anim->value_total/anim->duration)*real_dt;
		anim->elapsed+=real_dt;
		
		anim->target->ApplyAnimation(anim->type,anim->axis,anim->local_world,step);
	}
}

void Animator::Add_Animations(std::vector<Animation_Step*> anim,char inv){
	animations.push_back(anim);
	if(inv=='S'){
		std::vector<Animation_Step*> extra;
		for(auto it=anim.rbegin();it!=anim.rend();it++){
			Animation_Step* cp=new Animation_Step(**it);
			switch(cp->type){
				case 'a':{
					cp->type='s';
					extra.push_back(cp);
					break;
				}
				case 'd':{
					cp->type='f';
					extra.push_back(cp);
					break;
				}
				case 'g':{
					cp->type='h';
					extra.push_back(cp);
					break;
				}
				case 'o':{
					cp->value_total=-cp->value_total;
					extra.push_back(cp);
					break;
				}
				default:{
					delete cp;
					break;
				}
			}
			
		}
		animations.push_back(extra);
	}
	
}

void Animator::Execute_animations(float dt){
    if(animations.empty()){
		return;
    }

    Update_animation(dt);

    bool allFinished = true;
    for(auto anim : animations[0]){
        if(!anim->finished()){
            allFinished = false;
            break;
        }
    }

    if(allFinished){
        for(auto anim : animations[0]){
            delete anim;
        }
        animations.erase(animations.begin());
    }
}
