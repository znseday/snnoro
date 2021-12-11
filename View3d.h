#pragma once

#include "IView.h"
#include "IModel.h"

class View3d : public IView
{
private:
    bool inited = false;

	double x0, y0, w0, h0;
    int viewId = 0, width = 0, height = 0;
    IModel* model = nullptr;

public:
    View3d();

    void setModel(IModel* _model, int _viewId);

    void initialize(double x0_, double y0_, double w0_, double h0_);

    void update(); // (double x0_, double y0_, double w0_, double h0_)

    void reshape(int w, int h);

    void setupCamera();

    void display();
};
