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

    Abonent.Pos = Points.front().Pos; // to del

    if (RouteLength <= 0)
        throw std::runtime_error("RouteLength <= 0 in Route::CalcAbonentPos");

    double AboLen = fmod(Abonent.v*t, 2*RouteLength);

    // to do

    qDebug() << "CalcAbonentPos, t =" << t;
}
//----------------------------------------------------------

void Route::CalcRouteLength()
{
    RouteLength = 0;
    for (size_t i = 0; i < Points.size()-1; ++i)
    {
        QVector3D q = Points[i+1].Pos - Points[i].Pos;
        RouteLength += q.length();
    }
}
//----------------------------------------------------------





