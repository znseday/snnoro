#ifndef ROUTE_H
#define ROUTE_H

#include <vector>
#include <QJsonObject>

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

    QVector3D q;

    double accessRate = 0;

    Abonent_t() = default;
    Abonent_t(const Pos3d &_pos, double _v) : Pos(_pos), v(_v) {}
    QJsonObject RepresentAsJsonObject() const;
    void LoadFromJsonObject(const QJsonObject &_jsonObject);

    // double Calc(rel, nodes)
};
//----------------------------------------------------------

// При изменении маршрута - вызывать метод перерасчета длин
class Route
{
protected:

    QString Name = "Unnamed";
    // double Priority; // что-то в этом роде, инфа для конкретного маршрута/абонента
    // QColor Color;

    Abonent_t Abonent;

    double RouteLength = 0;

    double w = 1;

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

    void CalcAbonentPos(int t);

    const Abonent_t & GetAbonent() const {return Abonent;}
    Abonent_t & AbonentDirectAccess() {return Abonent;}

    const QString & GetName() const {return Name;}

    void SetName(const QString &_name) {Name = _name;}
    void SetVforAbonent(double _v) {Abonent.v = _v;} // ?

    void CalcRouteLength();

    double Get_w() const {return w;}
    void Set_w(double _w) {w = _w;}
};

#endif // ROUTE_H
