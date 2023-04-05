#ifndef SIGNALNODE_H
#define SIGNALNODE_H

#include <iostream>
#include <set>

#include "TypesAndUtils.h"
#include "MathUtils.h"

//class QVector3D;

#include <QRectF>
#include <QJsonObject>

extern const QString SignalNodesExtension;

#include <GL/glu.h>

class Relief3D;
struct Settings3dType;

enum class SignalNodeType
{
    Sphere,
    Cone,
    Unknown
};

enum class SignalNodeStatus
{
    NotSelected,
    Selected
};


struct BondType
{
    size_t iRoute;
    size_t iPoint;
    MyDoubleType arf;
    MyDoubleType relDist;
    BondType() = delete;
    BondType(size_t _iRoute, size_t _iPoint, MyDoubleType _arf, MyDoubleType _relDist)
        : iRoute(_iRoute), iPoint(_iPoint), arf(_arf), relDist(_relDist) {}
};
bool operator<(const BondType &lhs, const BondType &rhs);

//Каждый сигнальный узел будет иметь свой set<BondType>
using BondsType = std::set<BondType>;
//using BondsType = std::set<std::tuple<size_t, size_t, MyDoubleType, MyDoubleType>>;

class SignalNode
{
public:

    MyPos3d<> Pos;
    MyDoubleType R = 0;     // св-во оборудования
    MyDoubleType Alpha = 0;
    MyDoubleType Beta = 0;  // св-во оборудования

    BondsType Bonds;

    SignalNode() = default;

    SignalNode(const MyVector3D<> & _pos, MyDoubleType _R)
        : Pos(_pos), R(_R) {};

    SignalNode(const MyVector3D<> & _pos, MyDoubleType _R, MyDoubleType _alpha, MyDoubleType _beta)
        : Pos(_pos), R(_R), Alpha(_alpha), Beta(_beta) {};

    SignalNode(const SignalNode &) = default;
    SignalNode & operator=(const SignalNode &) = default;

    SignalNode(SignalNode &&) = default;
    SignalNode & operator=(SignalNode &&) = default;

    void SetRandomCoord(const QRectF &_area, const Relief3D &_relief);


    void SetRandomAlpha() {Alpha = rand()/(MyDoubleType)RAND_MAX*2.0*M_PI;}
    bool SetCoordForPos(const Relief3D &_relief, const MyPos3d<> &_pos);

    MyDoubleType accessRateSphere(const MyPos3d<> &p) const;
    MyDoubleType accessRateCone(const MyPos3d<> &p) const;

    friend std::ostream & operator<<(std::ostream & s, const SignalNode &ob);

    static SignalNodeType ConvertStringToSignalNodeType(const QString &str);
    static QString ConvertSignalNodeTypeToString(SignalNodeType snt);


    std::tuple<double, double, double> CalcEllispe_abc() const;

    QJsonObject RepresentAsJsonObject() const;
    void LoadFromJsonObject(const QJsonObject &_jsonObject);

    static GLUquadric * Quadric();

    void DrawIn3D(SignalNodeType _snt, const Relief3D *relief,
                  const Settings3dType & _settings3d, SignalNodeStatus _sns,
                  const WhatShowStruct &_whatShow) const;

    int CalcIntersectWithLineToPoint(const MyPos3d<> &_point, QPointF &_result) const;
};



#endif // SIGNALNODE_H
