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


class Route
{
protected:

    // QString Name;
    // double Priority; // что-то в этом роде, инфа для конкретного маршрута/абонента
    // QColor Color;

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
};

#endif // ROUTE_H
