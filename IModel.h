#pragma once

struct IModel
{
	virtual void draw() const = 0;
	virtual void drawSpecial(int k) const = 0;
	virtual int nSpecial() const = 0;
    virtual void getSpecialViewParams(int k, double &x0, double &y0, double &w, double &h) const = 0;

    virtual ~IModel(){};
};
