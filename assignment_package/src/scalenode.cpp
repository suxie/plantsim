#include "scalenode.h"

ScaleNode::ScaleNode(float x, float y, QString name)
    : Node(name), x(x), y(y)
{}

float ScaleNode::getX() {
    glm::mat3 scale = transform();
    return scale[0][0];
}

float ScaleNode::getY() {
    glm::mat3 scale = transform();
    return scale[1][1];
}

glm::mat3 ScaleNode::transform() {
    glm::mat3 scale = glm::mat3(1);
    scale[0][0] = x;
    scale[1][1] = y;
    return scale;
}

void ScaleNode::scale(float x, float y) {
    this->x = x;
    this->y = y;
}

ScaleNode::~ScaleNode() {}
