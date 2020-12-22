#ifndef ROTATEENODE_H
#define ROTATENODE_H
#include <node.h>

class RotateNode : public Node
{
protected:
    float angle;
public:
    RotateNode(float angle, QString name);
    float getAngle();
    glm::mat3 transform() override;
    void rotate(float angle);
    virtual ~RotateNode();
};

#endif // ROTATENODE_H
