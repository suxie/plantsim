#ifndef NODE_H
#define NODE_H
#include <smartpointerhelp.h>
#include <vector>
#include <drawable.h>
#include "node.h"
#include <QString>
#include <qtreewidget.h>

class Node : public QTreeWidgetItem
{
protected:
    Drawable* polygon;
    glm::vec3 color;
    QString name;

public:
    std::vector<uPtr<Node>> children;
    std::vector<float> velocity;
    std::vector<float> orient;
    std::vector<float> acceleration;
    Node* parent;
    Node(QString name);
    Drawable* getPolygon();
    void setPolygon(Drawable *p);
    void setColor(glm::vec3 color);
    const glm::vec3 getColor();
    const QString getName();

    virtual glm::mat3 transform() = 0;

    Node& addChild(uPtr<Node> n);
    void changeColor(glm::vec3 color);

    Node& operator=(Node &n);
    Node(const Node &n);
    virtual ~Node();
};

#endif // NODE_H
