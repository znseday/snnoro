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
            gluSphere(Quadric(), 0.02 + 0.05*sqrt(p.Weight), 12, 12);
            glPopMatrix();
        }

        if (isDrawAbonents)
        {
            const auto & abo = r.GetAbonent();
            double x = (abo.Pos.x()-offsetX)*k;
            double y = (abo.Pos.y()-offsetY)*k;
            double z;

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

            glColor3f(0.9f, 0.1f, 0.1f);

            glBegin(GL_LINE_LOOP);
            constexpr float kTri = 0.02f;
            glVertex3f(x-kTri, y, z);
            glVertex3f(x, y+2*kTri, z);
            glVertex3f(x+kTri, y, z);

            glEnd();
        }
    }

    glColor3f(0.9, 0.1, 0.1);
    glPointSize(3.0f);
    glLineWidth(1.0f);
    for (const auto &node : Nodes)
    {
//        glBegin(GL_POINTS);
//        //cout << node << endl;
//        glVertex3f((node.Pos.x()-offsetX)*k, (node.Pos.y()-offsetY)*k, 0);
//        glEnd();

        glPushMatrix();
        double x = (node.Pos.x()-offsetX)*k;
        double y = (node.Pos.y()-offsetY)*k;
        double z;


        if (Relief->GetIsMathRelief())
        {
            // z = ???;
            glTranslatef(x, y, zOffset + (Settings3d.IsPerspective ? Relief->CalcNormZbyNormXY(x, y) : 0));
        }
        else
        {
//            glTranslatef(x, y, zOffset + (Settings3d.IsPerspective ? Relief->CalcNormZbyRealXY(node.Pos.x(), node.Pos.y()) : 0));
            z = (node.Pos.z()-offsetZ)*Relief->Get_kz();
            glTranslatef(x, y, zOffset + (Settings3d.IsPerspective ? z : 0));
        }
        gluSphere(Quadric(), 0.02, 12, 12);
        glPopMatrix();

        const int nr = 32;
        double dfi, fiStart;

        if (_snt == SignalNodeType::Sphere)
        {
            dfi = 2.0*M_PI/nr;
            fiStart = 0;
        }
        else if (_snt == SignalNodeType::Cone)
        {
            dfi = 2.0*node.Beta / nr;
            fiStart = node.Alpha - node.Beta;
        }
        else
        {
            qDebug() << "Error: SignalNodeType is Unknown";
            dfi = 2*M_PI/nr;
            fiStart = 0;
        }

        glBegin(GL_LINE_STRIP);

        if (_snt == SignalNodeType::Cone) // далее будет копипаст этого ифа
        {
            double x = (node.Pos.x()-offsetX)*k;
            double y = (node.Pos.y()-offsetY)*k;
            double z;

            if (Relief->GetIsMathRelief())
            {
                glVertex3f(x, y, zOffset + (Settings3d.IsPerspective ? Relief->CalcNormZbyNormXY(x, y) : 0));
            }
            else
            {
                z = zOffset + (Settings3d.IsPerspective ? Relief->CalcNormToRealZbyRealXY(node.Pos.x(), node.Pos.y()) : 0);
                glVertex3f(x, y, z);
            }
        }

        for (int i = 0; i <= nr; i++)
        {
            double xt = node.Pos.x() + node.R*cos(fiStart + i*dfi);
            double yt = node.Pos.y() + node.R*sin(fiStart + i*dfi);
            double x = (xt-offsetX)*k;
            double y = (yt-offsetY)*k;
            double z;

            if (Relief->GetIsMathRelief())
            {
                // z = ???;         
                glVertex3f(x, y, zOffset + (Settings3d.IsPerspective ? Relief->CalcNormZbyNormXY(x, y) : 0));
            }
            else
            {
//                glVertex3f(x, y, zOffset + (Settings3d.IsPerspective ? Relief->CalcNormZbyRealXY(xt, yt) : 0));
                z = zOffset + (Settings3d.IsPerspective ? Relief->CalcNormToRealZbyRealXY(xt, yt) : 0);
                glVertex3f(x, y, z);
            }
        }

        if (_snt == SignalNodeType::Cone) // здесь копипаст
        {
            double x = (node.Pos.x()-offsetX)*k;
            double y = (node.Pos.y()-offsetY)*k;
            double z;

            if (Relief->GetIsMathRelief())
            {
                glVertex3f(x, y, zOffset + (Settings3d.IsPerspective ? Relief->CalcNormZbyNormXY(x, y) : 0));
            }
            else
            {
                z = zOffset + (Settings3d.IsPerspective ? Relief->CalcNormToRealZbyRealXY(node.Pos.x(), node.Pos.y()) : 0);
                glVertex3f(x, y, z);
            }
        }

        glEnd();

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
                                IGradDrawable *pGLWidget)
{
    cout << endl << "Grad Descent Started" << endl;

    for (auto & node : Nodes)
    {
        node.Bonds.clear();
    }
    Stats.Reset();

    const int param_count = Nodes.size()*2;
    size_t rCount = 0;
    for (const auto & route : Routes)
        rCount += route.Points.size();
    rCount *= Nodes.size();
    cout << "rCount = " << rCount << endl;

//    auto targetFunction = [this, rCount](const vector<double>& params)
//    {
//        vector<double> rs(rCount);
//        size_t i = 0;

//        for (size_t k = 0; k < Nodes.size()*2; k += 2)
//        {
//            for (const auto & route : Routes)
//            {
//                for (const auto & p : route.Points)
//                {
//                    rs[i++] = (p.Weight) * ( (p.Pos.x()-params[k])*(p.Pos.x()-params[k]) + (p.Pos.y()-params[k+1])*(p.Pos.y()-params[k+1]));
//                }
//            }
//        }
//        return accumulate(rs.begin(), rs.end(), 0);
//    };

//    auto targetFunction = [this](const vector<double>& params)
//    {
//        double y = 0;
//        for (size_t k = 0; k < Nodes.size()*2; k += 2)
//        {
//            for (const auto & route : Routes)
//            {
//                for (const auto & p1 : route.Points)
//                {
//                    SignalNode sn(QVector3D(params[k], params[k+1], 0), Nodes[k/2].R);
//                    y += p1.Weight*sn.accessRateF(p1.Pos);
//                }
//            }
//        }
//        y = -y*1e8;
//        return y;
//    };

    auto targetFunction = [this, &_targetFuncSettings](const vector<double>& params)
    {
        double y1 = 0;
        for (size_t k = 0; k < Nodes.size()*2; k += 2)
        {
//            SignalNode sn(QVector3D(params[k], params[k+1], 0), Nodes[k/2].R);

            SignalNode sn(QVector3D(params[k],
                                    params[k+1],
                                    //Nodes[k/2].Pos.z() // ??????
                                    Relief->CalcRealZbyRealXY(params[k], params[k+1])  ),
                          Nodes[k/2].R);


            for (auto & route : Routes)
            {
                for (auto & p1 : route.Points)
                {

                    double y = _targetFuncSettings.Aarf * sn.accessRateF(p1.Pos);

                    if (_targetFuncSettings.IsUseLineBetweenTwoPoints)
                    {
                        y *= IsLineBetweenTwoPoints(sn.Pos, p1.Pos);
                    }

//                    double y; // = _targetFuncSettings.Aarf * sn.accessRateF(p1.Pos);

//                    if ( IsLineBetweenTwoPoints(sn.Pos, p1.Pos) )
//                        y = _targetFuncSettings.Aarf * sn.accessRateF(p1.Pos);
//                    else
//                        y = 0;

//                    (1-tanh(k_step*(x-R12)))
//                    double x = (sn.Pos.x()-p1.Pos.x())*(sn.Pos.x()-p1.Pos.x()) + (sn.Pos.y()-p1.Pos.y())*(sn.Pos.y()-p1.Pos.y());
                    //double x = sn.Pos.distanceToPoint(p1.Pos);
                    //x = sqrt(x);
                    double w = p1.Weight;
                    //if (w > 0.95)
                    //    w = 1.1;

                    //y = pow(y, sqrt(w));
                    y = w*y;//*(1-tanh(k_step*(x-sn.R)));

                    if (_targetFuncSettings.IsUseCoveredFlag && !p1.IsCovered)
                    {
//                        y = pow(y, 1.1);
                        y *= 2;
                        //y += 100*Aarf;
                    }

                    y1 += y;

                    //y1 += Aarf * w * (1-tanh(k_step*(x-sn.R)));
                    //y1 += pow( Aarf *(1-tanh(k_step*(x-sn.R))) ,  sqrt(w));
                }
            }
        }

        //if (_targetFuncSettings.IsUseCoveredFlag)
        //{
            FindCoveredPointsUsingParams(params);

        //}

        double y2 = 0;
        for (size_t k1 = 0; k1 < (Nodes.size()-1)*2; k1 += 2)
        {
            for (size_t k2 = k1+2; k2 < Nodes.size()*2; k2 += 2)
            {
//                SignalNode sn1(QVector3D(params[k1], params[k1+1], 0), Nodes[k1/2].R);
                SignalNode sn1(QVector3D(params[k1],
                                         params[k1+1],
//                                         Nodes[k1/2].Pos.z()
                                         Relief->CalcRealZbyRealXY(params[k1], params[k1+1]) ),
                               Nodes[k1/2].R);

//                SignalNode sn2(QVector3D(params[k2], params[k2+1], 0), Nodes[k2/2].R);
                SignalNode sn2(QVector3D(params[k2],
                                         params[k2+1],
//                                            Nodes[k2/2].Pos.z()
                                         Relief->CalcRealZbyRealXY(params[k2], params[k2+1])),
                               Nodes[k2/2].R);

                double x = sn1.Pos.distanceToPoint(sn2.Pos); // Это расстояние в 3d !!! Считать его в 2d или 3d ?
                double R12 = sn1.R + sn2.R;

                R12 -= _targetFuncSettings.R_nodeOverlap;

                //=(1-TANH($I$4*(B3-$I$1)))
                double step = (1-tanh(_targetFuncSettings.k_step_ot*(x-R12)));
                y2 += _targetFuncSettings.A2 * log(_targetFuncSettings.p*(_targetFuncSettings.offX+x))*step;
                //y2 += A2*exp(p*(-x))*(1-tanh(k_step_ot*(x-R12)));
            }
        }

        return -(y1+y2);
    };

    GradDesc = _protoGD;

    GradDesc.SetUseUserTargetFunction(targetFunction);

    if (nDraw > 0)
    {
        GradDesc.SetCallback([this, pGLWidget]()
        {
            if (!IsGradCalculating)
                GradDesc.Stop();

            //cout << "Last Cost = " << GradDesc.GetLastCost() << endl;
            InitNodeCoordsFromParams(GradDesc.GetParams());
            //QApplication::processEvents();
            pGLWidget->Repaint();
            QApplication::processEvents();
            //Sleep(16);
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

    InitParamsFromNodeCoords(param_count);

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

    InitNodeCoordsFromParams(GradDesc.GetParams());

    CalcPointStats();

    Stats.LastCostPhase1 = GradDesc.GetLastCost();
    cout << "gd.GetLastCost() = " << GradDesc.GetLastCost() << endl;

    cout << "Iters: " << GradDesc.GetLastIters() << " out of " << GradDesc.GetMaxIters() << endl;
    cout << "Time: " << GradDesc.GetLastTime() << " out of " << GradDesc.GetMaxTime() << endl;

    CalcBonds(_targetFuncSettings);

    cout << "Grad Descent Finished" << endl << endl;

//    PrintBondsInfo();

    return true;
}
//----------------------------------------------------------

bool MyConfig::StartFinalGradDescent(int nDraw, const tf_gd_lib::GradDescent &_protoGD,
                                     const TargetFuncSettingsStruct &_targetFuncSettings,
                                     IGradDrawable *pGLWidget)
{
    cout << endl << "Grad Descent Started" << endl;

    const int param_count = Nodes.size()*2;

    auto targetFunction = [this, &_targetFuncSettings](const vector<double>& params)
    {
        double y1 = 0;
        for (size_t k = 0; k < Nodes.size()*2; k += 2)
        {
//            SignalNode sn(QVector3D(params[k], params[k+1], 0), Nodes[k/2].R);
            SignalNode sn(QVector3D(params[k],
                                    params[k+1],
                                  //Nodes[k/2].Pos.z() // ??????
                                  Relief->CalcRealZbyRealXY(params[k], params[k+1]) ),
                          Nodes[k/2].R);


            for (const auto & b : Nodes[k/2].Bonds)
            {
                const RoutePoint & p1 = Routes.at(std::get<0>(b)).Points.at(std::get<1>(b));

                double y = _targetFuncSettings.Aarf * sn.accessRateF(p1.Pos);

                y *= IsLineBetweenTwoPoints(sn.Pos, p1.Pos) ;

//                if ( IsLineBetweenTwoPoints(sn.Pos, p1.Pos) )
//                    y = _targetFuncSettings.Aarf * sn.accessRateF(p1.Pos);
//                else
//                    y = 0;


                double w = p1.Weight;
                y = w*y;//*(1-tanh(k_step*(x-sn.R)));

                y1 += y;

            }
        }

        return -y1;
    };

    GradDesc = _protoGD;

    GradDesc.SetUseUserTargetFunction(targetFunction);

    if (nDraw > 0)
    {
        GradDesc.SetCallback([this, pGLWidget]()
        {
            if (!IsGradCalculating)
                GradDesc.Stop();
            //cout << "Last Cost = " << GradDesc.GetLastCost() << endl;
            InitNodeCoordsFromParams(GradDesc.GetParams());
            //QApplication::processEvents();
            pGLWidget->Repaint();
            QApplication::processEvents();
            //Sleep(16);
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

    InitParamsFromNodeCoords(param_count);

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
    for (/*const*/ auto & route : Routes)
    {
        int iPoint = 0;
        for (/*const*/ auto & p1 : route.Points)
        {
            p1.IsCovered = false;
            for (size_t k = 0; k < Nodes.size(); ++k)
            {
                if (p1.Pos.distanceToPoint(Nodes[k].Pos) < Nodes[k].R)
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

    InitNodeCoordsFromParams(GradDesc.GetParams());

    cout << "Final Grad Descent Finished" << endl << endl;

    return true;
}
//----------------------------------------------------------

void MyConfig::InitNodeCoordsFromParams(const std::vector<double> & _params)
{
    for (size_t i = 0; i < _params.size(); i+=2)
    {
        Nodes.at(i/2).Pos.setX(_params[i]);
        Nodes.at(i/2).Pos.setY(_params[i+1]);

        Nodes.at(i/2).Pos.setZ( Relief->CalcRealZbyRealXY(_params[i], _params[i+1]) );
    }
}
//----------------------------------------------------------

void MyConfig::InitParamsFromNodeCoords(const int _param_count)
{
    vector<double> params(_param_count);
    vector<double> min_constrains(_param_count);
    vector<double> max_constrains(_param_count);
    vector<double> rel_constrains(_param_count); // relative constrains in %
    vector<bool> type_constrains(_param_count);  // set 'false' to use absolute constrains, 'true' - for relative


    const auto & area = Relief->GetArea();

    double min_x = area.right();
    double max_x = area.left();
    double min_y = area.bottom();
    double max_y = area.top();
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

void MyConfig::CalcBonds(const TargetFuncSettingsStruct &_targetFuncSettings)
{
    for (size_t iNode = 0; iNode < Nodes.size(); ++iNode)
    {
        for (size_t iRoute = 0; iRoute < Routes.size(); ++iRoute)
        {
            for (size_t iPoint = 0; iPoint < Routes[iRoute].Points.size(); ++iPoint)
            {
                double distToPoint = Routes[iRoute].Points[iPoint].Pos.distanceToPoint(Nodes[iNode].Pos);
                if (distToPoint < Nodes[iNode].R)
                {
                    double arf = Nodes[iNode].accessRateF(Routes[iRoute].Points[iPoint].Pos);

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
        }
    }

    //IsLineBetweenTwoPoints({1000, 1000, 1000}, {6000, 6000, 2000});// To delete!!!!!
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


