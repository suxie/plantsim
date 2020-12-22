#ifndef TRANSLATENODE_H
#define TRANSLATENODE_H
#include <node.h>

class TranslateNode : public Node
{
protected:
    float x;
    float y;
public:
    TranslateNode(float x, float y, QString name);
    float getX();
    float getY();
    void trans(float x, float y);
    glm::mat3 transform() override;
    virtual ~TranslateNode();
};

#endif // TRANSLATENODE_H
