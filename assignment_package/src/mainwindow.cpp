#include "mainwindow.h"
#include <ui_mainwindow.h>
#include <node.h>
#include <scalenode.h>
#include <translatenode.h>
#include <rotatenode.h>
#include <mygl.h>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), curr(nullptr)
{
    ui->setupUi(this);
    ui->mygl->setFocus();
    connect(ui->mygl, SIGNAL(sendRoot(QTreeWidgetItem*)),
            this, SLOT(slot_getRoot(QTreeWidgetItem*)));
    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(slot_receiveItemClicked(QTreeWidgetItem*)));
    connect(ui->transNode, SIGNAL(clicked(bool)), this, SLOT(slot_newTrans(bool)));
    connect(ui->scaleNode, SIGNAL(clicked(bool)), this, SLOT(slot_newScale(bool)));
    connect(ui->rotNode, SIGNAL(clicked(bool)), this, SLOT(slot_newRot(bool)));
    connect(ui->transX, SIGNAL(valueChanged(double)), this, SLOT(slot_transX(double)));
    connect(ui->transY, SIGNAL(valueChanged(double)), this, SLOT(slot_transY(double)));
    connect(ui->scaleX, SIGNAL(valueChanged(double)), this, SLOT(slot_scaleX(double)));
    connect(ui->scaleY, SIGNAL(valueChanged(double)), this, SLOT(slot_scaleY(double)));
    connect(ui->rotAng, SIGNAL(valueChanged(double)), this, SLOT(slot_rotAng(double)));
    connect(ui->velocityX, SIGNAL(valueChanged(double)), this, SLOT(slot_velocityX(double)));
    connect(ui->velocityY, SIGNAL(valueChanged(double)), this, SLOT(slot_velocityY(double)));
    connect(ui->accX, SIGNAL(valueChanged(double)), this, SLOT(slot_accX(double)));
    connect(ui->accY, SIGNAL(valueChanged(double)), this, SLOT(slot_accY(double)));
    connect(ui->axisX, SIGNAL(valueChanged(double)), this, SLOT(slot_axisX(double)));
    connect(ui->axisY, SIGNAL(valueChanged(double)), this, SLOT(slot_axisY(double)));
    connect(ui->axisZ, SIGNAL(valueChanged(double)), this, SLOT(slot_axisZ(double)));
    connect(ui->angle, SIGNAL(valueChanged(double)), this, SLOT(slot_angle(double)));
    connect(ui->branchFactor, SIGNAL(valueChanged(int)), this, SLOT(slot_branchFact(int)));
    connect(ui->grow, SIGNAL(clicked(bool)), this, SLOT(slot_grow(bool)));
}

void MainWindow::slot_getRoot(QTreeWidgetItem* root) {
    ui->treeWidget->addTopLevelItem(root);
    ui->scaleX->setEnabled(false);
    ui->scaleY->setEnabled(false);
    ui->transX->setEnabled(false);
    ui->transY->setEnabled(false);
    ui->rotAng->setEnabled(false);
}

