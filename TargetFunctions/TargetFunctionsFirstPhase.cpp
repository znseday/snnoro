#include "TargetFunctionsFirstPhase.h"
#include "GradConfig.h"

using namespace std;


//-------------------------------------------------------------


void TargetFuncAdditiveSphereFirstPhase::Init(MyConfig *_myConfig)
{
    TargetFunctionBase::Init(_myConfig);
    param_count = myConfig->NodesAccess().size()*2;
}

double TargetFuncAdditiveSphereFirstPhase::operator()(const std::vector<double> &params) const
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

    for (size_t k = 0; k < param_count; k += dk)
    {
            SignalNode sn(QVector3D(params[k],
                                    params[k+1],
                                    Relief->CalcRealZbyRealXY(params[k], params[k+1])  ),
                          Nodes[k/2].R);


//            qDebug() << "params[k] =" << params[k];
//            qDebug() << "params[k+1] =" <<  params[k+1];

        for (auto & route : Routes)
        {
            for (auto & p1 : route.Points)
            {

                double y; // = /*_targetFuncSettings.Aarf * */ sn.accessRateSphere(p1.Pos);
                y = sn.accessRateSphere(p1.Pos);

                if (IsUseLineBetweenTwoPoints)
                {
                    y *= myConfig->IsLineBetweenTwoPoints(sn.Pos, p1.Pos);
                }


                double w = p1.Weight;  // !!!!!!!!!!!!!!
                y *= w;                   //*(1-tanh(k_step*(x-sn.R)));

                if (IsUseCoveredFlag && !p1.IsCovered)
                {
                    y *= 2;
                }

                y1 += y;
            }

            y1 *= (route.Get_w() / sum_w_of_routes);
        }
    }

    myConfig->FindCoveredPointsUsingParams(params, SignalNodeType::Sphere);

    double y2 = 0;
    for (size_t k1 = 0; k1 < (Nodes.size()-1)*dk; k1 += dk)
    {
        for (size_t k2 = k1+2; k2 < Nodes.size()*dk; k2 += dk)
        {
            SignalNode sn1(QVector3D(params[k1],
                                     params[k1+1],
                                     Relief->CalcRealZbyRealXY(params[k1], params[k1+1]) ),
                           Nodes[k1/dk].R);

            SignalNode sn2(QVector3D(params[k2],
                                     params[k2+1],
                                     Relief->CalcRealZbyRealXY(params[k2], params[k2+1])),
                           Nodes[k2/dk].R);

            double x = sn1.Pos.distanceToPoint(sn2.Pos); // Это расстояние в 3d !!! Считать его в 2d или 3d ?
            double R12 = sn1.R + sn2.R;

            R12 -= R_nodeOverlap;

            double step = (1-tanh(k_step_ot*(x-R12)));
            y2 += A2 * log(p*(offX+x))*step;
        }
    }

    y1 *= Aarf;

    return -(y1+y2);
    //        return -y1;
}

TargetFuncAdditiveSphereFirstPhase::~TargetFuncAdditiveSphereFirstPhase()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl; // just to be sure
}
//-------------------------------------------------------------


void TargetFuncProbabilisticSphereFirstPhase::Init(MyConfig *_myConfig)
{
    TargetFunctionBase::Init(_myConfig);
    param_count = myConfig->NodesAccess().size()*2;
}

