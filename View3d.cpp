#include "View3d.h"

#include <GL/gl.h>
#include <GL/glu.h>

View3d::View3d()
{
    initialize(0, 0, 0, 0);
}

void View3d::setModel(IModel* _model, int _viewId)
{
    this->model  = _model;
    this->viewId = _viewId;
}

void View3d::initialize(double x0_, double y0_, double w0_, double h0_)
{
    if (inited)
        return;

    inited = true;

    (void)x0_;  (void)y0_;  (void)w0_;  (void)h0_;
}

void View3d::update() // (double x0_, double y0_, double w0_, double h0_)
{
    model->getSpecialViewParams(viewId, x0, y0, w0, h0);
}


void View3d::reshape(int w, int h)
{
    if (!inited)
        return;

    width = w, height = h;
}

void View3d::setupCamera()
{
    glViewport(x0 * height, y0 * height, w0*height, h0*height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(60, height / (double)width, 100, 10000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void View3d::display()
{
    if (!inited)
        return;

    update();

    setupCamera();

    //glClearColor(0, 0.3, 0.3, 1);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (model)
    {
        model->drawSpecial(this->viewId);
    }

    // glutSwapBuffers();

    return;
}



