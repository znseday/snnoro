#include "SignalNode.h"
#include "MathUtils.h"

#include "Relief.h"

#include <cmath>
#include <QDebug>
#include <QLineF>

#include "GradConfig.h"

using namespace std;

const QString SignalNodesExtension = "*.snf";

bool operator<(const BondType &lhs, const BondType &rhs)
{
//    return std::tie(lhs.iRoute, lhs.iPoint, lhs.arf, lhs.relDist) < std::tie(rhs.iRoute, rhs.iPoint, rhs.arf, rhs.relDist);
    auto tupleRefs = [](const BondType & p) { return std::tie(p.iRoute, p.iPoint, p.arf, p.relDist); };
    return tupleRefs(lhs) < tupleRefs(rhs);
}
//----------------------------------------------------------

//void SignalNode::SetRandomCoord(const Relief3D &_relief)
void SignalNode::SetRandomCoord(const QRectF &_area, const Relief3D &_relief)
{ 
//    const auto &_area = _relief.GetArea();
//    const auto &_area = _relief.GetArea();

    Pos.setX( SimpleRandom(_area.left(), _area.right()) );
    Pos.setY( SimpleRandom(_area.bottom(), _area.top()) );

    Pos.setZ( _relief.CalcRealZbyRealXY( Pos.x(), Pos.y()) );
}
//----------------------------------------------------------

bool SignalNode::SetCoordForPos(const Relief3D &_relief, const MyPos3d<> &_pos)
{
    const auto &_area = _relief.GetArea();

    if (_area.isValid())
        qDebug() << "Valid";
    else
        qDebug() << "Invalid";


    MyDoubleType x, y;
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

    Pos.setZ( _relief.CalcRealZbyRealXY( Pos.x(), Pos.y()) );

    return isOk;
}
//----------------------------------------------------------

MyDoubleType SignalNode::accessRateSphere(const MyPos3d<> &p) const
{
    // это квадрат расстояния между текущей позицией и p
    MyDoubleType d2 = (p.x()-Pos.x())*(p.x()-Pos.x()) +
                      (p.y()-Pos.y())*(p.y()-Pos.y()) +
                      (p.z()-Pos.z())*(p.z()-Pos.z());

    MyDoubleType q = 3; // Сделать настроиваемым параметром???
    MyDoubleType k;

    if (d2 >= (q*q))
    {
        k = 1;
    }
    else
    {
        k = d2/(q*q);
//        qDebug() << "d2 < (q*q)";  // !!!!!!!!!!!!!!!!!!!!
    }

    MyDoubleType y = exp(-d2/(2.0*R*R)) * k;
    return y;
}
//----------------------------------------------------------

