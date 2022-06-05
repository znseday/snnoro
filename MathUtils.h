#ifndef MATHUTILS_H
#define MATHUTILS_H

class QPointF;

double SimpleRandom(double a = 0, double b = 1);

void RotatePoint(QPointF &p, double alpha);

int CalcLineInterEllipse(double Rx, double Ry, const QPointF &p1, const QPointF &p2, QPointF &Res1, QPointF &Res2);

#endif // MATHUTILS_H
