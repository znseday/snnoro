#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <cmath>

#include <QPointF>

using MyDoubleType = long double;

template <typename T = MyDoubleType>
class MyVector2D
{
protected:
    T mx = 0, my = 0;
public:
    MyVector2D() = default;

    MyVector2D(T _x, T _y) : mx(_x), my(_y) {}

    T x() const {return mx;}
    T y() const {return my;}

    void setX(T _x) {mx = _x;}
    void setY(T _y) {my = _y;}

    T length() const
    {
        return std::sqrt(mx*mx + my*my);
    }

    MyVector2D normalized() const
    {
        MyVector2D res(*this);
        auto l = res.length();
        res.mx /= l;
        res.my /= l;
        return res;
    }

    static T dotProduct(const MyVector2D &lhs, const MyVector2D &rhs)
    {
        return lhs.x()*rhs.x() + lhs.y()*rhs.y();
    }
};
//----------------------------------------------------------

template <typename T>
inline MyVector2D<T> operator-(const MyVector2D<T> &lhs, const MyVector2D<T> &rhs)
{
    return {lhs.x() - rhs.x(), lhs.y() - rhs.y()};
}

template <typename T>
inline MyVector2D<T> operator+(const MyVector2D<T> &lhs, const MyVector2D<T> &rhs)
{
    return {lhs.x() + rhs.x(), lhs.y() + rhs.y()};
}

template <typename T>
inline MyVector2D<T> operator*(T k, const MyVector2D<T> &rhs)
{
    return {k*rhs.x(), k*rhs.y()};
}
//----------------------------------------------------------
//----------------------------------------------------------

template <typename T = MyDoubleType>
class MyVector3D;

template <typename T = MyDoubleType>
using MyPos3d = MyVector3D<T>;

template <typename T>
class MyVector3D
{
protected:
    T mx = 0, my = 0, mz = 0;
public:
    MyVector3D() = default;

    MyVector3D(T _x, T _y, T _z) : mx(_x), my(_y), mz(_z) {}

    T x() const {return mx;}
    T y() const {return my;}
    T z() const {return mz;}

    void setX(T _x) {mx = _x;}
    void setY(T _y) {my = _y;}
    void setZ(T _z) {mz = _z;}

    T length() const
    {
        return std::sqrt(mx*mx + my*my + mz*mz);
    }

    MyVector2D<T> toVector2D() const {return {mx, my};}

    QPointF toPointF() const {return {(double)mx, (double)my};}

    T distanceToPoint(const MyPos3d<T> &_p) const
    {
        return std::sqrt((mx-_p.x())*(mx-_p.x()) +
                         (my-_p.y())*(my-_p.y()) +
                         (mz-_p.z())*(mz-_p.z()) );
    }

    MyVector3D normalized() const
    {
        MyVector3D res(*this);
        auto l = res.length();
        res.mx /= l;
        res.my /= l;
        res.mz /= l;
        return res;
    }

    static T dotProduct(const MyVector3D &lhs, const MyVector3D &rhs)
    {
        return lhs.x()*rhs.x() + lhs.y()*rhs.y() + lhs.z()*rhs.z();
    }
};
//----------------------------------------------------------

template <typename T = MyDoubleType>
inline MyVector3D<T> operator-(const MyVector3D<T> &lhs, const MyVector3D<T> &rhs)
{
    return {lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z()};
}

template <typename T = MyDoubleType>
inline MyVector3D<T> operator*(T k, const MyVector3D<T> &rhs)
{
    return {k*rhs.x(), k*rhs.y(), k*rhs.z()};
}
//----------------------------------------------------------
//----------------------------------------------------------


double SimpleRandom(double a = 0, double b = 1);

void RotatePoint(QPointF &p, double alpha);

int CalcLineInterEllipse(double Rx, double Ry, const QPointF &p1, const QPointF &p2, QPointF &Res1, QPointF &Res2);

#endif // MATHUTILS_H
