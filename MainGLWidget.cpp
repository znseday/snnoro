#include "MainGLWidget.h"

#include <QApplication>

#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions>
#include <QDebug>
#include <QOpenGLFramebufferObject>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include <QPainter>
#include <QGraphicsScene>
#include <QOpenGLTexture>

#include <iostream>



using namespace std;

MainGLWidget::MainGLWidget(WorkModeType &_workMode, MyGradModel &_gradModel, WorldModeType &_worldMode, QWidget *parent)
    : QOpenGLWidget(parent), WorkMode(_workMode), WorldMode(_worldMode), GradModel(_gradModel)
{
//    MainTimer = new QTimer(this);
}
//----------------------------------------------------------

//void MainGLWidget::StartMainTimer()
//{
//    MainTimer->start(17);
//}
//----------------------------------------------------------

void MainGLWidget::Repaint()
{
    this->repaint();
}
//----------------------------------------------------------

//void MainGLWidget::slotReceiveMainTimerTimeout()
//{
//    if (!View::Instance().IsInit())
//        return;
//    if (!Model::Instance().IsInited())
//        return;

//    Model::Instance().dynamics();

//    this->repaint();
//}
//----------------------------------------------------------

bool MainGLWidget::MouseToWorld(int clientX, int clientY, GLdouble &_worldX, GLdouble &_worldY, GLdouble &_worldZ)
{
    int X = clientX;
    int Y = this->height() - 1 - clientY;

    GLfloat depth;

    this->makeCurrent();

    MyFBO->bind();
    glReadPixels(X, Y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    MyFBO->release();

    GLint vport[4];
    GLdouble modl[16], proj[16];

    if (GradModel.IsDrawOnlyOne())
    {
        glGetIntegerv(GL_VIEWPORT, vport);
        glGetDoublev(GL_MODELVIEW_MATRIX, modl);
        glGetDoublev(GL_PROJECTION_MATRIX, proj);
    }
    else
    {
        int iCurViewPort = GradModel.CalcCurViewPortNumber(clientX, clientY);
        if (iCurViewPort < 0)
            return false;

        //qDebug() << "iCurViewPort =" << iCurViewPort;

        //qDebug() << vport[0] << vport[1] << vport[2] << vport[3];


//        if (iCurViewPort >= (int)GradModel.GetConfigs().size())
//            qDebug() << "!!!!!!!!!!!!!!!";

        GradModel.GetConfigs().at(iCurViewPort).FillExternVportModlAndProj(vport, modl, proj);

    }

    this->doneCurrent();

    //qDebug() << "depth =" << depth;

    if ( qFuzzyCompare(depth, 1) )
    {
        return false;
    }

    if ( gluUnProject(X, Y, depth, modl, proj, vport, &_worldX, &_worldY, &_worldZ) == GL_TRUE )
        return true;
    else
        return false;
}
//----------------------------------------------------------

void MainGLWidget::initializeGL()
{
    //InitGL();

    glEnable(GL_DEPTH_TEST);

}
//----------------------------------------------------------

void MainGLWidget::resizeGL(int w, int h)
{
    if (h == 0) h = 1;
    AspectRatio = static_cast<double>(w) / static_cast<double>(h);

    if (MyFBO)
        delete MyFBO;

    QOpenGLFramebufferObjectFormat format;
    format.setSamples(0);
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    MyFBO = new QOpenGLFramebufferObject(QSize(width(), height()), format);

    if (WorkMode == WorkModeType::GradWork)
    {
//        if (MyFBO)
//            delete MyFBO;

//        QOpenGLFramebufferObjectFormat format;
//        format.setSamples(0);
//        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
//        MyFBO = new QOpenGLFramebufferObject(QSize(width(), height()), format);

        GradModel.OnResize(this->width(), this->height());
    }
}
//----------------------------------------------------------

void MainGLWidget::paintGL()
{
//    QPainter painter(this); // Наличие QPainter вырубает работу мультисэмплинга(((
//    //painter.draw();
//    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing); // Этот антиалайзинг работает, но мультисэмплинг нет
//    painter.beginNativePainting();


    if (WorkMode == WorkModeType::Nothing)
        return;

    if (WorkMode == WorkModeType::GradWork)
    {
//        GradModel.OnResize(this->width(), this->height());

        GradModel.DrawMeIn3D();

        QOpenGLContext *ctx = QOpenGLContext::currentContext();
        ctx->extraFunctions()->glBindFramebuffer(GL_READ_FRAMEBUFFER, this->defaultFramebufferObject());
        ctx->extraFunctions()->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, MyFBO->handle());
        ctx->extraFunctions()->glBlitFramebuffer(0, 0, width(), height(), 0, 0, MyFBO->width(), MyFBO->height(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);

//        cout << __PRETTY_FUNCTION__ << endl;
    }


//    QPainter painter(this);

//    painter.endNativePainting();

//    painter.drawLine(100, 100, 300, 300);
//    painter.drawText(10, 30, "Hellow");

//    painter.end();
}
//----------------------------------------------------------

void MainGLWidget::slotReceiveRepaintSignal()
{
    this->repaint(); // тестировать
}
//----------------------------------------------------------

void MainGLWidget::keyPressEvent(QKeyEvent *pe)
{
    if (WorkMode == WorkModeType::Nothing)
        return;

    if (WorkMode == WorkModeType::GradWork)
    {
        GradModel.OnKeyPress(pe);
    }

    this->repaint(); // тестировать!
}
//----------------------------------------------------------

void MainGLWidget::keyReleaseEvent([[maybe_unused]] QKeyEvent *pe)
{
    if (WorkMode == WorkModeType::Nothing)
        return;
}
//----------------------------------------------------------

void MainGLWidget::mousePressEvent(QMouseEvent *pe)
{
    if (WorkMode == WorkModeType::Nothing)
        return;

    if (WorkMode == WorkModeType::GradWork)
    {
        if (pe->buttons() & Qt::LeftButton)
        {
            GradModel.OnMousePress(pe);

            emit Signal_iCurConfigChanged(GradModel.Get_iCurConfig());
        }

        double wx, wy, wz;
        bool wExists = MouseToWorld(pe->pos().x(), pe->pos().y(), wx, wy, wz);

        if (WorldMode == WorldModeType::AddingRoutePoints &&
            pe->buttons() & Qt::RightButton)
        {
//            double wx, wy, wz;
//            bool wExists = MouseToWorld(pe->pos().x(), pe->pos().y(), wx, wy, wz);
            if (wExists)
            {
                GradModel.AddNewPointToLastRoute(wx, wy);
            }
        }

        if (WorldMode == WorldModeType::DeletingRoute &&
            pe->buttons() & Qt::RightButton) // было middle
        {
//            double wx, wy, wz;
//            bool wExists = MouseToWorld(pe->pos().x(), pe->pos().y(), wx, wy, wz);
            if (wExists)
            {
                emit SignalRouteDeleted( GradModel.DeleteRoute(wx, wy) );
            }
        }

        if (WorldMode == WorldModeType::SelectingSignalNode &&
            pe->buttons() & Qt::LeftButton)
        {
            if (wExists)
            {
                GradModel.SelectCurNodeByPos(wx, wy);

                WorldMode = WorldModeType::Nothing;
            }
        }

        if (WorldMode == WorldModeType::EditingPosSignalNode &&
            pe->buttons() & Qt::LeftButton)
        {
            if (wExists)
            {
                GradModel.PutCurNodeByPos(wx, wy);
//                WorldMode = WorldModeType::Nothing;
            }
        }

        if (WorldMode == WorldModeType::EditingAngleSignalNode &&
            pe->buttons() & Qt::LeftButton)
        {
            if (wExists)
            {
                GradModel.SetDirectCurNodeByPos(wx, wy);
//                WorldMode = WorldModeType::Nothing;
            }
        }

        if (pe->type() == QMouseEvent::MouseButtonDblClick &&
                GradModel.Get_iCurConfig() >= 0 &&
                WorldMode == WorldModeType::Nothing)
        {
            GradModel.SwitchDrawOnlyOne();
        }

        this->repaint();
    }
}
//----------------------------------------------------------

void MainGLWidget::mouseMoveEvent(QMouseEvent *pe)
{
    if (WorkMode == WorkModeType::Nothing)
        return;

    if (WorkMode == WorkModeType::GradWork)
    {
        if (IsShowCoordsAlways || WorldMode != WorldModeType::Nothing)
        {
            double wx = -1, wy = -1, wz = -1;
            bool wExists = MouseToWorld(pe->pos().x(), pe->pos().y(), wx, wy, wz);
        //    wExists = Model.MouseToWorld(pe->x(), pe->y(), this->height(), wx, wy, wz);
        //    auto tEnd = ClockType::now();
        //    auto t = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(tEnd - tStart).count();
        //    t /= 1.0e6;
        //    qDebug() << "t mouse move get depth (mili) =" << t;
            emit SignalSendWorldCoords(wx, wy, wz, wExists);
        }

        if (pe->buttons() & Qt::LeftButton)
        {
            GradModel.OnMouseMove(pe);
            this->repaint();
        }
    }
}
//----------------------------------------------------------

void MainGLWidget::mouseReleaseEvent([[maybe_unused]] QMouseEvent *pe)
{

}
//----------------------------------------------------------

void MainGLWidget::wheelEvent(QWheelEvent *pe)
{
    if (WorkMode == WorkModeType::Nothing)
        return;

    if (WorkMode == WorkModeType::GradWork)
    {
        GradModel.OnMouseWheel(pe);
        this->repaint();
    }
}