MyDoubleType SignalNode::accessRateCone(const MyPos3d<> &p) const
{
//    return 1; // !!!!!!!!!!!!!!!!!!!!!!!!

//    QPointF interPoint;
//    int countIntersects = CalcIntersectWithLineToPoint(p, interPoint);   // Переделать так, чтобы дистанция возвращалась сразу из функции?
//    if (countIntersects < 2)
//    {
//        qDebug() << "countIntersects < 2 in SignalNode::accessRateCone";
//        return 2; // ???
//    }

    //    if (!isOk)
//        throw std::logic_error("There is not any instersection with ellipse in SignalNode::accessRateCone");

    // Важно !!! Здесь рассчет в 2d !

//    qDebug() << "Pos.toPointF() = " << Pos.toPointF();
//    qDebug() << "interPoint =" << interPoint;
//    qDebug() << "p.toPointF() = " << p.toPointF();

//    interPoint = {100, 100};

//    MyDoubleType dist_from_sn_to_intersect = QLineF(Pos.toPointF(), interPoint).length();
//    MyDoubleType dist_from_sn_to_point_of_route = QLineF(Pos.toPointF(), p.toPointF()).length();
//    MyDoubleType k;// = dist_from_sn_to_intersect / (dist_from_sn_to_point_of_route + 10);


//    // Переделать. Сделать физично.
//    // Вылетает ошибка при подсчете пересечений, когда узел оказывается в точке маршрута и расстояние до него 0

//    if (dist_from_sn_to_point_of_route >= 100)
//        k = dist_from_sn_to_intersect / (dist_from_sn_to_point_of_route);
//    else
//        k = (dist_from_sn_to_intersect + 100) / (100.0);

//    if (fabs(k) < 0.01 || fabs(k) > 1000)
//        qDebug() << "k =" << k;  // k иногда уходит в inf !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//    return k;

    MyVector2D<> v1 = { float(p.x() - Pos.x()), float(p.y() - Pos.y()) };
    MyVector2D<> v2 = { float(cos(Alpha)), float(sin(Alpha))};

    MyDoubleType fix_zero = 0;

    MyDoubleType c = MyVector2D<>::dotProduct(v1, v2) / (v1.length()+fix_zero); // fix_zero???

    if (v1.length() < 1e-6)
    {
        qDebug() << "v1.length() =" << (double)v1.length();
//        fix_zero = 20; // ???
        c = 0;
    }




    if (c > 1.0)
    {
//        qDebug() << "c > 1.0, c =" << c;
        c = 1.0;
    }

    if (c < -1.0)
    {
//        qDebug() << "c < -1.0, c =" << c;
        c = -1.0;
    }

    MyDoubleType gamma = acos( c ); // fix_zero???



//    gamma *= 180.0/M_PI;
//    qDebug() << "gamma =" << gamma;
//    qDebug() << "Beta =" << Beta;


    MyDoubleType sigma = Beta/1.0;  // Beta in radians?
    MyDoubleType k = /* 1/(sigma*sqrt(2.0*M_PI)) * */ exp(-0.5*gamma*gamma/(sigma*sigma));

//    qDebug() << "gamma =" << gamma;

    MyDoubleType arfS = accessRateSphere(p);

//    qDebug() << "arfS =" << gamma;

    MyDoubleType res = k * arfS;

    if (std::isnan(k))
    {
        qDebug() << "sigma =" << (double)sigma;
        qDebug() << "gamma =" << (double)gamma;
        qDebug() << "c =" << (double)c;

        qDebug() << "std::isnan(k) in SignalNode::accessRateCone";
    }

    if (std::isnan(arfS))
    {
        qDebug() << "std::isnan(arfS) in SignalNode::accessRateCone";
    }

    if (std::isnan(res))
    {
        throw std::logic_error("std::isnan(res) in SignalNode::accessRateCone");
    }

    return res;
}
//----------------------------------------------------------

SignalNodeType SignalNode::ConvertStringToSignalNodeType(const QString &str) // static member-function
{
    if (str.toUpper() == "SPHERE")
        return SignalNodeType::Sphere;
    else if (str.toUpper() == "CONE")
        return SignalNodeType::Cone;
    else
        return SignalNodeType::Unknown;
}
//----------------------------------------------------------

QString SignalNode::ConvertSignalNodeTypeToString(SignalNodeType snt) // static member-function
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

std::tuple<double, double, double> SignalNode::CalcEllispe_abc() const
{
    double asp_ab = M_PI / Beta;
//    double a = R*1.2;
    double a = R;

    double b = a / sqrt(asp_ab);
    double c = sqrt(abs(a*a - b*b));
    return {a, b, c};
}
//----------------------------------------------------------

QJsonObject SignalNode::RepresentAsJsonObject() const
{
    QJsonObject nodeObject;
    nodeObject.insert("R", (int)R);
    nodeObject.insert("Beta", qRadiansToDegrees((double)Beta));
    return nodeObject;
}
//----------------------------------------------------------

void SignalNode::LoadFromJsonObject(const QJsonObject &_jsonObject)
{
    R = _jsonObject["R"].toDouble(-1);
    Beta = qDegreesToRadians(_jsonObject["Beta"].toDouble(0));
}
//----------------------------------------------------------

GLUquadric* SignalNode::Quadric()
{
    static GLUquadric* q = nullptr;
    if (!q) q = gluNewQuadric();
    return q;
}
//----------------------------------------------------------

