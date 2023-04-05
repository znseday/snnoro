#include "TargetFunctionsSecondPhase.h"
#include "GradConfig.h"

using namespace std;

void TargetFuncAdditiveSphereSecondPhase::Init(MyConfig *_myConfig)
{
    TargetFunctionBase::Init(_myConfig);
    param_count = myConfig->NodesAccess().size()*2;
}

long double TargetFuncAdditiveSphereSecondPhase::operator()(const std::vector<long double> &params) const
{
    const auto & Routes = myConfig->RoutesAccess();
    const auto & Relief = myConfig->GetRelief();
    /*const*/ auto & Nodes = myConfig->NodesAccess();

    double sum_w_of_routes = 0;
    for (auto & route : Routes)
    {
        sum_w_of_routes += route.Get_w();
    }

    double y1 = 0;
    size_t dk = 2;

//    for (size_t k = 0; k < Nodes.size()*dk; k += dk)
    for (size_t k = 0; k < param_count; k += dk)
    {
        SignalNode sn(MyVector3D<>(params[k],
                                   params[k+1],
                                   Relief->CalcRealZbyRealXY(params[k], params[k+1]) ),
                                   Nodes[k/dk].R);


        for (const auto & b : Nodes[k/dk].Bonds)
        {
            const RoutePoint & p1 = Routes.at(b.iRoute).Points.at(b.iPoint);

            double y = Aarf * sn.accessRateSphere(p1.Pos);

            if (IsUseLineBetweenTwoPoints)
            {
                y *= myConfig->IsLineBetweenTwoPoints(sn.Pos, p1.Pos);
            }

//                if ( IsLineBetweenTwoPoints(sn.Pos, p1.Pos) )
//                    y = _targetFuncSettings.Aarf * sn.accessRateF(p1.Pos);
//                else
//                    y = 0;


            double w = p1.Weight;                   // 11111111111111111111
            y *= w;//*(1-tanh(k_step*(x-sn.R)));

//                y *= Routes.at(std::get<0>(b)).Get_w(); // ?????????????????

            y *= Routes.at(b.iRoute).Get_w() / sum_w_of_routes;

            y1 += y;

        }
    }

    return -y1;
}

TargetFuncAdditiveSphereSecondPhase::~TargetFuncAdditiveSphereSecondPhase()
{
    cout << __PRETTY_FUNCTION__ << endl; // just to be sure
}
//-------------------------------------------------------------


void TargetFuncProbabilisticSphereSecondPhase::Init(MyConfig *_myConfig)
{
    TargetFunctionBase::Init(_myConfig);
    param_count = myConfig->NodesAccess().size()*2;
}

long double TargetFuncProbabilisticSphereSecondPhase::operator()(const std::vector<long double> &params) const
{
    const auto & Routes = myConfig->RoutesAccess();
    const auto & Relief = myConfig->GetRelief();
    /*const*/ auto & Nodes = myConfig->NodesAccess();

//    double sum_w_of_routes = 0;
//    for (auto & route : Routes)
//        sum_w_of_routes += route.Get_w();

    double y1 = 0;
    size_t dk = 2;
    for (size_t iRoute = 0; iRoute < Routes.size(); ++iRoute) // все точки всех маршрутов
    {
        for (size_t iPoint = 0; iPoint < Routes[iRoute].Points.size(); ++iPoint) // цикл по точкаи одного маршрута
        {
            double s = 0;
            bool isFirst_y = true;

            for (size_t k = 0; k < param_count; k += dk)
            {
                SignalNode sn(MyVector3D<>(params[k],
                                           params[k+1],
                                           Relief->CalcRealZbyRealXY(params[k], params[k+1])  ),
                                           Nodes[k/dk].R);

                bool isService = false;
                for (const auto & b : Nodes[k/dk].Bonds)
                {
//                    const RoutePoint & pb = Routes.at(b.iRoute).Points.at(b.iPoint);
                    if (iRoute == b.iRoute && iPoint == b.iPoint)
                    {
                        isService = true;
                        break;
                    }
                }

                if (!isService)
                    continue;

//                    double y = /* _targetFuncSettings.Aarf * */ sn.accessRateSphere(p1.Pos);
                double y;
                y = sn.accessRateSphere(Routes[iRoute].Points[iPoint].Pos);

                if (IsUseLineBetweenTwoPoints)
                {
                    y *= myConfig->IsLineBetweenTwoPoints(sn.Pos, Routes[iRoute].Points[iPoint].Pos);
                }

//                double w = p1.Weight;  // !!!!!!!!!!!!!!
//                    y *= w;                   //*(1-tanh(k_step*(x-sn.R)));

//                    qDebug() << "p1 =" << p1.Pos << ": w =" << w;

//                if (IsUseCoveredFlag && !p1.IsCovered)
//                {
//                    y *= 2;
//                }

                if (isFirst_y)
                {
                    s = y;
                    isFirst_y = false;
                }
                else
                {
                    s = s + y - s*y; // для каждоый точки по всем узлам
                }

            }

            y1 += s; // здесь s - это для каждоый из точек всех маршрутов

        }

//        y1 *= (route.Get_w() / sum_w_of_routes);
    }

    y1 *= Aarf;

    return -y1;
}
//-------------------------------------------------------------


