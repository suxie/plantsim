#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <node.h>
#include <translatenode.h>
#include <rotatenode.h>
#include <scalenode.h>


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      prog_flat(this),
      m_geomGrid(this), m_geomSquare(this, 10),
      m_showGrid(true),
      root(nullptr),
      apexes(std::vector<Node*>())
{
    setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();

    glDeleteVertexArrays(1, &vao);
    m_geomSquare.destroy();
    m_geomGrid.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
//    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the scene geometry
    m_geomGrid.create();
    m_geomSquare.create();

    // Create and set up the flat lighting shader
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);

    this->root = std::move(constructSceneGraph());
    emit sendRoot(root.get());
}

uPtr<Node> MyGL::constructSceneGraph() {
    uPtr<Node> root = mkU<TranslateNode>(3.f, -4.f, "root_T");
    root->setColor(glm::vec3(0.22, 0.42, 0.52));
    root->setPolygon(&m_geomSquare);
    return root;
}

void MyGL::traverse(const uPtr<Node>& n, glm::mat3 t) {
    t = t * (*n).transform();
    for(const uPtr<Node>& node : n->children) {
        traverse(node, t);
    }
    if ((*n).getPolygon() != nullptr) {
        Polygon2D* poly = static_cast<Polygon2D*>((*n).getPolygon());
        (*poly).setColor(n->getColor());
        prog_flat.setModelMatrix(t);
        prog_flat.draw(*this, *poly);
    }
}

void MyGL::grow(QTreeWidgetItem* widget) {
    Node* node = static_cast<Node*>(widget);

    node->addChild(growHelper(node));

    std::vector<Node*> newApexes = std::vector<Node*>();
    for (Node* apex : apexes) {
        apex->addChild(growHelper(apex));
        Node* apexNew = dynamic_cast<Node*>(apex->child(apex->childCount() - 1));
        newApexes.push_back(apexNew); //* don't think apexes are being handled correctly
    }
    apexes = newApexes;

    count++;
    if (branch > 0) {
        if (count % branch == 0) {
            if ((count / branch) % 2 == 0) {
                node->acceleration[0] += 1.f;
            } else {
                node->acceleration[0] -= 1.f;
            }
            apexes.push_back(node);
        }
    }
}

