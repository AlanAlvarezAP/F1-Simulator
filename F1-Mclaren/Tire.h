#pragma once

#include "Shape.h"
#include <vector>

class Tire : public ShapeNode {
public:
    Point center;
    float radius;
    float height;
    int segments;
    RGB color;

    unsigned int triangle_start;
    unsigned int triangle_count;
    unsigned int lines_start;
    unsigned int lines_count;

public:
    Tire(World* world, const Point& cent, float r, float h, int segs = 16, 
        RGB col = ColorTable[NEGRO], int tp = 0, std::string name = "Tire");
    
    void Generate() override;
    void DrawGeometry(const Matrix& parent) override;
    void printMenu() override;
    void handleKey(int key, int mods, char CURRENT_AXIS) override;
};