#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <queue>
#include <stack>
#include <cmath>
#include <random>
#include <algorithm>
#include <thread>
#include <chrono>
#include <utility>
#include <iomanip>

// Para facilitarme la creación de puntos xd
struct Point{
	float x,y,z,angle;
	
	Point operator+(const Point& other) const{
		return {x+other.x,y+other.y,z+other.z,angle};
	}
	Point operator-(const Point& other) const{
		return {x-other.x,y-other.y,z-other.z,angle};
	}
	Point operator*(float s) const{
		return {x*s,y*s,z*s,angle};
	}
	Point operator/ (float s) const{
		return {x/s,y/s,z/s,angle};
	}
};

#endif