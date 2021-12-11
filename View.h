#pragma once

#include "IView.h"
#include "IModel.h"
#include "View3d.h"

class View : public IView
{
private:

	int width, height;
    IModel* model = nullptr;

	View3d** specialViews = nullptr;

    int maxNViews() const { return 4; }

    bool inited = false;

public:

//    int test;

    View();

    ~View();

    void setModel(IModel *_model);

    void update();

    void reshape(int w, int h);

    void mapCoords(int X, int Y, double &_x, double &_y);

    void display();

    void initialize();

    bool IsInit() {return inited;}

    static View & Instance()
    {
        static View s_View;
        return s_View;
    }

};
