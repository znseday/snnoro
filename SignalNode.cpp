#include "SignalNode.h"
#include "MathUtils.h"

#include "Relief.h"

#include <cmath>
#include <QDebug>

using namespace std;

//void SignalNode::SetRandomCoord(const QRectF &_area)
void SignalNode::SetRandomCoord(const Relief3D &_relief)
{ 
    const auto &_area = _relief.GetArea();

    Pos.setX( SimpleRandom(_area.left(), _area.right()) );
    Pos.setY( SimpleRandom(_area.bottom(), _area.top()) );

    Pos.setZ( _relief.CalcRealZbyRealXY( Pos.x(), Pos.y()) );
}
//----------------------------------------------------------

//bool SignalNode::SetCoordForPos(const QRectF &_area, const Pos3d &_pos)
bool SignalNode::SetCoordForPos(const Relief3D &_relief, const Pos3d &_pos)
{
    const auto &_area = _relief.GetArea();

    if (_area.isValid())
        qDebug() << "Valid";
    else
        qDebug() << "Invalid";


    double x, y;
    x = _pos.x();
    y = _pos.y();
    bool isOk = true;
    if ( x < _area.left() )
    {
        x = _area.left();
        isOk = false;
    }
    if ( x > _area.right() )
    {
        x = _area.right();
        isOk = false;
    }
    if ( y > _area.bottom() )
    {
        y = _area.bottom();
        isOk = false;
    }
    if ( y < _area.top() )
    {
        y = _area.top();
        isOk = false;
    }

    Pos.setX(x);
    Pos.setY(y);

    Pos.setZ( _relief.CalcRealZbyRealXY( Pos.x(), Pos.y()) ); // ?

    return isOk;
}
//----------------------------------------------------------

double SignalNode::accessRateF(const QVector3D &p) const
{
    // это квадрат расстояния между текущей позицией и p
    double d2 = (p.x()-Pos.x())*(p.x()-Pos.x()) +
                (p.y()-Pos.y())*(p.y()-Pos.y()) +
                (p.z()-Pos.z())*(p.z()-Pos.z());

    double y = exp(-d2/(2.0*R*R));
    return y;
}
//----------------------------------------------------------

SignalNodeType SignalNode::ConvertStringToSignalNodeType(QString &str)
{
    if (str.toUpper() == "SPHERE")
        return SignalNodeType::Sphere;
    else if (str.toUpper() == "CONE")
        return SignalNodeType::Cone;
    else
        return SignalNodeType::Unknown;
}
//----------------------------------------------------------

QString SignalNode::ConvertSignalNodeTypeToString(SignalNodeType snt)
{
    switch (snt)
    {
    case SignalNodeType::Sphere:
        return "Sphere";
    case SignalNodeType::Cone:
        return "Cone";
    case SignalNodeType::Unknown:
    default:
        return "Unknown";
    }
}
//----------------------------------------------------------

ostream & operator<<(ostream & s, const SignalNode &ob)
{
    s << "{" << ob.Pos.x() << "; " << ob.Pos.y() <<  "; " << ob.Pos.z() << "}" << "; R = " << ob.R;
    return s;
}
//----------------------------------------------------------
