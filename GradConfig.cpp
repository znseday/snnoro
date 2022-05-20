#include "GradConfig.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include <algorithm>
#include <numeric>

#include <QApplication>
#include <QDebug>

using namespace std;

void MyConfig::StatsStruct::Reset()
{
    PercentOfCovered = 0;
    UncoveredCount = -1;
    LastCostPhase1 = 0;
    LastCostPhase2 = 0;
}
//----------------------------------------------------------

GLUquadric* MyConfig::Quadric()
{
    static GLUquadric* q = nullptr;
    if (!q) q = gluNewQuadric();
    return q;
}
//----------------------------------------------------------

//void MyConfig::SetRandomNodeCoords(const QRectF &_area)
void MyConfig::SetRandomNodeCoords()
{
    if (!Relief)
        throw std::runtime_error("There is no relief in MyConfig");

    for (auto & node : Nodes)
    {
        node.SetRandomCoord(*Relief);
        node.SetRandomAlpha();
    }
}
//----------------------------------------------------------

void MyConfig::DrawIn3D(SignalNodeType _snt, bool isDrawAbonents) const
{
    if (!Relief)
        throw std::runtime_error("Relief is not set");

//    glColor3f(0, 0, 0.7f);
//    glBegin(GL_LINE_STRIP);
//        glVertex2d(0, 0);
//        glVertex2d(1, 0);
//        glVertex2d(1, 1);
//        glVertex2d(0, 1);
//        glVertex2d(0, 0);
//    glEnd();

//    glPushMatrix();

    //glEnable(GL_DEPTH_TEST);

    if (Settings3d.IsPerspective)
    {
        glTranslatef(Settings3d.TrX, Settings3d.TrY, Settings3d.TrZ);
        glRotatef(Settings3d.RotX, 1, 0, 0);
        glRotatef(Settings3d.RotY, 0, 1, 0);
        glRotatef(Settings3d.RotZ, 0, 0, 1);
    }
    else
    {
        glTranslatef(Settings3d.TrX, Settings3d.TrY, 0);
        float ks = fabs(1.0/(Settings3d.TrZ+2.5));
        glScalef(ks, ks, 1.0);
    }

    const auto & area = Relief->GetArea();

    double kx = 2.0/area.width();
    double ky = 2.0/area.height();
    double k = min(kx, ky);
//    double k = max(kx, ky);

    double hW = area.width()/2.0;
    double hH = area.height()/2.0;

    double offsetX = area.x()+hW; // in meters
    double offsetY = area.y()+hH; // in meters
    double offsetZ = 0;

    glLineWidth(1.0f);

//    glColor3f(0.5f, 0.5f, 0.5f);
//    glBegin(GL_QUADS);
//        glVertex3f((Area.left()-offsetX)*k,  (Area.bottom()-offsetY)*k, -0.03f);
//        glVertex3f((Area.left()-offsetX)*k,  (Area.top()-offsetY)*k, -0.03f);
//        glVertex3f((Area.right()-offsetX)*k, (Area.top()-offsetY)*k, -0.03f);
//        glVertex3f((Area.right()-offsetX)*k, (Area.bottom()-offsetY)*k, -0.03f);
//    glEnd();


    glPushMatrix();
    glTranslatef(0, 0, -Relief->GetAverZ());

    Relief->Draw(!Settings3d.IsPerspective);

    constexpr float zOffset = 0.01f;

    for (const auto & r : Routes)
    {
        glColor3f(0.1f, 0.7f, 0.7f);

        glBegin(GL_LINE_STRIP);
        for (const auto & p : r.Points)
        {
            double x = (p.Pos.x()-offsetX)*k;
            double y = (p.Pos.y()-offsetY)*k;

            double z;

            if (Relief->GetIsMathRelief())
            {
//                z = ???;
                glVertex3f(x, y, zOffset + (Settings3d.IsPerspective ? Relief->CalcNormZbyNormXY(x, y) : 0));
            }
            else
            {
                //glVertex3f(x, y, zOffset + (Settings3d.IsPerspective ? Relief->CalcNormZbyRealXY(p.Pos.x(), p.Pos.y()) : 0));
                z = (p.Pos.z()-offsetZ)*Relief->Get_kz();
                glVertex3f(x, y, zOffset + (Settings3d.IsPerspective ? z : 0));
            }
        }
        glEnd();


        for (const auto & p : r.Points)
        {
            if (p.IsCovered)
                glColor3f(0.1f, 0.8f, 0);
            else
                glColor3f(0.1f, 0.1f, 0.8f);


            //glPointSize(3.0 + p.Weight*16.0);
            //glBegin(GL_POINTS);
            //glVertex3f((p.Pos.x()-offsetX)*k, (p.Pos.y()-offsetY)*k, 0);
            //glEnd();



            glPushMatrix();    
            double x = (p.Pos.x()-offsetX)*k;
            double y = (p.Pos.y()-offsetY)*k;
            double z;

            if (Relief->GetIsMathRelief())
            {
                // z = ???;
                glTranslatef(x, y, zOffset + (Settings3d.IsPerspective ? Relief->CalcNormZbyNormXY(x, y) : 0));
            }
            else
            {
//                glTranslatef(x, y, zOffset + (Settings3d.IsPerspective ? Relief->CalcNormZbyRealXY(p.Pos.x(), p.Pos.y()) : 0));

                z = (p.Pos.z()-offsetZ)*Relief->Get_kz();
                glTranslatef(x, y, zOffset + (Settings3d.IsPerspective ? z : 0));
            }
            gluQuadricDrawStyle(Quadric(), GLU_FILL);
            gluSphere(Quadric(), 0.02 + 0.05*sqrt(p.Weight), 12, 12);
            glPopMatrix();
        }

        if (/*false && */isDrawAbonents)
        {
            const auto & abo = r.GetAbonent();
            double x = (abo.Pos.x()-offsetX)*k;
            double y = (abo.Pos.y()-offsetY)*k;
            double z = 0;

            if (Relief->GetIsMathRelief())
            {
                // z = ???;
                glTranslatef(x, y, zOffset + (Settings3d.IsPerspective ? Relief->CalcNormZbyNormXY(x, y) : 0));
            }
            else
            {
//                glTranslatef(x, y, zOffset + (Settings3d.IsPerspective ? Relief->CalcNormZbyRealXY(p.Pos.x(), p.Pos.y()) : 0));
                z = (abo.Pos.z()-offsetZ)*Relief->Get_kz();
            }

            constexpr float kTri = 0.02f;

            glPushMatrix();         

            const auto & q = abo.q;

            double tetha = acos(q.z() / q.length()) * 180.0 / M_PI;
            double fi;
            if (qFuzzyCompare(q.toVector2D().length(), 0))
                fi = 0;
            else
                fi = acos(q.y() / q.toVector2D().length()) * 180.8 / M_PI;

            glTranslatef(x, y, z);

            //glDisable(GL_DEPTH_TEST);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glBegin(GL_QUADS);
            constexpr float hr = 0.03;
            constexpr float wr = 0.25;

            float wrr = abo.accessRate / 1.0 * wr;
            constexpr double endHue = 120;
            int hu = /*startHue -*/ -20 + (abo.accessRate - 0)/(1.0 - 0)*endHue;
            if (hu > 120)
                hu = 120;
            if (hu < 0)
                hu += 360;

            QColor res = QColor::fromHsv(hu, 220, 150);
        //    qDebug() << res.hslHue() << res.hslSaturation() << res.lightness();
        //    qDebug() << res.isValid();
            res = res.toRgb();

            glColor3f(res.redF(), res.greenF(), res.blueF());
            glVertex3f(0, 0, 0);
            glVertex3f(0, hr, 0);
            glVertex3f(wrr, hr, 0);
            glVertex3f(wrr, 0, 0);
            glEnd();

            glLineWidth(2.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glBegin(GL_QUADS);
            glColor3f(0, 0, 0);
            glVertex3f(0, 0, 0);
            glVertex3f(0, hr, 0);
            glVertex3f(wr, hr, 0);
            glVertex3f(wr, 0, 0);
            glEnd();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glColor3f(0.9f, 0.1f, 0.1f);
            glRotatef( (q.x() > 0) ? -fi : fi, 0, 0, 1);
            glRotatef(-tetha, 1, 0, 0);

            glTranslatef(0, 0, -kTri*2);

            glLineWidth(1.0);
            gluQuadricDrawStyle(Quadric(), GLU_LINE);
            gluCylinder(Quadric(), kTri, 0, kTri*4, 16, 4);

            //glEnable(GL_DEPTH_TEST);
            glPopMatrix();

//            qDebug() << "abo =" << abo.Pos << ", ar =" << abo.accessRate;
//            glBegin(GL_LINE_LOOP);
//            glVertex3f(x-kTri, y - kTri,     z + zOffset);
//            glVertex3f(x,      y + kTri,     z + zOffset);
//            glVertex3f(x+kTri, y - kTri,     z + zOffset);
//            glEnd();
        }
    }



    for (const auto &node : Nodes)
    {
        glColor3f(0.9, 0.1, 0.1);
        glPointSize(3.0f);
        glLineWidth(1.0f);

        node.DrawIn3D(_snt, Relief, Settings3d);
    }

    glPopMatrix();

    glGetIntegerv(GL_VIEWPORT, vport);
    glGetDoublev(GL_MODELVIEW_MATRIX, modl);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
}
//----------------------------------------------------------

void MyConfig::CancelGradDescent()
{
    IsGradCalculating = false;
}
//----------------------------------------------------------

bool MyConfig::StartGradDescent(int nDraw, const tf_gd_lib::GradDescent &_protoGD,
                                const TargetFuncSettingsStruct &_targetFuncSettings,
                                SignalNodeType _snt,
                                IGradDrawable *pGLWidget)
{
    cout << endl << "Grad Descent Started" << endl;

    for (auto & node : Nodes)
    {
        node.Bonds.clear();
    }
    Stats.Reset();

    int param_count;
    if (_snt == SignalNodeType::Sphere)
        param_count = Nodes.size()*2;
    else if (_snt == SignalNodeType::Cone)
        param_count = Nodes.size()*3;
    else
        throw std::runtime_error("Unknown _snt");


    size_t rCount = 0;
    for (const auto & route : Routes)
        rCount += route.Points.size();
    rCount *= Nodes.size();
    cout << "rCount = " << rCount << endl;

    auto targetFunctionAdditive = [this, &_targetFuncSettings](const vector<double>& params)
    {
        double sum_w_of_routes = 0;
        for (auto & route : Routes)
        {
            sum_w_of_routes += route.Get_w();
        }

        double y1 = 0;
        for (size_t k = 0; k < Nodes.size()*2; k += 2)
        {
            SignalNode sn(QVector3D(params[k],
                                    params[k+1],
                                    Relief->CalcRealZbyRealXY(params[k], params[k+1])  ),
                          Nodes[k/2].R);


            for (auto & route : Routes)
            {
                for (auto & p1 : route.Points)
                {

                    double y = /*_targetFuncSettings.Aarf * */ sn.accessRateSphere(p1.Pos);

                    if (_targetFuncSettings.IsUseLineBetweenTwoPoints)
                    {
                        y *= IsLineBetweenTwoPoints(sn.Pos, p1.Pos);
                    }


                    double w = p1.Weight;  // !!!!!!!!!!!!!!
                    y *= w;                   //*(1-tanh(k_step*(x-sn.R)));

                    if (_targetFuncSettings.IsUseCoveredFlag && !p1.IsCovered)
                    {
                        y *= 2;
                    }

                    y1 += y;
                }

                y1 *= (route.Get_w() / sum_w_of_routes);
            }
        }

        FindCoveredPointsUsingParams(params);

        double y2 = 0;
        for (size_t k1 = 0; k1 < (Nodes.size()-1)*2; k1 += 2)
        {
            for (size_t k2 = k1+2; k2 < Nodes.size()*2; k2 += 2)
            {
                SignalNode sn1(QVector3D(params[k1],
                                         params[k1+1],
                                         Relief->CalcRealZbyRealXY(params[k1], params[k1+1]) ),
                               Nodes[k1/2].R);

                SignalNode sn2(QVector3D(params[k2],
                                         params[k2+1],
                                         Relief->CalcRealZbyRealXY(params[k2], params[k2+1])),
                               Nodes[k2/2].R);

                double x = sn1.Pos.distanceToPoint(sn2.Pos); // Это расстояние в 3d !!! Считать его в 2d или 3d ?
                double R12 = sn1.R + sn2.R;

                R12 -= _targetFuncSettings.R_nodeOverlap;

                double step = (1-tanh(_targetFuncSettings.k_step_ot*(x-R12)));
                y2 += _targetFuncSettings.A2 * log(_targetFuncSettings.p*(_targetFuncSettings.offX+x))*step;
            }
        }

        y1 *= _targetFuncSettings.Aarf;

        return -(y1+y2);
//        return -y1;
    };


    auto targetFunctionProbabilistic = [this, &_targetFuncSettings](const vector<double>& params)
    {
        double sum_w_of_routes = 0;
        for (auto & route : Routes)
            sum_w_of_routes += route.Get_w();

        double y1 = 0;

        for (auto & route : Routes) // все точки всех маршрутов
        {
            for (auto & p1 : route.Points) // цикл по точкаи одного маршрута
            {
                double s = 0;
                for (size_t k = 0; k < Nodes.size()*2; k += 2)
                {
                    SignalNode sn(QVector3D(params[k],
                                                params[k+1],
                                                Relief->CalcRealZbyRealXY(params[k], params[k+1])  ),
                                      Nodes[k/2].R);

                    double y = /* _targetFuncSettings.Aarf * */ sn.accessRateSphere(p1.Pos);

                    if (_targetFuncSettings.IsUseLineBetweenTwoPoints)
                    {
                        y *= IsLineBetweenTwoPoints(sn.Pos, p1.Pos);
                    }


                    double w = p1.Weight;  // !!!!!!!!!!!!!!
//                    y *= w;                   //*(1-tanh(k_step*(x-sn.R)));

//                    qDebug() << "p1 =" << p1.Pos << ": w =" << w;

                    if (_targetFuncSettings.IsUseCoveredFlag && !p1.IsCovered)
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

        FindCoveredPointsUsingParams(params);

        double y2 = 0;
        for (size_t k1 = 0; k1 < (Nodes.size()-1)*2; k1 += 2)
        {
            for (size_t k2 = k1+2; k2 < Nodes.size()*2; k2 += 2)
            {
                SignalNode sn1(QVector3D(params[k1],
                                         params[k1+1],
                                         Relief->CalcRealZbyRealXY(params[k1], params[k1+1]) ),
                               Nodes[k1/2].R);

                SignalNode sn2(QVector3D(params[k2],
                                         params[k2+1],
                                         Relief->CalcRealZbyRealXY(params[k2], params[k2+1])),
                               Nodes[k2/2].R);

                double x = sn1.Pos.distanceToPoint(sn2.Pos); // Это расстояние в 3d !!! Считать его в 2d или 3d ?
                double R12 = sn1.R + sn2.R;

                R12 -= _targetFuncSettings.R_nodeOverlap;

                double step = (1-tanh(_targetFuncSettings.k_step_ot*(x-R12)));
                y2 += _targetFuncSettings.A2 * log(_targetFuncSettings.p*(_targetFuncSettings.offX+x))*step;
            }
        }

        y1 *= _targetFuncSettings.Aarf;

        return -(y1+y2);
//        return -y1;
    };


    GradDesc = _protoGD;

    if (_targetFuncSettings.TargetFuncType == TargetFuncEnum::Additive)
        GradDesc.SetUseUserTargetFunction(targetFunctionAdditive);
    else if (_targetFuncSettings.TargetFuncType == TargetFuncEnum::Probabilistic)
        GradDesc.SetUseUserTargetFunction(targetFunctionProbabilistic);
    else
        throw std::runtime_error("Unknown or Empty TargetFuncEnum in StartGradDescent");


    if (nDraw > 0)
    {
        GradDesc.SetCallback([this, pGLWidget, _snt]()
        {
            if (!IsGradCalculating)
                GradDesc.Stop();

            InitNodeCoordsFromParams(GradDesc.GetParams(), _snt);
            pGLWidget->Repaint();
            QApplication::processEvents();
        });
    }
    else
    {
        GradDesc.SetCallback([this]()
        {
            if (!IsGradCalculating)
                GradDesc.Stop();
            QApplication::processEvents();
        });
    }

    InitParamsFromNodeCoords(param_count, _snt);

    IsGradCalculating = true;
    tf_gd_lib::GradErrorType res = GradDesc.Go();
    IsGradCalculating = false;

    cout << "gd_user_target_function result: ";
    switch (res)
    {
    case tf_gd_lib::GradErrorType::Success:
        cout << "Success" << endl;
        break;
    case tf_gd_lib::GradErrorType::VectorSizesNotTheSame:
        cout << "VectorSizesNotTheSame" << endl;
        break;
    case tf_gd_lib::GradErrorType::CanceledByUser:
        cout << "CanceledByUser" << endl;
        break;
    case tf_gd_lib::GradErrorType::TimeOut:
        cout << "TimeOut" << endl;
        break;
    case tf_gd_lib::GradErrorType::ItersOverflow:
        cout << "ItersOverflow" << endl;
        break;
    }


    CalcPointStats();

    Stats.LastCostPhase1 = GradDesc.GetLastCost();
    cout << "gd.GetLastCost() = " << GradDesc.GetLastCost() << endl;

    cout << "Iters: " << GradDesc.GetLastIters() << " out of " << GradDesc.GetMaxIters() << endl;
    cout << "Time: " << GradDesc.GetLastTime() << " out of " << GradDesc.GetMaxTime() << endl;

    InitNodeCoordsFromParams(GradDesc.GetParams(), _snt);

    CalcBonds(_targetFuncSettings, _snt);

    cout << "Grad Descent Finished" << endl << endl;

//    PrintBondsInfo();

    return true;
}
//----------------------------------------------------------

bool MyConfig::StartFinalGradDescent(int nDraw, const tf_gd_lib::GradDescent &_protoGD,
                                     const TargetFuncSettingsStruct &_targetFuncSettings,
                                     SignalNodeType _snt,
                                     IGradDrawable *pGLWidget)
{
    cout << endl << "Grad Descent Started" << endl;

    int param_count;
    if (_snt == SignalNodeType::Sphere)
        param_count = Nodes.size()*2;
    else if (_snt == SignalNodeType::Cone)
        param_count = Nodes.size()*3;
    else
        throw std::runtime_error("Unknown _snt");

    auto targetFunction = [this, &_targetFuncSettings](const vector<double>& params)
    {
        double y1 = 0;
        for (size_t k = 0; k < Nodes.size()*2; k += 2)
        {
            SignalNode sn(QVector3D(params[k],
                                    params[k+1],
                                  Relief->CalcRealZbyRealXY(params[k], params[k+1]) ),
                          Nodes[k/2].R);


            for (const auto & b : Nodes[k/2].Bonds)
            {
                const RoutePoint & p1 = Routes.at(std::get<0>(b)).Points.at(std::get<1>(b));

                double y = _targetFuncSettings.Aarf * sn.accessRateSphere(p1.Pos);

                if (_targetFuncSettings.IsUseLineBetweenTwoPoints)
                {
                    y *= IsLineBetweenTwoPoints(sn.Pos, p1.Pos);
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
    };

    GradDesc = _protoGD;

    GradDesc.SetUseUserTargetFunction(targetFunction);

    if (nDraw > 0)
    {
        GradDesc.SetCallback([this, pGLWidget, _snt]()
        {
            if (!IsGradCalculating)
                GradDesc.Stop();
            InitNodeCoordsFromParams(GradDesc.GetParams(), _snt);
            pGLWidget->Repaint();
            QApplication::processEvents();
        });
    }
    else
    {
        GradDesc.SetCallback([this]()
        {
            if (!IsGradCalculating)
                GradDesc.Stop();
            QApplication::processEvents();
        });
    }

    InitParamsFromNodeCoords(param_count, _snt);

    IsGradCalculating = true;
    tf_gd_lib::GradErrorType res = GradDesc.Go();
    IsGradCalculating = false;

    cout << "gd_user_target_function result: ";
    switch (res)
    {
    case tf_gd_lib::GradErrorType::Success:
        cout << "Success" << endl;
        break;
    case tf_gd_lib::GradErrorType::VectorSizesNotTheSame:
        cout << "VectorSizesNotTheSame" << endl;
        break;
    case tf_gd_lib::GradErrorType::CanceledByUser:
        cout << "CanceledByUser" << endl;
        break;
    case tf_gd_lib::GradErrorType::TimeOut:
        cout << "TimeOut" << endl;
        break;
    case tf_gd_lib::GradErrorType::ItersOverflow:
        cout << "ItersOverflow" << endl;
        break;
    }

    int iRoute = 0;
    for (/*const*/ auto & route : Routes) // сделать это всё и для Cone !!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
    {
        int iPoint = 0;
        for (/*const*/ auto & p1 : route.Points)
        {
            p1.IsCovered = false;
            for (size_t k = 0; k < Nodes.size(); ++k)
            {
                if (p1.Pos.distanceToPoint(Nodes[k].Pos) < Nodes[k].R) // А для Cone ??????!!!!!!
                {
                    p1.IsCovered = true;
                    break;
                }
            }
            iPoint++;
        }
        iRoute++;
    }

    CalcPointStats();

    Stats.LastCostPhase2 = GradDesc.GetLastCost();
    cout << "gd.GetLastCost() = " << GradDesc.GetLastCost() << endl;

    cout << "Iters: " << GradDesc.GetLastIters() << " out of " << GradDesc.GetMaxIters() << endl;
    cout << "Time: " << GradDesc.GetLastTime() << " out of " << GradDesc.GetMaxTime() << endl;

    InitNodeCoordsFromParams(GradDesc.GetParams(), _snt);

    cout << "Final Grad Descent Finished" << endl << endl;

    return true;
}
//----------------------------------------------------------

void MyConfig::InitNodeCoordsFromParams(const std::vector<double> & _params, SignalNodeType _snt)
{
    if (_snt == SignalNodeType::Sphere)
    {
        for (size_t i = 0; i < _params.size(); i+=2)
        {
            Nodes.at(i/2).Pos.setX(_params[i]);
            Nodes.at(i/2).Pos.setY(_params[i+1]);

            Nodes.at(i/2).Pos.setZ( Relief->CalcRealZbyRealXY(_params[i], _params[i+1]) );
        }
    }
    else if (_snt == SignalNodeType::Cone)
    {
        for (size_t i = 0; i < _params.size(); i+=3)
        {
            Nodes.at(i/3).Pos.setX(_params[i]);
            Nodes.at(i/3).Pos.setY(_params[i+1]);
            Nodes.at(i/3).Pos.setZ( Relief->CalcRealZbyRealXY(_params[i], _params[i+1]) );

            Nodes.at(i/3).Alpha = _params[i+2];
        }
    }
    else
        throw std::runtime_error("Unknown _snt in MyConfig::InitNodeCoordsFromParams");
}
//----------------------------------------------------------

void MyConfig::InitParamsFromNodeCoords(const int _param_count, SignalNodeType _snt)
{
    if (!(_snt == SignalNodeType::Sphere || _snt == SignalNodeType::Cone))
        throw std::runtime_error("Unknown _snt in MyConfig::InitParamsFromNodeCoords");

    vector<double> params(_param_count);
    vector<double> min_constrains(_param_count);
    vector<double> max_constrains(_param_count);
    vector<double> rel_constrains(_param_count); // relative constrains in %
    vector<bool> type_constrains(_param_count);  // set 'false' to use absolute constrains, 'true' - for relative


    const auto & area = Relief->GetArea();

    double min_x = area.right();   // Для поиска
    double max_x = area.left();
    double min_y = area.bottom();
    double max_y = area.top();

//    double min_x = area.left(); // для жесткий ограничений без поиска
//    double max_x = area.right();
//    double min_y = area.top();
//    double max_y = area.bottom();

    for (const auto & route : Routes)
    {
        for (const auto & p1 : route.Points)
        {
            if (p1.Pos.x() < min_x)
                min_x = p1.Pos.x();
            if (p1.Pos.y() < min_y)
                min_y = p1.Pos.y();

            if (p1.Pos.x() > max_x)
                max_x = p1.Pos.x();
            if (p1.Pos.y() > max_y)
                max_y = p1.Pos.y();
        }
    }

    size_t i = 0;
    for (const auto & node : Nodes)
    {
        params[i] = node.Pos.x();
//        min_constrains[i] = Area.left();
//        max_constrains[i] = Area.right();
        min_constrains[i] = min_x;
        max_constrains[i] = max_x;
        rel_constrains[i] = 50; // not used yet
        type_constrains[i] = false; // use absolute constrains
        i++;

        params[i] = node.Pos.y();
//        min_constrains[i] = Area.top();
//        max_constrains[i] = Area.bottom();
        min_constrains[i] = min_y;
        max_constrains[i] = max_y;
        rel_constrains[i] = 50; // not used yet
        type_constrains[i] = false; // use absolute constrains
        i++;

        if (_snt == SignalNodeType::Cone)
        {
            params[i] = node.Alpha;
            min_constrains[i] = -2*M_PI; // ???
            max_constrains[i] = +2*M_PI; // ???
//            min_constrains[i] = min_y;
//            max_constrains[i] = max_y;
            rel_constrains[i] = 50; // not used yet
            type_constrains[i] = false; // use absolute constrains
            i++;
        }
    }

    GradDesc.SetParams(params);
    GradDesc.SetMinConstrains(min_constrains);
    GradDesc.SetMaxConstrains(max_constrains);
    GradDesc.SetRelConstrains(rel_constrains);
    GradDesc.SetTypeConstrains(type_constrains);
}
//----------------------------------------------------------

void MyConfig::FindCoveredPointsUsingParams(const std::vector<double> &params)
{
    int iRoute = 0;
    for (/*const*/ auto & route : Routes)
    {
        int iPoint = 0;
        for (/*const*/ auto & p1 : route.Points)
        {
            p1.IsCovered = false;
            for (size_t k = 0; k < Nodes.size()*2; k += 2)
            {
//                SignalNode sn(QVector3D(params[k], params[k+1], 0), Nodes[k/2].R);
                SignalNode sn(QVector3D(params[k],
                                        params[k+1],
                                        Relief->CalcRealZbyRealXY(params[k], params[k+1])),
                                        Nodes[k/2].R);
                if (p1.Pos.distanceToPoint(sn.Pos) < sn.R) // Сравниваем расстояния в 3d! Придумать что-то еще?
                {
                    p1.IsCovered = true;
                    break;
                }
            }
            iPoint++;
        }
        iRoute++;
    }
}
//----------------------------------------------------------

bool MyConfig::CheckIsAllCovered() const
{
    bool res = true;
    for (auto & route : Routes)
        for (auto & p1 : route.Points)
            if (!p1.IsCovered)
                return false;
    return res;
}
//----------------------------------------------------------

void MyConfig::CalcBonds(const TargetFuncSettingsStruct &_targetFuncSettings, SignalNodeType _snt)
{
    for (size_t iNode = 0; iNode < Nodes.size(); ++iNode)
    {
        for (size_t iRoute = 0; iRoute < Routes.size(); ++iRoute)
        {
            for (size_t iPoint = 0; iPoint < Routes[iRoute].Points.size(); ++iPoint)
            {
                double distToPoint;

                if (_snt == SignalNodeType::Sphere)
                {
                    distToPoint = Routes[iRoute].Points[iPoint].Pos.distanceToPoint(Nodes[iNode].Pos);

                    if (distToPoint < Nodes[iNode].R)
                    {
                        double arf = Nodes[iNode].accessRateSphere(Routes[iRoute].Points[iPoint].Pos);

                        if (_targetFuncSettings.IsUseLineBetweenTwoPoints )
                        {
                            arf *= IsLineBetweenTwoPoints(Nodes[iNode].Pos, Routes[iRoute].Points[iPoint].Pos);

    //                      if ( IsLineBetweenTwoPoints(Nodes[iNode].Pos, Routes[iRoute].Points[iPoint].Pos) )
    //                          arf = Nodes[iNode].accessRateF(Routes[iRoute].Points[iPoint].Pos);
    //                      else
    //                          arf = 0;
                        }

                        Nodes[iNode].Bonds.emplace(iRoute, iPoint, arf, distToPoint/Nodes[iNode].R);
                    }
                }
                else if (_snt == SignalNodeType::Cone)
                {
                    // to do (to test)

                    // ????
                    distToPoint = Routes[iRoute].Points[iPoint].Pos.distanceToPoint(Nodes[iNode].Pos);
//                    distToPoint = для Cone (учесть, что центр в другом месте)

                    if (distToPoint < Nodes[iNode].R) // R переделать на другой критерий
                    {
                        double arf = Nodes[iNode].accessRateCone(Routes[iRoute].Points[iPoint].Pos);

                        if (_targetFuncSettings.IsUseLineBetweenTwoPoints)
                        {
                            // Что делать с этим?
                            arf *= IsLineBetweenTwoPoints(Nodes[iNode].Pos, Routes[iRoute].Points[iPoint].Pos);

    //                      if ( IsLineBetweenTwoPoints(Nodes[iNode].Pos, Routes[iRoute].Points[iPoint].Pos) )
    //                          arf = Nodes[iNode].accessRateF(Routes[iRoute].Points[iPoint].Pos);
    //                      else
    //                          arf = 0;
                        }

                        // Строчкой ниже R на что заменить?
                        Nodes[iNode].Bonds.emplace(iRoute, iPoint, arf, distToPoint/Nodes[iNode].R);
                    }
                }
                else
                    throw std::runtime_error("Unknown _snt in MyConfig::CalcBonds");
            }
        }
    }
}
//----------------------------------------------------------

void MyConfig::FillExternVportModlAndProj(GLint _vport[4], GLdouble _modl[16], GLdouble _proj[16]) const
{
    memcpy(_vport, vport, sizeof(vport));
    memcpy(_modl,  modl,  sizeof(modl));
    memcpy(_proj,  proj,  sizeof(proj));
}
//----------------------------------------------------------

double MyConfig::IsLineBetweenTwoPoints(const Pos3d &_p1, const Pos3d &_p2) const
{
    double result = 1.0;

    double dRay = Relief->GetDeltaDiag();
   // qDebug() << "dRay  =" << dRay;

   // Pos3d p1 = _p1;
    //p1.setZ(Relief->CalcRealZbyRealXY(p1.x(), p1.y()));
    //Pos3d p2 = _p2;
    //p2.setZ(Relief->CalcRealZbyRealXY(p2.x(), p2.y()));

    double len = (_p1.toVector2D() - _p2.toVector2D()).length();
    int n = 2 + int( len / dRay );
    dRay = len / (n-1);
  //  qDebug() << "n =" << n;
  //  qDebug() << "dRay2 =" << dRay;

    QVector2D d = (_p2.toVector2D() - _p1.toVector2D()).normalized() * dRay;
//    Pos3d p;
    QVector2D p;
    QVector2D p0 = _p1.toVector2D();

    double lenZ = _p2.z() - _p1.z();
    double dz = lenZ / (n-1);
    double LineZ;

    for (int i = 0; i < n; ++i)
    {
        p = p0 + i*d;
        double RealZ = Relief->CalcRealZbyRealXY(p.x(), p.y());

        //double k = (p - p0).length() / len;
        //double LineZ = p1.z() + lenZ*k;

        LineZ = _p1.z() + i*dz;

        //qDebug() << "RealZ / LineZ =" << RealZ << " / " << LineZ;

        constexpr double offZ = 20;
        if (RealZ > LineZ+offZ)
        {
            double k = (RealZ-(LineZ+offZ)) / RealZ;
            result -= k;
        }
    }

//    if ( qFuzzyCompare(p, p2.toVector2D()) )
//        qDebug() << "YES";
//    else
//        qDebug() << "NO";

//    if ( qFuzzyCompare(float(LineZ), p2.z()) )
//        qDebug() << "YES";
//    else
//        qDebug() << "NO";

    return (result > 0) ? result : 0;
}
//----------------------------------------------------------

void MyConfig::TestTwoLines() const
{
    for (const auto &p : Routes.front().Points)
    {
//        if ( IsLineBetweenTwoPoints(p.Pos, Nodes.front().Pos)  )
//        {
//            qDebug() << "YES " << p.Pos;
//        }
//        else
//        {
//            qDebug() << "NO " << p.Pos;
//        }

        qDebug() << "IsLineBetweenTwoPoints =" << IsLineBetweenTwoPoints(p.Pos, Nodes.front().Pos);
    }
}
//----------------------------------------------------------

void MyConfig::PrintBondsInfo() const
{
    cout << "PrintBondsInfo:" << endl;

    for (size_t iNode = 0; iNode < Nodes.size(); ++iNode)
    {
        cout << "Node " << iNode << " connected with:" << endl;

        for (const auto & b : Nodes[iNode].Bonds)
        {
            cout << "{"  << std::get<0>(b) << "; " << std::get<1>(b) <<
                    "; " << std::get<2>(b) << "; " << std::get<3>(b) << "}" << endl;
        }
    }

    cout << endl;
}
//----------------------------------------------------------

void MyConfig::CalcPointStats()
{
    size_t coveredCount = 0;
    size_t pointCount = 0;
    for (const auto & route : Routes)
        for (const auto & p1 : route.Points)
        {
            pointCount++;
            if (p1.IsCovered)
                coveredCount++;
        }
    Stats.UncoveredCount = pointCount - coveredCount;
    Stats.PercentOfCovered = coveredCount/double(pointCount);
}
//----------------------------------------------------------

void MyConfig::CalcAccessRateForAbos(bool _isUseLineBetweenTwoPoints, SignalNodeType _snt)
{
    for (auto & route : Routes)
    {
        auto & abo = route.AbonentDirectAccess();
        double y1 = 0;
        for (const auto & sn : Nodes)
        {
            double y;// = sn.accessRateSphere(abo.Pos);
            if (_snt == SignalNodeType::Sphere)
                y = sn.accessRateSphere(abo.Pos);
            else if (_snt == SignalNodeType::Cone)
                y = sn.accessRateCone(abo.Pos);
            else
                throw std::runtime_error("Unknown _snt in MyConfig::CalcAccessRateForAbos");

            if (_isUseLineBetweenTwoPoints)
            {
                y *= IsLineBetweenTwoPoints(sn.Pos, abo.Pos); // осторожно для Cone!
            }

            y1 += y;
        }
        abo.accessRate = y1;
    }
}
//----------------------------------------------------------


