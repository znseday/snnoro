#pragma once

#include "MathUtil.h"

#include <QVector2D>

class Trajectory
{
protected:
    QVector2D line, u;

    void init()
	{
        line = p1 - p0;
        u = line.normalized();
        T = line.length() / v;  // km / (km/sec) = sec
	}

public:

    QVector2D p0, p1;

    double v, T;
    double SignalPower = -1;
    double DistToNearestNode = -1;

    Trajectory(const QVector2D & p0_, const QVector2D & p1_, double v_) : p0(p0_), p1(p1_), v(v_)
	{
		init();
	}

	Trajectory() = delete;

    Trajectory(const Trajectory &other) : p0(other.p0), p1(other.p1), v(other.v)
	{
		init();
	}

    Trajectory & operator=(const Trajectory &) = default; // Использует по дефолту, т.к. тривиально копируемый тип

    Trajectory(Trajectory &&) noexcept = delete; // удаляем т.к. в проекте не предусмотрена семантика перемещения
    Trajectory & operator=(Trajectory &&) noexcept = delete;// удаляем т.к. в проекте не предусмотрена семантика перемещения


	// Статика
	// Динамика

    QVector2D location(double t) const // видимо это алгоритм того, что узел разворачивается в конце маршрута
	{
		int k = floor(t / (2 * T));
		t -= k * 2.0 * T;
        return t < T ? QVector2D(p0 + v * t * u) : p1 - v * (t - T) * u;
	}
};
