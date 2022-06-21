#include "TargetFunctionsSecondPhase.h"
#include "GradConfig.h"

using namespace std;

void TargetFuncAdditiveSphereSecondPhase::Init(MyConfig *_myConfig)
{
    TargetFunctionBase::Init(_myConfig);
    param_count = myConfig->NodesAccess().size()*2;
}

double TargetFuncAdditiveSphereSecondPhase::operator()(const std::vector<double> &params) const
{
    const auto & Routes = myConfig->RoutesAccess();
    const auto & Relief = myConfig->GetRelief();
    /*const*/ auto & Nodes = myConfig->NodesAccess();

    double sum_w_of_routes = 0; // Важно! Здесь пока НЕ используется!
    for (auto & route : Routes)
    {
        sum_w_of_routes += route.Get_w();
    }

    double y1 = 0;
    size_t dk = 2;

    for (size_t k = 0; k < Nodes.size()*2; k += dk)
    {
        SignalNode sn(QVector3D(params[k],
                                params[k+1],
                              Relief->CalcRealZbyRealXY(params[k], params[k+1]) ),
                      Nodes[k/2].R);


        for (const auto & b : Nodes[k/2].Bonds)
        {
//                const RoutePoint & p1 = Routes.at(std::get<0>(b)).Points.at(std::get<1>(b));
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

double TargetFuncProbabilisticSphereSecondPhase::operator()(const std::vector<double> &params) const
{
    // Не было реализовано и в изначальной версии на лямбдах!
    (void)params;
    return 0;
}
//-------------------------------------------------------------


void TargetFuncAdditiveConeSecondPhase::Init(MyConfig *_myConfig)
{
    TargetFunctionBase::Init(_myConfig);
    param_count = myConfig->NodesAccess().size()*3;
}
//-------------------------------------------------------------


double TargetFuncAdditiveConeSecondPhase::operator()(const std::vector<double> &params) const
{
    // Не было реализовано и в изначальной версии на лямбдах!
    (void)params;
    return 0;
}
//-------------------------------------------------------------


void TargetFuncProbabilisticConeSecondPhase::Init(MyConfig *_myConfig)
{
    TargetFunctionBase::Init(_myConfig);
    param_count = myConfig->NodesAccess().size()*3;
}
//-------------------------------------------------------------


double TargetFuncProbabilisticConeSecondPhase::operator()(const std::vector<double> &params) const
{
    // Не было реализовано и в изначальной версии на лямбдах!
    (void)params;
    return 0;
}
//-------------------------------------------------------------

