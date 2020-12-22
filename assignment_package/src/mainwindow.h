#pragma once

#include <QMainWindow>
#include <qtreewidget.h>
#include <node.h>


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionQuit_triggered();

public slots:
    void slot_getRoot(QTreeWidgetItem* root);
    void slot_receiveItemClicked(QTreeWidgetItem* curr);
    void slot_newTrans(bool clicked);
    void slot_newScale(bool clicked);
    void slot_newRot(bool clicked);
    void slot_transX(double val);
    void slot_transY(double val);
    void slot_scaleX(double val);
    void slot_scaleY(double val);
    void slot_rotAng(double val);
    void slot_velocityX(double val);
    void slot_velocityY(double val);
    void slot_accX(double val);
    void slot_accY(double val);
    void slot_axisX(double val);
    void slot_axisY(double val);
    void slot_axisZ(double val);
    void slot_angle(double val);
    void slot_branchFact(int val);
    void slot_grow(bool clicked);

private:
    Ui::MainWindow *ui;
    Node* curr;
};