double TargetFuncProbabilisticSphereFirstPhase::operator()(const std::vector<double> &params) const
{
    const auto & Routes = myConfig->RoutesAccess();
    const auto & Relief = myConfig->GetRelief();
    /*const*/ auto & Nodes = myConfig->NodesAccess();

    double sum_w_of_routes = 0;
    for (auto & route : Routes)
        sum_w_of_routes += route.Get_w();

    double y1 = 0;
    size_t dk = 2;
    for (auto & route : Routes) // все точки всех маршрутов
    {
        for (auto & p1 : route.Points) // цикл по точкаи одного маршрута
        {
            double s = 0;
            for (size_t k = 0; k < param_count; k += dk)
            {
                SignalNode sn(QVector3D(params[k],
                                            params[k+1],
                                            Relief->CalcRealZbyRealXY(params[k], params[k+1])  ),
                                  Nodes[k/dk].R);

//                    double y = /* _targetFuncSettings.Aarf * */ sn.accessRateSphere(p1.Pos);
                double y;
                y = sn.accessRateSphere(p1.Pos);

                if (IsUseLineBetweenTwoPoints)
                {
                    y *= myConfig->IsLineBetweenTwoPoints(sn.Pos, p1.Pos);
                }


                double w = p1.Weight;  // !!!!!!!!!!!!!!
//                    y *= w;                   //*(1-tanh(k_step*(x-sn.R)));

//                    qDebug() << "p1 =" << p1.Pos << ": w =" << w;

                if (IsUseCoveredFlag && !p1.IsCovered)
                {
                    y *= 2;
                }

                if (k == 0)
                    s = y;
                else
                {
                    s = s + y - s*y;
                }

            }

            y1 += s;

        }

        y1 *= (route.Get_w() / sum_w_of_routes);
    }

    myConfig->FindCoveredPointsUsingParams(params, SignalNodeType::Sphere);

    double y2 = 0;
    for (size_t k1 = 0; k1 < (Nodes.size()-1)*dk; k1 += dk)
    {
        for (size_t k2 = k1+2; k2 < Nodes.size()*dk; k2 += dk)
        {
            SignalNode sn1(QVector3D(params[k1],
                                     params[k1+1],
                                     Relief->CalcRealZbyRealXY(params[k1], params[k1+1]) ),
                           Nodes[k1/dk].R);

            SignalNode sn2(QVector3D(params[k2],
                                     params[k2+1],
                                     Relief->CalcRealZbyRealXY(params[k2], params[k2+1])),
                           Nodes[k2/dk].R);

            double x = sn1.Pos.distanceToPoint(sn2.Pos); // Это расстояние в 3d !!! Считать его в 2d или 3d ?
            double R12 = sn1.R + sn2.R;

            R12 -= R_nodeOverlap;

            double step = (1-tanh(k_step_ot*(x-R12)));
            y2 += A2 * log(p*(offX+x))*step;
        }
    }

    y1 *= Aarf;

    return -(y1+y2);
//        return -y1;
}
//-------------------------------------------------------------


void TargetFuncAdditiveConeFirstPhase::Init(MyConfig *_myConfig)
{
    TargetFunctionBase::Init(_myConfig);
    param_count = myConfig->NodesAccess().size()*3;
}

double TargetFuncAdditiveConeFirstPhase::operator()(const std::vector<double> &params) const
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

    for (size_t k = 0; k < param_count; k += dk)
    {
        SignalNode sn(QVector3D(params[k],
                                params[k+1],
                                Relief->CalcRealZbyRealXY(params[k], params[k+1])  ),
                      Nodes[k/dk].R,
                      params[k+2], //Nodes[k/dk].Alpha,
                      Nodes[k/dk].Beta);

        //            qDebug() << "params[k] =" << params[k];
        //            qDebug() << "params[k+1] =" <<  params[k+1];

        for (auto & route : Routes)
        {
            for (auto & p1 : route.Points)
            {

                double y; // = /*_targetFuncSettings.Aarf * */ sn.accessRateSphere(p1.Pos);

                y = sn.accessRateCone(p1.Pos);

                if (IsUseLineBetweenTwoPoints)
                {
                    y *= myConfig->IsLineBetweenTwoPoints(sn.Pos, p1.Pos);
                }


                double w = p1.Weight;  // !!!!!!!!!!!!!!
                y *= w;                   //*(1-tanh(k_step*(x-sn.R)));

                if (IsUseCoveredFlag && !p1.IsCovered)
                {
                    y *= 2;
                }

                y1 += y;
            }

            y1 *= (route.Get_w() / sum_w_of_routes);
        }
    }

    myConfig->FindCoveredPointsUsingParams(params, SignalNodeType::Cone);

    double y2 = 0;
    for (size_t k1 = 0; k1 < (Nodes.size()-1)*dk; k1 += dk)
    {
        for (size_t k2 = k1+2; k2 < Nodes.size()*dk; k2 += dk)
        {
            SignalNode sn1(QVector3D(params[k1],
                                    params[k1+1],
                                    Relief->CalcRealZbyRealXY(params[k1], params[k1+1])  ),
                          Nodes[k1/dk].R,
                          params[k1+2], //Nodes[k/dk].Alpha,
                          Nodes[k1/dk].Beta);

            SignalNode sn2(QVector3D(params[k2],
                                    params[k2+1],
                                    Relief->CalcRealZbyRealXY(params[k2], params[k2+1])  ),
                          Nodes[k2/dk].R,
                          params[k2+2], //Nodes[k/dk].Alpha,
                          Nodes[k2/dk].Beta);

            double x = sn1.Pos.distanceToPoint(sn2.Pos); // Это расстояние в 3d !!! Считать его в 2d или 3d ?
            double R12 = sn1.R + sn2.R;

            R12 -= R_nodeOverlap;

            double step = (1-tanh(k_step_ot*(x-R12)));
            y2 += A2 * log(p*(offX+x))*step;
        }
    }

    y1 *= Aarf;

    return -(y1+y2);
}
//-------------------------------------------------------------


