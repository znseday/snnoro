#ifndef SIGNALNODE_H
#define SIGNALNODE_H

#include <iostream>
#include <set>

#include "Types.h"
//#include <QVector2D>
//#include <QVector3D>
class QVector3D;
#include <QRectF>

#include <QJsonObject>

//#include "Relief.h"

#include <GL/glu.h>

class Relief3D;
struct Settings3dType;

enum class SignalNodeType
{
    Sphere,
    Cone,
    Unknown
};


struct BondType
{
    size_t iRoute;
    size_t iPoint;
    double arf;
    double relDist;
    BondType() = delete;
    BondType(size_t _iRoute, size_t _iPoint, double _arf, double _relDist)
        : iRoute(_iRoute), iPoint(_iPoint), arf(_arf), relDist(_relDist) {}
};
bool operator<(const BondType &lhs, const BondType &rhs);

//Каждый сигнальный узел будет иметь свой set<BondType>
using BondsType = std::set<BondType>;
//using BondsType = std::set<std::tuple<size_t, size_t, double, double>>;

class SignalNode
{
public:

    Pos3d Pos;
    double R = 0;     // св-во оборудования
    double Alpha = 0;
    double Beta = 0;  // св-во оборудования

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

    void SetRandomCoord(const Relief3D &_relief);
    void SetRandomAlpha() {Alpha = rand()/(double)RAND_MAX*2.0*M_PI;}
    bool SetCoordForPos(const Relief3D &_relief, const Pos3d &_pos);

    double accessRateSphere(const Pos3d &p) const;
    double accessRateCone(const Pos3d &p) const;

    friend std::ostream & operator<<(std::ostream & s, const SignalNode &ob);

    static SignalNodeType ConvertStringToSignalNodeType(QString &str);
    static QString ConvertSignalNodeTypeToString(SignalNodeType snt);


    std::tuple<double, double, double> CalcEllispe_abc() const;

    QJsonObject RepresentAsJsonObject() const;
    void LoadFromJsonObject(const QJsonObject &_jsonObject);

    static GLUquadric * Quadric();

    void DrawIn3D(SignalNodeType _snt, const Relief3D *relief,
                  const Settings3dType & _settings3d) const;

    int CalcIntersectWithLineToPoint(const Pos3d &_point, QPointF &_result) const;
};



#endif // SIGNALNODE_H