void SignalNode::DrawIn3D(SignalNodeType _snt, const Relief3D *relief,
                          const Settings3dType & _settings3d, SignalNodeStatus _sns,
                          const WhatShowStruct &_whatShow) const
{
//    constexpr float zOffset = 0.01f;
    constexpr float zOffset = 0.005f;

    const auto & area = relief->GetArea();
    double kx = 2.0/area.width();
    double ky = 2.0/area.height();
    double k = min(kx, ky);

    double hW = area.width()/2.0;
    double hH = area.height()/2.0;

    double offsetX = area.x()+hW; // in meters
    double offsetY = area.y()+hH; // in meters
    double offsetZ = 0;

    double x = (Pos.x()-offsetX)*k;
    double y = (Pos.y()-offsetY)*k;
    double z;

    if (_sns == SignalNodeStatus::NotSelected)
        glColor3f(0.6, 0.1, 0.1);
    else
        glColor3f(0.95, 0.05, 0.05);

    z = (Pos.z()-offsetZ)*relief->GetGlobal_kz();

    glPushMatrix();

        glTranslatef(x, y, zOffset + (_settings3d.IsPerspective ? z : 0));

        gluQuadricDrawStyle(Quadric(), GLU_FILL);

//        if (_sns == SignalNodeStatus::Selected)
//        {
//            qDebug() << "_settings3d.TrZ =" << _settings3d.TrZ;
//            qDebug() << "z =" << z;
//            qDebug() << "pow =" << pow(fabs(fabs(_settings3d.TrZ) - z), 0.25);
//        }

//        gluSphere(Quadric(), 0.01 * pow(fabs(fabs(_settings3d.TrZ) - z), 0.25), 12, 12);
        gluSphere(Quadric(), 0.015, 12, 12);

    glPopMatrix();

    if (_whatShow.ShowRadii)
    {
        const int nr = 32;
        double dfi, fiStart;

        if (_snt == SignalNodeType::Sphere)
        {
            dfi = 2.0*M_PI/nr;
            fiStart = 0;
        }
        else if (_snt == SignalNodeType::Cone)
        {
            dfi = 2.0*Beta / nr;
            fiStart = Alpha - Beta;
        }
        else
        {
            qDebug() << "Error: SignalNodeType is Unknown";
            dfi = 2*M_PI/nr;
            fiStart = 0;
        }

        glBegin(GL_LINE_STRIP);

        if (_snt == SignalNodeType::Cone) // далее будет копипаст этого ифа
        {
            double x = (Pos.x()-offsetX)*k;
            double y = (Pos.y()-offsetY)*k;
            double z;

            z = zOffset + (_settings3d.IsPerspective ? relief->CalcNormToRealZbyRealXY(Pos.x(), Pos.y()) : 0);
            glVertex3f(x, y, z);
        }

        for (int i = 0; i <= nr; i++)
        {
            double xt = Pos.x() + R*cos(fiStart + i*dfi);
            double yt = Pos.y() + R*sin(fiStart + i*dfi);
            double x = (xt-offsetX)*k;
            double y = (yt-offsetY)*k;
            double z;

            z = zOffset + (_settings3d.IsPerspective ? relief->CalcNormToRealZbyRealXY(xt, yt) : 0);
            glVertex3f(x, y, z);
        }

        if (_snt == SignalNodeType::Cone) // здесь копипаст
        {
            double x = (Pos.x()-offsetX)*k;
            double y = (Pos.y()-offsetY)*k;
            double z;

            z = zOffset + (_settings3d.IsPerspective ? relief->CalcNormToRealZbyRealXY(Pos.x(), Pos.y()) : 0);
            glVertex3f(x, y, z);
        }

        glEnd();

    }


    glColor3f(0.9, 0.1, 0.9);

    if (_whatShow.ShowEllipses && _snt == SignalNodeType::Cone) // здесь ellipse // !!!!!!!!!!
    {
        glBegin(GL_LINE_STRIP);

        const int nr = 32;
        double dfi = 2.0*M_PI/(nr-1);

        auto [Rx, Ry, c] = CalcEllispe_abc();

        for (int i = 0; i < nr; i++)
        {
            double fi = dfi*i;

            QPointF tp = {double(c + Rx*cos(fi)), double(Ry*sin(fi))};
            RotatePoint(tp, Alpha);
            tp += Pos.toPointF();

            // tp - в реальных координатах
            double xt = tp.x();
            double yt = tp.y();

            double x = (tp.x()-offsetX)*k;
            double y = (tp.y()-offsetY)*k;
            double z;

            z = zOffset + (_settings3d.IsPerspective ? relief->CalcNormToRealZbyRealXY(xt, yt) : 0);
            glVertex3f(x, y, z);
        }

        glEnd();
    }
}
//----------------------------------------------------------

