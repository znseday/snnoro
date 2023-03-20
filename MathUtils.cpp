#include "MathUtils.h"

#include <cmath>
#include <QPointF>
#include <QDebug>

double SimpleRandom(double a, double b)
{
    constexpr double k = 0.0001;
    int f = rand() % 10000;
    int l = rand() % 10001;
    return a + (k*f + k*k*l)*(b - a);
}
//-------------------------------------------------------------

void RotatePoint(QPointF &p, double alpha)
{
    double x1 = p.x()*cos(alpha) - p.y()*sin(alpha);
    double y1 = p.x()*sin(alpha) + p.y()*cos(alpha);
    p.setX(x1);
    p.setY(y1);
}
//-------------------------------------------------------------

int CalcLineInterEllipse(double Rx, double Ry, const QPointF &p1, const QPointF &p2, QPointF &Res1, QPointF &Res2)
{
    double k, b, v, s, dx, dy;
    double AA, BB, CC, D;
    int Result = 0;
    Res1 = Res2 = {-1,-1};

    dx = p2.x() - p1.x();
    dy = p2.y() - p1.y();

    constexpr double eps = 0.0000001;

    if (fabs(dx) < eps) // Подумать над этой константой?
    {
//        qDebug() << "This is fabs(dx) <" << eps << "situation !!!";

        AA = 1;
        BB = 0;
        CC = -(1 - (p1.x()*p1.x())/(Rx*Rx))*(Ry*Ry);
        D = BB*BB - 4 * AA * CC;
        if (D < 0)
            return 0;

        Res1.setX(p1.x());
        Res2.setX(p1.x());

        ++Result;
        Res1.setY( (-BB + sqrt(D)) / (2.0*AA) );  // Проверить не перепутаны ли Res1.y и Res2.y?
        if (D > 0)
        {
            Res2.setY( (-BB - sqrt(D)) / (2.0*AA) );
            ++Result;
        }
    }
    else
    {
        k = dy/dx;
        b = p1.y() - k * p1.x();
        v = Rx*Rx * Ry*Ry;
        s = b;
        AA = Ry*Ry + Rx*Rx*k*k;
        BB = 2.0 * (Rx*Rx * k * s);
        CC = Rx*Rx * s*s - v;
        D = BB*BB - 4 * AA * CC;
        if (D < 0)
            return 0;

        Res1.setX( (-BB + sqrt(D)) / (2.0*AA) );
        Res1.setY( Res1.x() * k + b );
        ++Result;

        if (D > 0)
        {
            Res2.setX( (-BB - sqrt(D)) / (2.0*AA) );
            Res2.setY( Res2.x() * k + b );
            ++Result;
        }
    }

    return Result;
}
//-------------------------------------------------------------
