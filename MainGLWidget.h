#ifndef MAINGLWIDGET_H
#define MAINGLWIDGET_H

#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>

#include <QOpenGLFramebufferObject>
//#include <QOpenGLFunctions>

#include "Types.h"
#include "GradModel.h"

class MainGLWidget : public QOpenGLWidget, public IGradDrawable
{
    Q_OBJECT

private:

    QTimer *MainTimer;

    double AspectRatio = 1;

    int OldX = 0, CurrentX = 0;
    int OldY = 0, CurrentY = 0;

    WorkModeType &WorkMode;
    WorldModeType &WorldMode;

    MyGradModel &GradModel;

    bool IsShowCoordsAlways = false;

    QOpenGLFramebufferObject *MyFBO = nullptr;

//    double wx = 0, wy = 0, wz = 0; // ?
//    bool wExists = false;          // ?

//    GLint vport[4];
//    GLdouble modl[16], proj[16];

    bool MouseToWorld(int clientX, int clientY,
                      GLdouble &_worldX, GLdouble &_worldY, GLdouble &_worldZ);

protected:

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void keyPressEvent(QKeyEvent *pe) override;
    void keyReleaseEvent(QKeyEvent *pe) override;

    void mousePressEvent(QMouseEvent *pe) override;
    void mouseMoveEvent(QMouseEvent *pe) override;
    void mouseReleaseEvent(QMouseEvent *pe) override;

    void wheelEvent(QWheelEvent *pe) override;

public:
    MainGLWidget(WorkModeType &_workMode, MyGradModel &_gradModel, WorldModeType &_worldMode, QWidget *parent = nullptr);
    //~MainGLWidget() override;

    void StartMainTimer();
    virtual void Repaint() override;

    void SetIsShowCoordsAlways(bool _b) {IsShowCoordsAlways = _b;}
    bool GetIsShowCoordsAlways() const {return IsShowCoordsAlways;}

public slots:

    void slotReceiveRepaintSignal();

    void slotReceiveMainTimerTimeout();

signals:
    void SignalSendWorldCoords(double wx, double wy, double wz, bool wExists);
    void SignalRouteDeleted(bool);
    void Signal_iCurConfigChanged(int);

};

#endif // MAINGLWIDGET_H
