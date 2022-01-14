#ifndef ROUTE_H
#define ROUTE_H

#include <vector>

#include "Types.h"

class RoutePoint
{
public:
    Pos3d Pos;
    double Weight = -1;

    bool IsCovered = false;

    double SignalPower = -1;
    double DistToNearestNode = -1;
};
//----------------------------------------------------------

class Abonent_t
{
public:
    QString Name;
    Pos3d Pos;
    double v = 0;
    Abonent_t() = default;
    Abonent_t(const Pos3d &_pos, double _v) : Pos(_pos), v(_v) {}
};
//----------------------------------------------------------

class Route
{
protected:

    // QString Name;
    // double Priority; // что-то в этом роде, инфа для конкретного маршрута/абонента
    // QColor Color;

    Abonent_t Abonent;

public:

    std::vector<RoutePoint> Points;

    Route() = default;

    Route(const Route &) = default;
    Route & operator=(const Route &) = default;

    Route(Route &&) = default;
    Route & operator=(Route &&) = default;

    RoutePoint & operator[](size_t i) {return Points[i];}
    const RoutePoint & operator[](size_t i) const {return Points[i];}

    void CalcOtherWeights(); // распределяет оставшийся вес по незаданным точкам маршрута

    void CalcAbonentPos(double t);

    const Abonent_t & GetAbonent() const {return Abonent;}
};

#endif // ROUTE_H