void TargetFuncProbabilisticConeFirstPhase::Init(MyConfig *_myConfig)
{
    TargetFunctionBase::Init(_myConfig);
    param_count = myConfig->NodesAccess().size()*3;
}

double TargetFuncProbabilisticConeFirstPhase::operator()(const std::vector<double> &params) const
{
    const auto & Routes = myConfig->RoutesAccess();
    const auto & Relief = myConfig->GetRelief();
    const auto & Nodes = myConfig->NodesAccess();

    double sum_w_of_routes = 0;
    for (auto & route : Routes)
        sum_w_of_routes += route.Get_w();

    double y1 = 0;
    size_t dk =  3;
    for (auto & route : Routes) // все точки всех маршрутов
    {
        for (auto & p1 : route.Points) // цикл по точкаи одного маршрута
        {
            double s = 0;
            for (size_t k = 0; k < param_count; k += dk)
            {
                SignalNode sn(QVector3D(params[k],
                                        params[k+1],
                                        Relief->CalcRealZbyRealXY(params[k], params[k+1])  ),
                              Nodes[k/dk].R,
                              params[k+2], //Nodes[k/dk].Alpha,
                              Nodes[k/dk].Beta);

//                    double y = /* _targetFuncSettings.Aarf * */ sn.accessRateSphere(p1.Pos);
                double y;

                y = sn.accessRateCone(p1.Pos);

                if (IsUseLineBetweenTwoPoints)
                {
                    y *= myConfig->IsLineBetweenTwoPoints(sn.Pos, p1.Pos);
                }


                double w = p1.Weight;  // !!!!!!!!!!!!!!
//                    y *= w;                   //*(1-tanh(k_step*(x-sn.R)));

//                    qDebug() << "p1 =" << p1.Pos << ": w =" << w;

                if (IsUseCoveredFlag && !p1.IsCovered)
                {
                    y *= 2;
                }

                if (k == 0)
                    s = y;
                else
                {
                    s = s + y - s*y;
                }

            }

            y1 += s;

        }

        y1 *= (route.Get_w() / sum_w_of_routes);
    }

    myConfig->FindCoveredPointsUsingParams(params, SignalNodeType::Cone);

    double y2 = 0;
    for (size_t k1 = 0; k1 < (Nodes.size()-1)*dk; k1 += dk)
    {
        for (size_t k2 = k1+2; k2 < Nodes.size()*dk; k2 += dk)
        {
//                SignalNode sn1(QVector3D(params[k1],
//                                         params[k1+1],
//                                         Relief->CalcRealZbyRealXY(params[k1], params[k1+1]) ),
//                               Nodes[k1/dk].R);

            SignalNode sn1(QVector3D(params[k1],
                                    params[k1+1],
                                    Relief->CalcRealZbyRealXY(params[k1], params[k1+1])  ),
                          Nodes[k1/dk].R,
                          params[k1+2], //Nodes[k/dk].Alpha,
                          Nodes[k1/dk].Beta);

//                SignalNode sn2(QVector3D(params[k2],
//                                         params[k2+1],
//                                         Relief->CalcRealZbyRealXY(params[k2], params[k2+1])),
//                               Nodes[k2/dk].R);

            SignalNode sn2(QVector3D(params[k2],
                                    params[k2+1],
                                    Relief->CalcRealZbyRealXY(params[k2], params[k2+1])  ),
                          Nodes[k2/dk].R,
                          params[k2+2], //Nodes[k/dk].Alpha,
                          Nodes[k2/dk].Beta);

            double x = sn1.Pos.distanceToPoint(sn2.Pos); // Это расстояние в 3d !!! Считать его в 2d или 3d ?
            double R12 = sn1.R + sn2.R;

            R12 -= R_nodeOverlap;

            double step = (1-tanh(k_step_ot*(x-R12)));
            y2 += A2 * log(p*(offX+x))*step;
        }
    }

    y1 *= Aarf;

    return -(y1+y2);
//        return -y1;
}
//-------------------------------------------------------------

