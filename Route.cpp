#include "Route.h"


QJsonObject Abonent_t::RepresentAsJsonObject() const
{
    QJsonObject resultObject;

    resultObject.insert("v", v);

    return resultObject;
}
//----------------------------------------------------------

void Abonent_t::LoadFromJsonObject(const QJsonObject &_jsonObject)
{
    v = _jsonObject["v"].toDouble(0);
}
//----------------------------------------------------------

void Route::CalcOtherWeights()
{
    size_t undefinedCount = 0;
    double sum_w = 0;
    for (const auto & p : Points)
        if (p.Weight <= 0)
            undefinedCount++;
        else
            sum_w += p.Weight;

    if (sum_w > 1.0)
        throw std::runtime_error("sum_w > 1");

    double w = (1.0 - sum_w)/double(undefinedCount);

    for (auto & p : Points)
        if (p.Weight <= 0)
            p.Weight = w;
}
//----------------------------------------------------------

void Route::CalcAbonentPos(int t) // in sec
{
    if (Points.empty())
        throw std::runtime_error("Points.empty() in Route::CalcAbonentPos");

    qDebug() << "CalcAbonentPos, t =" << t;

    Abonent.Pos = Points.front().Pos; // to del

    if (RouteLength <= 0)
        throw std::runtime_error("RouteLength <= 0 in Route::CalcAbonentPos");

    double AboLen = fmod(Abonent.v*t, 2*RouteLength);
    qDebug() << "AboLen =" << AboLen;

    // to do
    double S = 0;
    for (size_t k = 0; k < Points.size()*2 - 1; ++k)
    {
        int i1, i2;
        if (k < Points.size()-1)
        {
            i1 = k;
            i2 = k+1;
        }
        else if (k == Points.size()-1)
        {
            i1 = k;
            i2 = k-1;
        }
        else
        {
            i1 = Points.size()*2 - (k+2);
            i2 = Points.size()*2 - (k+3);
        }

        Abonent.q = Points.at(i2).Pos - Points.at(i1).Pos;
        S += Abonent.q.length();
        if (S > AboLen)
        {
            double reverseS = S - AboLen;

            Abonent.Pos = Points[i2].Pos - Abonent.q.normalized()*reverseS;

            qDebug() << "Abonent.Pos =" << Abonent.Pos;

            break;
//            Abonent.Pos = Points[i].Pos + q.normalized()*Abonent.v*dt;
        }
    }
}
// k = 0 1 2 3 4 5 6 7 8 9
//     0 1 2 3 4 3 2 1 0


//----------------------------------------------------------

void Route::CalcRouteLength()
{
    RouteLength = 0;
    for (size_t i = 0; i < Points.size()-1; ++i)
    {
        QVector3D q = Points[i+1].Pos - Points[i].Pos;
        RouteLength += q.length();
    }
    qDebug() << "RouteLength =" << RouteLength;
}
//----------------------------------------------------------





