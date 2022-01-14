#include "Route.h"

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

void Route::CalcAbonentPos(double t)
{
    if (Points.empty())
        throw std::runtime_error("Points.empty() in Route::CalcAbonentPos");

    Abonent.Pos = Points.front().Pos;

    // to do...
}
//----------------------------------------------------------


