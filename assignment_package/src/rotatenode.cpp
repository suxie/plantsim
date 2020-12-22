#include "rotatenode.h"

RotateNode::RotateNode(float angle, QString name)
    : Node(name), angle(angle)
{}

float RotateNode::getAngle() {
    glm::mat3 rotate = transform();
    return acos(rotate[0][0]) * (180/M_PI);
}

glm::mat3 RotateNode::transform() {
    glm::mat3 rotate = glm::mat3(1);
    float angleRad = this->angle * (M_PI/180);
    rotate[0][0] = cos(angleRad);
    rotate[0][1] = sin(angleRad);
    rotate[1][0] = -1 * sin(angleRad);
    rotate[1][1] = cos(angleRad);
    return rotate;
}

void RotateNode::rotate(float angle) {
    this->angle = angle;
}

RotateNode::~RotateNode() {}
