#pragma once

#include "MathUtil.h"


#include <QVector2D>
#include <QVector3D>

class Sota
{
public:
    QVector2D p0;
	double R;

public:

	Sota() = delete;

    Sota(const QVector2D & p0_, double R_) : p0(p0_), R(R_){}

    Sota(const Sota &other) = default; // Использует по дефолту, т.к. тривиально копируемый тип

    Sota & operator=(const Sota &) = default; // Использует по дефолту, т.к. тривиально копируемый тип

    Sota(Sota &&) noexcept = delete;             // удаляем т.к. в проекте не предусмотрена семантика перемещения
    Sota & operator=(Sota &&) noexcept = delete; // удаляем т.к. в проекте не предусмотрена семантика перемещения

    double accessRateF(const QVector3D &p) const
	{
		double d2 = pow(p.x() - p0.x(), 2) + pow(p.y() - p0.y(), 2); // это квадрат расстояния между точками p0 и p
		double y = exp(-d2/(2*R*R));

        // z НЕ учитывается!!!!!!!!!!!!!!!!!!!
		return y;
	}

    QVector2D accessGradF(const QVector3D &p, double *y_ = nullptr) const // используем ли мы это где-то?
	{
		double y = accessRateF(p);
		double R2 = R * R, k = y / R2; 
		if (y_)
			*y_ = y / R;

        return k * QVector2D(-p.x() + p0.x(), -p.y() + p0.y()); // вот тут еще осталось непонятным что мы делаем
	}

};
