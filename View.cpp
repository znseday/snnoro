#include "View.h"

#include <GL/gl.h>
#include <GL/glu.h>

//#include <QOpenGLWidget>

#include <iostream>

using namespace std;

View::View()
{
    width = height = 0;

    specialViews = new View3d*[maxNViews()];
    for (int k = 0; k < maxNViews(); k++)
    {
        specialViews[k] = new View3d();
    }
}

View::~View()
{
    if (specialViews)
    {
        for (int k = 0; k < maxNViews(); k++)
        {
            delete specialViews[k];
        }
        delete[] specialViews;
    }
}


void View::setModel(IModel* _model)
{
    this->model = _model;
    if (specialViews)
    {
        for (int k = 0; k < maxNViews(); k++)
        {
            specialViews[k]->setModel(this->model, k);
        }
    }
}

void View::update()
{
    for (int k = 0; k < model->nSpecial(); k++)
    {
        specialViews[k]->update();
    }
}

void View::reshape(int w, int h)
{
    if (!inited) return;

    width = w, height = h;

    if (specialViews)
    {
        int n = maxNViews();
        //
        for (int k = 0; k < n; k++)
        {
            specialViews[k]->reshape(w, h);
        }
    }
}


void View::mapCoords(int X, int Y, double &_x, double &_y)
{
    double k = 1.0 / height;
    _x = k * X;
    _y = k * (height - 1 - Y);
}

void View::display()
{
    if (!inited)
        return;

    glViewport(0, 0, width, height); // Убрать - перенести в resize или типо того
    //glViewport(0, 0, 330, 330);
    //glScissor(0, 0, 330, 330);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width / (double)height, 0, 1); // Убрать - перенести в resize или типо того ???

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (model)
    {
        model->draw();
        //
        auto n = model->nSpecial();
        for (int k = 0; k < n; k++)
        {
            specialViews[k]->display();
        }
    }

    return;
}

void View::initialize()
{
    inited = true;
}


