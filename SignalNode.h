#ifndef SIGNALNODE_H
#define SIGNALNODE_H

#include <iostream>
#include <set>

#include "Types.h"
#include <QVector2D>
#include <QVector3D>
#include <QRectF>

//#include "Relief.h"

class Relief3D;

enum class SignalNodeType
{
    Sphere,
    Cone,
    Unknown
};


using BondsType = std::set<std::tuple<size_t, size_t, double, double>>;

class SignalNode
{
public:

    Pos3d Pos;
    double R = 0;
    double Alpha = 0;
    double Beta = 0;

//    std::set<std::pair<size_t, size_t>> Bonds;
    BondsType Bonds;

    SignalNode() = default;

    SignalNode(const QVector3D & _pos, double _R)
        : Pos(_pos), R(_R) {};

    SignalNode(const QVector3D & _pos, double _R, double _alpha, double _beta)
        : Pos(_pos), R(_R), Alpha(_alpha), Beta(_beta) {};

    SignalNode(const SignalNode &) = default;
    SignalNode & operator=(const SignalNode &) = default;

    SignalNode(SignalNode &&) = default;
    SignalNode & operator=(SignalNode &&) = default;

//    void SetRandomCoord(const QRectF &_area);
//    bool SetCoordForPos(const QRectF &_area, const Pos3d &_pos);

    void SetRandomCoord(const Relief3D &_relief);
    void SetRandomAlpha() {Alpha = rand()/(double)RAND_MAX*2.0*M_PI;}
    bool SetCoordForPos(const Relief3D &_relief, const Pos3d &_pos);

    double accessRateF(const QVector3D &p) const;

    friend std::ostream & operator<<(std::ostream & s, const SignalNode &ob);

//    QVector2D accessGradF(const QVector3D &p, double *y_ = nullptr) const // используем ли мы это где-то?
//    {
//        double y = accessRateF(p);
//        double R2 = R * R, k = y / R2;
//        if (y_)
//            *y_ = y / R;

//        return k * QVector2D(-p.x() + Pos.x(), -p.y() + Pos.y()); // вот тут еще осталось непонятным что мы делаем
//    }

    static SignalNodeType ConvertStringToSignalNodeType(QString &str);
    static QString ConvertSignalNodeTypeToString(SignalNodeType snt);
};



#endif // SIGNALNODE_H