void TargetFuncAdditiveConeSecondPhase::Init(MyConfig *_myConfig)
{
    TargetFunctionBase::Init(_myConfig);
    param_count = myConfig->NodesAccess().size()*3;
}
//-------------------------------------------------------------

long double TargetFuncAdditiveConeSecondPhase::operator()(const std::vector<long double> &params) const
{
    const auto & Routes = myConfig->RoutesAccess();
    const auto & Relief = myConfig->GetRelief();
    const auto & Nodes = myConfig->NodesAccess();

    double sum_w_of_routes = 0;
    for (auto & route : Routes)
    {
        sum_w_of_routes += route.Get_w();
    }

    double y1 = 0;
    size_t dk = 3;

//    printf("param_count = %i\n", (int)param_count);

    for (size_t k = 0; k < param_count; k += dk)
    {
        SignalNode sn(MyVector3D<>(params[k],
                                   params[k+1],
                                   Relief->CalcRealZbyRealXY(params[k], params[k+1])  ),
                                   Nodes[k/dk].R,
                                   params[k+2] / WierdCoeffAlpha, //Nodes[k/dk].Alpha,
                                   Nodes[k/dk].Beta);

        for (const auto & b : Nodes[k/dk].Bonds)
        {
            const RoutePoint & p1 = Routes.at(b.iRoute).Points.at(b.iPoint);

            double y = Aarf * sn.accessRateCone(p1.Pos);

            if (IsUseLineBetweenTwoPoints)
            {
                y *= myConfig->IsLineBetweenTwoPoints(sn.Pos, p1.Pos);
            }

            double w = p1.Weight;  // !!!!!!!
            y *= w;                   //*(1-tanh(k_step*(x-sn.R)));

            y *= Routes.at(b.iRoute).Get_w() / sum_w_of_routes;

            y1 += y;
        }
    }

    return -y1;
}
//-------------------------------------------------------------

void TargetFuncProbabilisticConeSecondPhase::Init(MyConfig *_myConfig)
{
    TargetFunctionBase::Init(_myConfig);
    param_count = myConfig->NodesAccess().size()*3;
}
//-------------------------------------------------------------

long double TargetFuncProbabilisticConeSecondPhase::operator()(const std::vector<long double> &params) const
{
    const auto & Routes = myConfig->RoutesAccess();
    const auto & Relief = myConfig->GetRelief();
    const auto & Nodes = myConfig->NodesAccess();

//    double sum_w_of_routes = 0;
//    for (auto & route : Routes)
//        sum_w_of_routes += route.Get_w();

    double y1 = 0;
    size_t dk =  3;

    for (size_t iRoute = 0; iRoute < Routes.size(); ++iRoute) // все точки всех маршрутов
    {
        for (size_t iPoint = 0; iPoint < Routes[iRoute].Points.size(); ++iPoint) // цикл по точкаи одного маршрута
        {
            double s = 0;
            bool isFirst_y = true;

            for (size_t k = 0; k < param_count; k += dk)
            {
                SignalNode sn(MyVector3D<>(params[k],
                                           params[k+1],
                                           Relief->CalcRealZbyRealXY(params[k], params[k+1])  ),
                                           Nodes[k/dk].R,
                                           params[k+2], //Nodes[k/dk].Alpha,
                                           Nodes[k/dk].Beta);

//                    double y = /* _targetFuncSettings.Aarf * */ sn.accessRateSphere(p1.Pos);
                bool isService = false;
                for (const auto & b : Nodes[k/dk].Bonds)
                {
//                    const RoutePoint & pb = Routes.at(b.iRoute).Points.at(b.iPoint);
                    if (iRoute == b.iRoute && iPoint == b.iPoint)
                    {
                        isService = true;
                        break;
                    }
                }

                if (!isService)
                    continue;

                double y = sn.accessRateCone(Routes[iRoute].Points[iPoint].Pos);

                if (IsUseLineBetweenTwoPoints)
                {
                    y *= myConfig->IsLineBetweenTwoPoints(sn.Pos, Routes[iRoute].Points[iPoint].Pos);
                }

                if (isFirst_y)
                {
                    s = y;
                    isFirst_y = false;
                }
                else
                {
                    s = s + y - s*y; // для каждоый точки по всем узлам
                }

            }

            y1 += s;

        }

    }

    y1 *= Aarf;

    return -y1;
}
//-------------------------------------------------------------