uPtr<TranslateNode> MyGL::growHelper(Node* node) {
    float time = 0.1;

    // UPDATE ORIENT
    // find norm of acceleration vec
    float accNorm = sqrt(node->acceleration[0] * node->acceleration[0] +
            node->acceleration[1] * node->acceleration[1] +
            node->acceleration[2] * node->acceleration[2]);

    // calculate sin(|w|t / 2), cos(|w|t / 2)
    float accY = sin((accNorm * time) / 2.f);
    float accX = cos((accNorm * time) / 2.f);

    // w' * sin(|w|t / 2)
    std::vector<float> eulerQuat = std::vector<float>{accX,
                                                      node->acceleration[0] * accY,
                                                      node->acceleration[1] * accY,
                                                      node->acceleration[2] * accY};

    float sa = eulerQuat[0];
    float xa = eulerQuat[1];
    float ya = eulerQuat[2];
    float za = eulerQuat[3];

    float sb = node->orient[0];
    float xb = node->orient[1];
    float yb = node->orient[2];
    float zb = node->orient[3];

    // qp = (w' sin(|w|t / 2), cos(|w|t)/2) q
    std::vector<float> qp = std::vector<float>{sa * sb - xa * xb - ya * yb - za * zb,
                                               sa * xb + sb * xa + ya * zb - yb * za,
                                               sa * yb + sb * ya + za * xb - zb * xa,
                                               sa * zb + sb * za + xa * yb - xb * ya};

    // UPDATE ACCELERATION
    // 1/||q||^2
    float normInv = 1.f / (node->orient[0] * node->orient[0]
            + node->orient[1] * node->orient[1]
            + node->orient[2] * node->orient[2]
            + node->orient[3] * node->orient[3]);

    // calculate inverse - 1/||q||^2 [s, v]T
    std::vector<float> orientInv = std::vector<float>{normInv * node->orient[0],
                                                      normInv * -1 * node->orient[1],
                                                      normInv * -1 * node->orient[2],
                                                      normInv * -1 * node->orient[3]};
    sa = qp[0];
    xa = qp[1];
    ya = qp[2];
    za = qp[3];

    sb = orientInv[0];
    xb = orientInv[1];
    yb = orientInv[2];
    zb = orientInv[3];

    // qp * q-1
    std::vector<float> qqp = {sa * sb - xa * xb - ya * yb - za * zb,
                               sa * xb + sb * xa + ya * zb - yb * za,
                               sa * yb + sb * ya + za * xb - zb * xa,
                               sa * zb + sb * za + xa * yb - xb * ya};
    // find angle from quat
    float accpAngle = 2.f * acos(qqp[0]);

    // find axis
    std::vector<float> accpAxis = std::vector<float>{qqp[1], qqp[2], qqp[3]};
    float accpAxisNorm = sqrt(accpAxis[0] * accpAxis[0] +
                              accpAxis[1] * accpAxis[1] +
                              accpAxis[2] * accpAxis[2]);
    accpAxis[0] = accpAxis[0] / accpAxisNorm;
    accpAxis[1] = accpAxis[1] / accpAxisNorm;
    accpAxis[2] = accpAxis[2] / accpAxisNorm;
    std::vector<float> accp = std::vector<float>{accpAngle * accpAxis[0] / time,
                                                 accpAngle * accpAxis[1] / time,
                                                 accpAngle * accpAxis[2] / time};

    // calculate new positions; adjust based on acceleration
    float posX = node->velocity[0] * time + ((9.8 * node->acceleration[0]) * time * time) / 2.0;
    float posY = node->velocity[1] * time + ((9.8 * node->acceleration[1]) * time * time) / 2.0;

    // adjust based on orientation
    double w = qqp[0];
    double x = qqp[1];
    double y = qqp[2];
    double z = qqp[3];

    double m00 = 1.f - 2.f * y * y - 2.f * z * z;
    double m10 = 2.f * x * y + 2.f * w * z;
    double m20 = 2.f * x * z - 2.f * w * y;

    double m01 = 2.f * x * y - 2.f * w * z;
    double m11 = 1.f - 2.f * x * x - 2.f * z * z;
    double m21 = 2.f * y * z + 2.f * w * x;

    double m02 = 2.f * x * z + 2.f * w * y;
    double m12 = 2.f * y * z - 2.f * w * x;
    double m22 = 1.f - 2.f * x * x - 2.f * y * y;

    posX = (m00 * posX + m10 * posY + m20) / (m02 * posX + m12 * posY + m22);
    posX = (m01 * posX + m11 * posY + m21) / (m02 * posX + m12 * posY + m22);

    uPtr<TranslateNode> trans = mkU<TranslateNode>(posX, posY, "default_T");
    trans->setPolygon(&m_geomSquare);
    trans->velocity = std::vector<float>{posX / time, posY / time, 0.f};
    trans->orient = qp;
    trans->acceleration = accp;

    return std::move(trans);
}

void MyGL::resizeGL(int w, int h)
{
    glm::mat3 viewMat = glm::scale(glm::mat3(), glm::vec2(0.2, 0.2)); // Screen is -5 to 5

    // Upload the view matrix to our shader (i.e. onto the graphics card)
    prog_flat.setViewMatrix(viewMat);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function updateGL is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_showGrid)
    {
        prog_flat.setModelMatrix(glm::mat3());
        prog_flat.draw(*this, m_geomGrid);
    }

    traverse(this->root, glm::mat3(1));


//    //VVV CLEAR THIS CODE WHEN YOU IMPLEMENT SCENE GRAPH TRAVERSAL VVV///////////////////

//    // Shapes will be drawn on top of one another, so the last object
//    // drawn will appear in front of everything else

//    prog_flat.setModelMatrix(glm::mat3());
//    m_geomSquare.setColor(glm::vec3(0,1,0));
//    prog_flat.draw(*this, m_geomSquare);

//    m_geomSquare.setColor(glm::vec3(1,0,0));
//    prog_flat.setModelMatrix(glm::translate(glm::mat3(), glm::vec2(-1.f, 0.f)) * glm::rotate(glm::mat3(), glm::radians(-30.f)));
//    prog_flat.draw(*this, m_geomSquare);

//    m_geomSquare.setColor(glm::vec3(0,0,1));
//    prog_flat.setModelMatrix(glm::translate(glm::mat3(), glm::vec2(1.f, 0.f)) * glm::rotate(glm::mat3(), glm::radians(30.f)));
//    prog_flat.draw(*this, m_geomSquare);

//    //^^^ CLEAR THIS CODE WHEN YOU IMPLEMENT SCENE GRAPH TRAVERSAL ^^^/////////////////

//    // Here is a good spot to call your scene graph traversal function.
//    // Any time you want to draw an instance of geometry, call
//    // prog_flat.draw(*this, yourNonPointerGeometry);
}

void MyGL::keyPressEvent(QKeyEvent *e)
{
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    switch(e->key())
    {
    case(Qt::Key_Escape):
        QApplication::quit();
        break;

    case(Qt::Key_G):
        m_showGrid = !m_showGrid;
        break;
    }
}
