#ifndef SCALENODE_H
#define SCALENODE_H
#include <node.h>

class ScaleNode : public Node
{
protected:
    float x;
    float y;
public:
    ScaleNode(float x, float y, QString name);
    float getX();
    float getY();
    void scale(float x, float y);
    glm::mat3 transform() override;
    virtual ~ScaleNode();
};

#endif // SCALENODE_H
