#pragma once

#include "Shape.h"

class Sphere : public ShapeNode {
public:
    Point center;
    float radius;
    int stacks;
    int slices;
    unsigned int sector_Start, lines_Start, points_Start;

    std::vector<RGB> triColors;
    std::vector<RGB> lineColors;
    std::vector<RGB> pointColors;

public:
    Sphere(World* world, const Point& c, float r, int stacks, int slices);

    void Generate() override;
    void DrawGeometry(const Matrix& parent) override;
    void printMenu() override;
    void handleKey(int key, int mods, char axis) override;
};