void MainWindow::slot_receiveItemClicked(QTreeWidgetItem* curr) {
    this->curr = (Node*)curr;
    if (dynamic_cast<TranslateNode*>(curr) != nullptr) {
        ui->transX->setEnabled(true);
        ui->transY->setEnabled(true);
        ui->transX->setValue(dynamic_cast<TranslateNode*>(curr)->getX());
        ui->transY->setValue(dynamic_cast<TranslateNode*>(curr)->getY());
        ui->scaleX->setEnabled(false);
        ui->scaleY->setEnabled(false);
        ui->rotAng->setEnabled(false);
    } else if (dynamic_cast<RotateNode*>(curr) != nullptr) {
        ui->rotAng->setEnabled(true);
        ui->rotAng->setValue(dynamic_cast<RotateNode*>(curr)->getAngle());
        ui->scaleX->setEnabled(false);
        ui->scaleY->setEnabled(false);
        ui->transX->setEnabled(false);
        ui->transY->setEnabled(false);
    } else if (dynamic_cast<ScaleNode*>(curr) != nullptr) {
        ui->scaleX->setEnabled(true);
        ui->scaleY->setEnabled(true);
        ui->scaleX->setValue(dynamic_cast<ScaleNode*>(curr)->getX());
        ui->scaleY->setValue(dynamic_cast<ScaleNode*>(curr)->getY());
        ui->transX->setEnabled(false);
        ui->transY->setEnabled(false);
        ui->rotAng->setEnabled(false);
    } else {
        ui->scaleX->setEnabled(false);
        ui->scaleY->setEnabled(false);
        ui->transX->setEnabled(false);
        ui->transY->setEnabled(false);
        ui->rotAng->setEnabled(false);
    }

    Node* node = dynamic_cast<Node*>(curr);
    if (node != nullptr) {
        ui->velocityX->setValue(node->velocity[0]);
        ui->velocityY->setValue(node->velocity[1]);
        ui->accX->setValue(node->acceleration[0]);
        ui->accY->setValue(node->acceleration[1]);
        ui->axisX->setValue(node->orient[1]);
        ui->axisY->setValue(node->orient[2]);
        ui->axisZ->setValue(node->orient[3]);
        ui->angle->setValue(node->orient[0]);
    }
}

void MainWindow::slot_newTrans(bool clicked) {
    curr->addChild(mkU<TranslateNode>(0, 0, "default_T"));
}

void MainWindow::slot_newScale(bool clicked) {
    curr->addChild(mkU<ScaleNode>(0, 0, "default_S"));
}

void MainWindow::slot_newRot(bool clicked) {
    curr->addChild(mkU<RotateNode>(0, "default_R"));
}

void MainWindow::slot_transX(double val) {
    TranslateNode* T = dynamic_cast<TranslateNode*>(curr);
    T->trans(val, T->getY());
}

void MainWindow::slot_transY(double val) {
    TranslateNode* T = dynamic_cast<TranslateNode*>(curr);
    T->trans(T->getX(), val);
}

void MainWindow::slot_scaleX(double val) {
    ScaleNode* T = dynamic_cast<ScaleNode*>(curr);
    dynamic_cast<ScaleNode*>(curr)->scale(val, T->getY());
}

void MainWindow::slot_scaleY(double val) {
    ScaleNode* T = dynamic_cast<ScaleNode*>(curr);
    dynamic_cast<ScaleNode*>(curr)->scale(T->getX(), val);
}

void MainWindow::slot_rotAng(double val) {
    dynamic_cast<RotateNode*>(curr)->rotate(val);
}

void MainWindow::slot_velocityX(double val) {
    dynamic_cast<Node*>(curr)->velocity[0] = val;
}

void MainWindow::slot_velocityY(double val) {
    dynamic_cast<Node*>(curr)->velocity[1] = val;
}

void MainWindow::slot_accX(double val) {
    dynamic_cast<Node*>(curr)->acceleration[0] = val;
}

void MainWindow::slot_accY(double val) {
    dynamic_cast<Node*>(curr)->acceleration[1] = val;
}

void MainWindow::slot_axisX(double val) {
    dynamic_cast<Node*>(curr)->orient[1] = val;
}

void MainWindow::slot_axisY(double val) {
    dynamic_cast<Node*>(curr)->orient[2] = val;
}

void MainWindow::slot_axisZ(double val) {
    dynamic_cast<Node*>(curr)->orient[3] = val;
}

void MainWindow::slot_angle(double val) {
    dynamic_cast<Node*>(curr)->orient[0] = val;
}

void MainWindow::slot_branchFact(int val) {
    ui->mygl->branch = val;
}

void MainWindow::slot_grow(bool clicked) {
    ui->mygl->grow(curr);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}
