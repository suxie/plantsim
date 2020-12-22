#include "translatenode.h"

TranslateNode::TranslateNode(float x, float y, QString name)
    : Node(name), x(x), y(y)
{}

float TranslateNode::getX() {
    glm::mat3 trans = transform();
    return trans[2][0];
}

float TranslateNode::getY() {
    glm::mat3 trans = transform();
    return trans[2][1];
}

void TranslateNode::trans(float x, float y) {
    this->x = x;
    this->y = y;
}

glm::mat3 TranslateNode::transform() {
    glm::mat3 translate = glm::mat3(1);
    translate[2][0] = x;
    translate[2][1] = y;
    return translate;
}

TranslateNode::~TranslateNode() {}