int SignalNode::CalcIntersectWithLineToPoint(const MyPos3d<> &_point, QPointF &_result) const
{
    // ВАЖНО! Рассчет ведется в 2d проекции!

    _result = {-1, -1}; // ?
    auto [Rx, Ry, c] = CalcEllispe_abc();
    QPointF ellCenter = {(double)c, 0};
    RotatePoint(ellCenter, Alpha);

    ellCenter += Pos.toPointF();

    double dist_from_sn_to_intersect = QLineF(Pos.toPointF(), _point.toPointF()).length();
//    if (dist_from_sn_to_intersect < 0.01)
//        qDebug() << "dist_from_sn_to_intersect =" << dist_from_sn_to_intersect;


    QPointF nodeInEllCoords = Pos.toPointF() - ellCenter; // оптимизировать ?

//    qDebug() << "nodeInEllCoordsX =" << nodeInEllCoordsX;
//    qDebug() << "nodeInEllCoordsY =" << nodeInEllCoordsY;

//    RotatePoint(nodeInEllCoordsX, nodeInEllCoordsY, -alpha);
    RotatePoint(nodeInEllCoords, -Alpha);

//    qDebug() << "nodeInEllCoordsX (rot reverse) =" << nodeInEllCoordsX;
//    qDebug() << "nodeInEllCoordsY (rot reverse) =" << nodeInEllCoordsY;

//    double pointInEllCoordsX = pointX - ellCenterX;
//    double pointInEllCoordsY = pointY - ellCenterY;
    QPointF pointInEllCoords = _point.toPointF() - ellCenter;

//    qDebug() << "pointInEllCoordsX =" << pointInEllCoordsX;
//    qDebug() << "pointInEllCoordsY =" << pointInEllCoordsY;

//    RotatePoint(pointInEllCoordsX, pointInEllCoordsY, -alpha);
    RotatePoint(pointInEllCoords, -Alpha);

//    qDebug() << "pointInEllCoordsX (rot reverse) =" << pointInEllCoordsX;
//    qDebug() << "pointInEllCoordsY (rot reverse) =" << pointInEllCoordsY;

    QPointF intersect1, intersect2;

    int count = CalcLineInterEllipse(Rx, Ry, nodeInEllCoords, pointInEllCoords, intersect1, intersect2);

    if (count < 2 && dist_from_sn_to_intersect < 0.0001)
    {
        return count; // Подумать, что делать с точками пересечения и как оих обрабатывать в вызывающем коде
    }

    if (count < 2)
    {
        qDebug() << "nodeInEllCoords =" << nodeInEllCoords;
        qDebug() << "nodeInEllCoords =" << pointInEllCoords;
        qDebug() << "count of intersections =" << count;
        qDebug() << "intersect1 =" << intersect1;
        qDebug() << "intersect2 =" << intersect2;
        throw std::runtime_error("count of intersections < 2");
    }

    if (count < 1)
        return count;

    RotatePoint(intersect1, Alpha);
    intersect1 += ellCenter;

    RotatePoint(intersect2, Alpha);
    intersect2 += ellCenter;

    // Важно!!! Здесь дистанция в 2d !
//    double distToInter1 = QLineF(_point.toPointF(), intersect1).length();
//    double distToInter2 = QLineF(_point.toPointF(), intersect2).length();

    double dx = _point.x() - Pos.x();
    double dy = _point.y() - Pos.y();

    double dx1 = intersect1.x() - Pos.x();
    double dy1 = intersect1.y() - Pos.y();

//    qDebug() << "dx =" << dx;
//    qDebug() << "dy =" << dy;
//    qDebug() << "dx1 =" << dx1;
//    qDebug() << "dy1 =" << dy1;

    if ( ( (dx >= 0 && dx1 >= 0) || (dx <= 0 && dx1 <= 0) ) &&
         ( (dy >= 0 && dy1 >= 0) || (dy <= 0 && dy1 <= 0) ) )
    {
        _result = intersect1; // painter.drawText(x1, y1, "This inside");
    }
    else
    {
        _result = intersect2; // painter.drawText(x2, y2, "This inside");
    }

//    if ( pointInEllCoords.x()*pointInEllCoords.x()/(Rx*Rx) + pointInEllCoords.y()*pointInEllCoords.y()/(Ry*Ry) < 1.0 )
//    {
//        if (distToInter1 > distToInter2) // Вместо этого нужно что-то подумать
//        {
//            _result = intersect2; // painter.drawText(x2, y2, "This inside");
//        }
//        else
//        {
//            _result = intersect1; // painter.drawText(x1, y1, "This inside");
//        }
//    }

//    else
//    {
//        if (distToInter1 > distToInter2)
//        {
//            _result = intersect2; // painter.drawText(x2, y2, "This outside");
//        }
//        else
//        {
//            _result = intersect1; // painter.drawText(x1, y1, "This outside");
//        }
//    }

    return count;
}
//----------------------------------------------------------


ostream & operator<<(ostream & s, const SignalNode &ob) // для отладки
{
    s << "{" << ob.Pos.x() << "; " << ob.Pos.y() <<  "; " << ob.Pos.z() << "}" << "; R = " << ob.R;
    return s;
}
//----------------------------------------------------------
