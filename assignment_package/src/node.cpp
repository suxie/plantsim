#include <smartpointerhelp.h>
#include <vector>
#include <drawable.h>
#include "node.h"
#include <QString>

Node::Node(QString name)
    : QTreeWidgetItem(), polygon(nullptr), color(glm::vec3()),
      name(name),
      children(std::vector<uPtr<Node>>()),
      parent(nullptr),
      velocity(std::vector<float>{2.f, 1.2f, 0.f}),
      orient(std::vector<float>{1.5f, 0.5f, 1.f, 1.f}),
      acceleration(std::vector<float>{5.5f, 0.8f, 0.f})
{
    this->setText(0, name);
}

Drawable* Node::getPolygon() {
    return this->polygon;
}

void Node::setPolygon(Drawable* p) {
    this->polygon = p;
}

void Node::setColor(glm::vec3 color) {
    this->color = color;
}

const glm::vec3 Node::getColor() {
    return this->color;
}

const QString Node::getName() {
    return this->name;
}

Node& Node::addChild(uPtr<Node> n) {
    Node& child = *n;
    this->children.push_back(std::move(n));
    QTreeWidgetItem::addChild(&child);
    return child;
}

void Node::changeColor(glm::vec3 color) {
    this->color = color;
}

Node& Node::operator=(Node &n) {
    this->polygon = n.getPolygon();
    this->color = n.getColor();
    this->name = n.getName();
    this->children = std::vector<uPtr<Node>>();
    return *this;
}

Node::Node(const Node &n)
    : QTreeWidgetItem(), polygon(n.polygon), color(n.color),
      name(n.name), children(std::vector<uPtr<Node>>())
{
    this->setText(0, name);
}

Node::~Node() {}
