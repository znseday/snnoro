#include "MainGLWidget.h"

#include <QApplication>

#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions>
#include <QDebug>

#include "View.h"
#include "Model.h"


#include <iostream>

using namespace std;

MainGLWidget::MainGLWidget(WorkModeType &_workMode, MyGradModel &_gradModel, WorldModeType &_worldMode, QWidget *parent)
    : QOpenGLWidget(parent), WorkMode(_workMode), WorldMode(_worldMode), GradModel(_gradModel)
{
    MainTimer = new QTimer(this);
    connect(MainTimer, SIGNAL(timeout()),
                 this, SLOT(slotReceiveMainTimerTimeout()));
}
//----------------------------------------------------------

void MainGLWidget::StartMainTimer()
{
    MainTimer->start(17);
}
//----------------------------------------------------------

void MainGLWidget::Repaint()
{
    this->repaint();
}
//----------------------------------------------------------

void MainGLWidget::slotReceiveMainTimerTimeout()
{
    if (!View::Instance().IsInit())
        return;
    if (!Model::Instance().IsInited())
        return;

    Model::Instance().dynamics();

    this->repaint();
}
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

//        const QRect & rect = GradModel.GetViewPorts().at(iCurViewPort);
//        int x = rect.left();
//        int y = rect.top();

//        vport[0] = x;
//        vport[1] = this->height() - 1 - y - rect.height();
//        vport[2] = rect.width();
//        vport[3] = rect.height();

        //qDebug() << vport[0] << vport[1] << vport[2] << vport[3];

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

    if (WorkMode == WorkModeType::OldWork)
    {
        if (!View::Instance().IsInit())
            return;
        if (!Model::Instance().IsInited())
            return;

        View::Instance().reshape(w, h);
    }
    else if (WorkMode == WorkModeType::GradWord)
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
    if (WorkMode == WorkModeType::Nothing)
        return;

    if (WorkMode == WorkModeType::OldWork)
    {
        if (!View::Instance().IsInit())
            return;
        if (!Model::Instance().IsInited())
            return;
        //cout << __PRETTY_FUNCTION__ << endl;
        View::Instance().display();
    }
    else if (WorkMode == WorkModeType::GradWord)
    {
//        GradModel.OnResize(this->width(), this->height());

        GradModel.DrawMeIn3D();

        QOpenGLContext *ctx = QOpenGLContext::currentContext();
        ctx->extraFunctions()->glBindFramebuffer(GL_READ_FRAMEBUFFER, this->defaultFramebufferObject());
        ctx->extraFunctions()->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, MyFBO->handle());
        ctx->extraFunctions()->glBlitFramebuffer(0, 0, width(), height(), 0, 0, MyFBO->width(), MyFBO->height(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        //glGetIntegerv(GL_VIEWPORT, vport);
//        glGetDoublev(GL_MODELVIEW_MATRIX, modl);
//        glGetDoublev(GL_PROJECTION_MATRIX, proj);

        cout << __PRETTY_FUNCTION__ << endl;
    }
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

    if (WorkMode == WorkModeType::OldWork)
    {
        if (!View::Instance().IsInit())
            return;

        switch (pe->key())
        {
        case Qt::Key_Up:
            Model::Instance().beta += 7.5;
            break;
        case Qt::Key_Down:
            Model::Instance().beta -= 7.5;
            break;
        case Qt::Key_Left:
            Model::Instance().alpha += 5;
            break;
        case Qt::Key_Right:
            Model::Instance().alpha -= 5;
            break;
        case Qt::Key_F:
//            if (Model::Instance().nCView > 1)
//            {
//                Model::Instance().nCView = 1;
//            }
//            else // 1
//            {
//                Model::Instance().nCView = 4;
//            }

            break;

        case Qt::Key_P:
        //    Model::Instance().fDrawCfg3d = !Model::Instance().fDrawCfg3d;
            break;

        case Qt::Key_Escape:

            break;
        }
    }
    else if (WorkMode == WorkModeType::GradWord)
    {
        GradModel.OnKeyPress(pe);
    }

    this->repaint(); // тестировать!
}
//----------------------------------------------------------

void MainGLWidget::keyReleaseEvent(QKeyEvent *pe)
{
    if (WorkMode == WorkModeType::Nothing)
        return;

    if (WorkMode == WorkModeType::OldWork)
    {
        if (!View::Instance().IsInit())
            return;
        switch(pe->key())
        {
        case Qt::Key_Up:
            break;
        case Qt::Key_Down:
            break;
        case Qt::Key_Left:
            break;
        case Qt::Key_Right:
            break;
        }
    }
}
//----------------------------------------------------------

void MainGLWidget::mousePressEvent(QMouseEvent *pe)
{
    if (WorkMode == WorkModeType::Nothing)
        return;

    if (WorkMode == WorkModeType::OldWork)
    {
        if (!View::Instance().IsInit())
            return;

        this->setFocus();

    //    OldX = pe->x();
    //    OldY = pe->y();

        double x, y;
        View::Instance().mapCoords(pe->pos().x(), pe->pos().y(), x, y);
        bool IsUpdated = Model::Instance().onMouseClick(x, y);
        if (IsUpdated)
            this->repaint();
    }
    else if (WorkMode == WorkModeType::GradWord)
    {
        if (pe->buttons() & Qt::LeftButton)
        {
            GradModel.OnMousePress(pe);
        }

        if (WorldMode == WorldModeType::AddingRoutePoints &&
            pe->buttons() & Qt::RightButton)
        {
            double wx, wy, wz;
            bool wExists = MouseToWorld(pe->pos().x(), pe->pos().y(), wx, wy, wz);
            if (wExists)
            {
                GradModel.AddNewPointToLastRoute(wx, wy);
            }
        }

        if (WorldMode == WorldModeType::DeletingRoute &&
            pe->buttons() & Qt::MiddleButton)
        {
            double wx, wy, wz;
            bool wExists = MouseToWorld(pe->pos().x(), pe->pos().y(), wx, wy, wz);
            if (wExists)
            {
//                if ( GradModel.DeleteRoute(wx, wy) )
//                    emit SignalRouteDeleted();
//                else
//                    throw std::runtime_error("GradModel.DeleteRoute(wx, wy) == false");

                emit SignalRouteDeleted( GradModel.DeleteRoute(wx, wy) );
            }
        }

        this->repaint();
    }
}
//----------------------------------------------------------

void MainGLWidget::mouseMoveEvent(QMouseEvent *pe)
{
    if (WorkMode == WorkModeType::Nothing)
        return;

    if (WorkMode == WorkModeType::OldWork)
    {
        if (!View::Instance().IsInit())
            return;

    //    CurrentX = pe->x();
    //    CurrentY = pe->y();

    //    int dx = CurrentX - OldX;
    //    int dy = CurrentY - OldY;

        double x, y;
        View::Instance().mapCoords(pe->pos().x(), pe->pos().y(), x, y);
        bool IsUpdated = Model::Instance().onMouseMove(x, y);
        if (IsUpdated)
            this->repaint(); // тестировать!

        if (QApplication::keyboardModifiers() == Qt::ControlModifier) // Отследить состояние клавиши Ctrl
        {
        }
        else if (QApplication::keyboardModifiers() == Qt::AltModifier)
        {
        }
        else
        {
        }

        OldX = CurrentX;
        OldY = CurrentY;
    }
    else if (WorkMode == WorkModeType::GradWord)
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

    if (WorkMode == WorkModeType::OldWork)
    {

    }
    else if (WorkMode == WorkModeType::GradWord)
    {
        GradModel.OnMouseWheel(pe);
        this->repaint();
    }
}
