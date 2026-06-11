#include "Tire.h"

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <cmath>

Tire::Tire(World* world, const Point& cent, float r, float h, int segs, RGB col, int tp, std::string name) :
    ShapeNode(world, 0, name), center(cent), radius(r), height(h), segments(segs), color(col),
    triangle_start(0), triangle_count(0), lines_start(0), lines_count(0) {}

void Tire::Generate() {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    unsigned int base = world->all_vertices.size() / 3;
    // z limits
    vertices.insert(vertices.end(), {center.x, center.y, center.z + height / 2.0f});
    vertices.insert(vertices.end(), {center.x, center.y, center.z - height / 2.0f});

    // superior perimeter
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * M_PI * float(i) / float(segments);
        float px = center.x + radius * cos(theta);
        float py = center.y + radius * sin(theta);
        vertices.insert(vertices.end(), {px, py, center.z + height / 2.0f});
    }
    // inferior perimeter
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * M_PI * float(i) / float(segments);
        float px = center.x + radius * cos(theta);
        float py = center.y + radius * sin(theta);
        vertices.insert(vertices.end(), {px, py, center.z - height / 2.0f});
    }
    // idx for triangles
    triangle_start = indices.size();
    
    for (int i = 0; i < segments; i++) {
        int next_i = (i + 1) % segments;

        unsigned int top_center = base + 0;
        unsigned int bottom_center = base + 1;
        // +2 cuz 0 & 1 r centers
        unsigned int top_current = base + 2 + i;
        unsigned int top_next = base + 2 + next_i;
        unsigned int bottom_current = base + 2 + segments + i;
        unsigned int bottom_next = base + 2 + segments + next_i;
        // triangle superior limit
        indices.insert(indices.end(), {top_center, top_current, top_next});
        // triangle inferior limit
        indices.insert(indices.end(), {bottom_center, bottom_next, bottom_current});
        // lateral faces
        indices.insert(indices.end(), {top_current, bottom_current, top_next});
        indices.insert(indices.end(), {top_next, bottom_current, bottom_next});
    }
    triangle_count = indices.size() - triangle_start;
    // idx for lines
    lines_start = indices.size();
    for (int i = 0; i < segments; i++) {
        int next_i = (i + 1) % segments;
        unsigned int top_current = base + 2 + i;
        unsigned int top_next = base + 2 + next_i;
        unsigned int bottom_current = base + 2 + segments + i;
        unsigned int bottom_next = base + 2 + segments + next_i;
        // superior & inferior circle lines
        indices.insert(indices.end(), {top_current, top_next});
        indices.insert(indices.end(), {bottom_current, bottom_next});
        // vertical line - connect limits
        indices.insert(indices.end(), {top_current, bottom_current});
    }
    lines_count = indices.size() - lines_start;

    EBOs_range = world->Add_Batch(vertices, indices, offset);
    IsDrawable = true;
}

void Tire::DrawGeometry(const Matrix& parent) {
    Shader.use();
    Shader.SetMatrix(parent);
    // solid surface
    Shader.SetColor(color.r, color.g, color.b);
    glDrawElements(GL_TRIANGLES, triangle_count, GL_UNSIGNED_INT, (void*)((offset + triangle_start) * sizeof(unsigned int)));
    // borders
    Shader.SetColor(0.2f, 0.2f, 0.2f); 
    glDrawElements(GL_LINES, lines_count, GL_UNSIGNED_INT, (void*)((offset + lines_start) * sizeof(unsigned int)));
}

void Tire::handleKey(int key, int mods, char CURRENT_AXIS) {
    ShapeNode* target = this;
    if (selected_part != -1) {
        target = children[selected_part];
    }
	
    Matrix* mat = &(target->Mat);

    switch (key) {
        case GLFW_KEY_UP:        mat->UpdateView('a', 0.0f,  0.1f,  0.0f, CURRENT_AXIS, 'W'); break;
        case GLFW_KEY_DOWN:      mat->UpdateView('a', 0.0f, -0.1f,  0.0f, CURRENT_AXIS, 'W'); break;
        case GLFW_KEY_RIGHT:     mat->UpdateView('a', 0.1f,  0.0f,  0.0f, CURRENT_AXIS, 'W'); break;
        case GLFW_KEY_LEFT:      mat->UpdateView('a', -0.1f, 0.0f,  0.0f, CURRENT_AXIS, 'W'); break;
        case GLFW_KEY_PAGE_UP:   mat->UpdateView('a', 0.0f,  0.0f,  0.1f, CURRENT_AXIS, 'W'); break;
        case GLFW_KEY_PAGE_DOWN: mat->UpdateView('a', 0.0f,  0.0f, -0.1f, CURRENT_AXIS, 'W'); break;
        case GLFW_KEY_D:         mat->UpdateView('d', 10.0f, 0.0f,  0.0f, CURRENT_AXIS, 'W'); break;
        case GLFW_KEY_F:         mat->UpdateView('f', 10.0f, 0.0f,  0.0f, CURRENT_AXIS, 'W'); break;
        case GLFW_KEY_G:         mat->UpdateView('g', 1.1f,  1.1f,  1.1f, CURRENT_AXIS, 'W'); break;
        case GLFW_KEY_H:         mat->UpdateView('g', 0.9f,  0.9f,  0.9f, CURRENT_AXIS, 'W'); break;
        default: break;
    }
}

void Tire::printMenu() {
    std::cout << "===================================" << std::endl;
    std::cout << "|  Simulador de Rueda (Tire)  |" << std::endl;
    std::cout << "===================================" << std::endl;
}