#include "GradModel.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include <QApplication>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include <iostream>

#include "TargetFunctions/TargetFunctionsFirstPhase.h"
#include "TargetFunctions/TargetFunctionsSecondPhase.h"

#include "GradDescSaveLoad.h"

using namespace std;

static const QString GlobalSettingsFileName = "Settings_snnoro.json";

constexpr float RotSpeed = 0.12f;
constexpr float TransSpeed = 0.004f;


const QString SettingsDefaultDir = "Settings";
const QString SettingsGDExtension = "*.gds";
const QString SettingsTFExtension = "*.tfs";

MyGradModel::MyGradModel()
{
    ProtoGradDesc.SetIsUseUserTargetFunction(true);

    TargetFunctions.try_emplace("AdditiveSphereFirstPhase",
                                new TargetFuncAdditiveSphereFirstPhase());

    TargetFunctions.try_emplace("ProbabilisticSphereFirstPhase",
                                new TargetFuncProbabilisticSphereFirstPhase());

    TargetFunctions.try_emplace("AdditiveConeFirstPhase",
                                new TargetFuncAdditiveConeFirstPhase());

    TargetFunctions.try_emplace("ProbabilisticConeFirstPhase",
                                new TargetFuncProbabilisticConeFirstPhase());

    TargetFunctions.try_emplace("AdditiveSphereSecondPhase",
                                new TargetFuncAdditiveSphereSecondPhase());

    TargetFunctions.try_emplace("ProbabilisticSphereSecondPhase",
                                new TargetFuncProbabilisticSphereSecondPhase());

    TargetFunctions.try_emplace("AdditiveConeSecondPhase",
                                new TargetFuncAdditiveConeSecondPhase());

    TargetFunctions.try_emplace("ProbabilisticConeSecondPhase",
                                new TargetFuncProbabilisticConeSecondPhase());
}
//----------------------------------------------------------

MyGradModel::~MyGradModel()
{
    for (auto & item : TargetFunctions)
        delete item.second;
}
//----------------------------------------------------------

void MyGradModel::Set_nDraws(size_t _nDraws)
{
    nDraws = min(_nDraws, Configs.size());
    OnResize(Width, Height);
}
//----------------------------------------------------------

void MyGradModel::DrawMeIn3D()
{
    if (DrawOnlyOne)
    {
        DrawOneConfig(iCurConfig, true);
    }
    else
    {
        DrawSeveralConfigs();
    }
}
//----------------------------------------------------------

void MyGradModel::DrawOneConfig(size_t ind, bool OnlyOne)
{
    if (ind >= Configs.size() )
        return;

    if (OnlyOne)
    {
        if ( (iCurConfig < 0) || (iCurConfig > (int)ind) || (iCurConfig >= (int)Configs.size())  )
            return;
        //iCurConfig = ind;
        //DrawOnlyOne = true;

        int minSize = min(Width, Height);
        BigViewPort = {5, 5, minSize-5*2, minSize-5*2};

//        qDebug() << "BigViewPort.top() =" << BigViewPort.top();

        glViewport(BigViewPort.left(), BigViewPort.top(), BigViewPort.width(), BigViewPort.height());
//        glViewport(5, 5, minSize-5*2, minSize-5*2);
//        glViewport(50, 50, minSize-100, minSize-100);

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.04, 1.04, -1.04, 1.04, -30, 30);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glLineWidth(1.0f);
    glColor3f(0, 0, 0.7f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(-1.02, -1.02);
        glVertex2f(-1.02, 1.02);
        glVertex2f(1.02, 1.02);
        glVertex2f(1.02, -1.02);
        glVertex2f(-1.02, -1.02);
    glEnd();

    if ((int)ind == iCurConfig /*&& !DrawOnlyOne*/)
    {
        glLineWidth(3.0f);
        glColor3f(0.9f, 0.1f, 0.1f);
        glBegin(GL_LINE_STRIP);
            glVertex2f(-1.03, -1.03);
            glVertex2f(-1.03, 1.03);
            glVertex2f(1.03, 1.03);
            glVertex2f(1.03, -1.03);
            glVertex2f(-1.03, -1.03);
        glEnd();
    }

    if (Configs.at(ind).Settings3d.IsPerspective)
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

//        gluPerspective(45.0, Width/(double)Height, 0.0001, 30.0);
        gluPerspective(45.0, 1.0, 0.0001, 30.0);

//        glGetDoublev(GL_MODELVIEW_MATRIX, modl);
//        glGetDoublev(GL_PROJECTION_MATRIX, proj);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    Configs.at(ind).DrawIn3D(NodesType, IsDrawAbonents, AreaRandCoords, AreaGradDesc, WhatShow);
}
//----------------------------------------------------------

void MyGradModel::SwitchDrawOnlyOne()
{
    DrawOnlyOne = !DrawOnlyOne;
    DrawMeIn3D();
}
//----------------------------------------------------------

void MyGradModel::SwitchPerspective()
{
    if (iCurConfig < 0 || iCurConfig >= (int)Configs.size())
        return;

    Configs.at(iCurConfig).Settings3d.IsPerspective = !Configs.at(iCurConfig).Settings3d.IsPerspective;
    DrawMeIn3D();
}
//----------------------------------------------------------

void MyGradModel::SwitchPerspectiveForAll()
{
    IsPerspectiveForAll = !IsPerspectiveForAll;
    for (auto & c : Configs)
        c.Settings3d.IsPerspective = IsPerspectiveForAll;
    DrawMeIn3D();
}
//----------------------------------------------------------

void MyGradModel::ResetViewPoint()
{
    if (iCurConfig < 0 || iCurConfig >= (int)Configs.size())
        return;

    Configs.at(iCurConfig).Settings3d = Settings3dType();
    DrawMeIn3D();
}
//----------------------------------------------------------

void MyGradModel::ResetViewPointForAll()
{
    for (auto & c : Configs)
        c.Settings3d = Settings3dType();
    DrawMeIn3D();
}
//----------------------------------------------------------

void MyGradModel::OnResize(int _width, int _height)
{
    SetWidthAndHeight(_width, _height);

//    if (nDraws > Configs.size())
//    {
//        cout << "WARNING: n > Configs.size()" << endl;
//        nDraws = Configs.size();
//    }

    int offset = 8; int nCols; double w;

    if (Width >= Height)
    {
        int minSide = Height;
        int nRows = 1;
        do
        {
            if (minSide*(floor(nDraws/(double)nRows)) > Width)
            {
                nRows++;
                minSide = min(Width, Height/nRows);
            }
            else
                break;

        } while(1);

        w = minSide - 2*offset;
        nCols = floor(nDraws/(double)nRows);
    }
    else
    {
        int minSide = Width;
        int nRows = 1;
        do
        {
            if (minSide*(floor(nDraws/(double)nRows)) > Width)
            {
                nRows++;
                minSide = min(Width/nRows, Height);
            }
            else
                break;
        } while(1);

        w = minSide - 2*offset;
        nCols = floor(nDraws/(double)nRows);
    }

    ViewPorts.clear();
    ViewPorts.reserve(nDraws);
    for (size_t i = 0; i < nDraws; ++i)
    {
        size_t row = i / nCols;
        size_t col = i % nCols;

        QPoint p1, p2;
        p1.setX(offset + col*(w+2*offset));
        p2.setX(offset + col*(w+2*offset) + w);
//        p2.setY(Height - 1 -  (offset + row*(w+2*offset)) );
//        p1.setY(Height - 1 -  (offset + row*(w+2*offset) + w));

        p1.setY( (offset + row*(w+2*offset)) );
        p2.setY( (offset + row*(w+2*offset) + w));

        QRect rect(p1, p2);

        ViewPorts.emplace_back(rect);
    }
}
//----------------------------------------------------------

void MyGradModel::OnKeyPress(QKeyEvent *pe)
{
    switch (pe->key())
    {
    case Qt::Key_Up:

        break;
    case Qt::Key_Down:

        break;
    case Qt::Key_Left:

        break;
    case Qt::Key_Right:

        break;
    case Qt::Key_1:

        //SwitchDrawOnlyOne();
        break;

    case Qt::Key_3:

        //SwitchPerspective();
        break;
    }
}
//----------------------------------------------------------

int MyGradModel::CalcCurViewPortNumber(int ClientX, int ClientY) const
{
    //size_t i = 0;
    int iCurInd = -1;
    for (const auto & v : ViewPorts)
    {
        //qDebug() << v;

        if ( !v.isValid() )
            throw std::runtime_error("!v.isValid() ");

        ++iCurInd;
        if (iCurInd >= (int)nDraws)
            return -1;

        if ( v.contains(ClientX, ClientY) )
        {
//            iCurInd = i;
            return iCurInd;
        }

        //i++;
    }

//    return iCurInd;
    return -1;
}
//----------------------------------------------------------

bool MyGradModel::LoadReliefFromFile(const QString &_reliefFileName)
{
    return Relief.LoadFromFile(_reliefFileName);
}
//----------------------------------------------------------

bool MyGradModel::AddNewRoute()
{
    if (iCurConfig < 0 || iCurConfig >= (int)Configs.size())
    {
        QMessageBox::warning(nullptr, "Warning", "Yoy have to choose config");
        return false;
    }

    Routes.push_back({});
    Configs.at(iCurConfig).RoutesAccess().push_back({});

    return true;
}
//----------------------------------------------------------

void MyGradModel::AddNewPointToLastRoute(double wx, double wy)
{
//    if (wx < Relief.GetArea().left()   || wx > Relief.GetArea().right() ||
//        wy < Relief.GetArea().top() || wy > Relief.GetArea().bottom() )
    if (wx < -1 || wx > 1 ||
        wy < -1 || wy > 1 )
    {
//        qDebug() << "Error: wx < Relief.GetArea().left()   || wx > Relief.GetArea().right() ||"
//                    "wy < Relief.GetArea().bottom() || wy > Relief.GetArea().top()";
        qDebug() << "wx < -1 || wx > 1 ||"
                    "wy < -1 || wy > 1";
        return;
    }

    bool isOk = false;
    double weight = QInputDialog::getDouble(nullptr, "Input value", "Input weight of point ( (0, 1] or -1)", 0.1, -1, 1, 5, &isOk);
    if (!isOk)
    {
        QMessageBox::warning(nullptr, "Warning", "The value has not been actepted");
        return;
    }

    double realX, realY, realZ;
    realZ = Relief.CalcRealXYZbyNormXY(wx, wy, realX, realY);

    Routes.back().Points.push_back({});
//    Routes.back().Points.back().Pos = {float(wx), float(wy), (float)Relief.CalcRealZbyRealXY(wx, wy)};
    Routes.back().Points.back().Pos = {float(realX), float(realY), float(realZ)};
    Routes.back().Points.back().Weight = weight;

    Configs.at(iCurConfig).RoutesAccess().back().Points.push_back(Routes.back().Points.back());

}
//----------------------------------------------------------

void MyGradModel::FinishRoute()
{
    if (Routes.back().Points.empty())
    {
        qDebug() << "Routes.back().Points.empty() --- there is no new route? -- back route will be deleted now";
        Routes.pop_back();
        return;
    }

    Routes.back().CalcOtherWeights();
    Routes.back().CalcRouteLength();
}
//----------------------------------------------------------

bool MyGradModel::DeleteRoute(double wx, double wy)
{
    if (wx < -1 || wx > 1 ||
        wy < -1 || wy > 1 )
    {
        qDebug() << "wx < -1 || wx > 1 ||"
                    "wy < -1 || wy > 1";
        return false;
    }

    double realX, realY, realZ;
    realZ = Relief.CalcRealXYZbyNormXY(wx, wy, realX, realY);

    MyVector3D<> PosToDel = {realX, realY, realZ};
    float MinDist = std::numeric_limits<float>::max();
    auto itToDel = Routes.end();
    for ( auto it = Routes.begin(); it != Routes.end(); ++it)
    {
        for (const auto & p : it->Points)
        {
            float dist = (PosToDel - p.Pos).length();  // 3d vs. 2d ?
            if (dist < MinDist)
            {
                MinDist = dist;
                itToDel = it;
            }
        }
    }

    if (itToDel == Routes.end())
    {
        qDebug() << "itToDel == Routes.end()";
        return false;
    }
    else
    {
        Routes.erase(itToDel);
    }

    return true;
}
//----------------------------------------------------------

void MyGradModel::CalcAbonentsPos(int t)
{
    for (auto & r : Routes)
    {
        r.CalcAbonentPos(t);
    }
}
//----------------------------------------------------------

void MyGradModel::ChangeFileName(const QString &_newFileName)
{
    FileName = _newFileName;
}
//----------------------------------------------------------

void MyGradModel::NewGradModelBulk()
{
    OldX = 0; CurrentX = 0;
    OldY = 0; CurrentY = 0;

    NodesType = SignalNodeType::Sphere;

    IsRandomNodeCoords = true;
    Nodes.clear();
    Routes.clear();
    IsRandomRoutes = false;

//    ReliefMatInfo = ReliefMatInfoStruct();
    Relief.Clear();

    IsRandomRelief = false;

    Configs.clear();

    Width = 200; Height = 200; // ???
    iCurConfig = -1;
    DrawOnlyOne = false;
    ViewPorts.clear();
    IsPerspectiveForAll = false;

    Name = "Unknown";
    FileName = "";
    //IsLoaded = false;

    nDraws = 1;
    IsGradCalculating = false;

//    TargetFuncSettingsGlobal.TargetFuncType = TargetFuncEnum::Additive;

    //IsSaved = false;

    ProtoGradDesc.SetIsUseUserTargetFunction(true);
    ProtoGradDesc.SetAlpha(0.35);
    ProtoGradDesc.SetEps(0.01);
    ProtoGradDesc.SetEta_FirstJump(10);
    ProtoGradDesc.SetEta_k_inc(1.08);
    ProtoGradDesc.SetEta_k_dec(2.01);
    ProtoGradDesc.SetMin_Eta(1e-7);
    ProtoGradDesc.SetFinDifMethod(false);
    ProtoGradDesc.SetMaxIters(500);
    ProtoGradDesc.SetMaxTime(5);
    ProtoGradDesc.SetCallBackFreq(20);
}
//----------------------------------------------------------

void MyGradModel::ReCreateReliefListsAndBuildRelief()
{
    Relief.ReCreateReliefListsGL();
    Relief.BuildReliefToGL(false);
    Relief.BuildReliefToGL(true);
}
//----------------------------------------------------------

void MyGradModel::ReCreateSignalNodes(size_t _count, double _R, double _betha)
{
    Nodes.clear();
    Nodes.reserve(_count);

    for (size_t i = 0; i < _count; ++i)
    {
        Nodes.emplace_back(MyVector3D<>(), _R, 0, _betha);
    }
}
//----------------------------------------------------------

void MyGradModel::ApplySignalNodesToAllConfigs()
{
    for (auto & cnf : Configs)
    {
        cnf.SetNodes(Nodes);
        if (IsRandomNodeCoords)
        {
//            cnf.SetRandomNodeCoords(CalcAreaByBoundsRandCoords());
            cnf.SetRandomNodeCoords(AreaRandCoords);
        }
    }
    IsSaved = false;
}
//----------------------------------------------------------

void MyGradModel::ApplyAbonentsPosInRoutesToAllConfigs()
{
    for (auto & cnf : Configs)
    {
        cnf.SetAbonentsPosForRoutes(Routes);
    }
}
//----------------------------------------------------------

void MyGradModel::ApplyRoutesToAllConfigs(NeedToSave _NeedToSave)
{
    for (auto & cnf : Configs)
    {
        cnf.SetRoutes(Routes);
    }
    if (_NeedToSave == NeedToSave::Need)
        IsSaved = false;
}
//----------------------------------------------------------

void MyGradModel::ApplyCurNodeFromCurConfigToAllConfigs()
{
    if (iCurConfig >= 0 && Configs.at(iCurConfig).Get_iCurNode() >= 0)
    {
        const auto & curConfig = GetCurrentConfig();
        int ind = curConfig.Get_iCurNode();
        const auto & curNode = curConfig.GetNodes().at(ind);

        for (auto & cnf : Configs)
        {
            cnf.SetNode(ind, curNode);
        }

        Nodes.at(ind) = curNode;

    }
    else
        qDebug() << "CurNode is not selected in MyGradModel::ApplyCurNodeFromCurConfigToAllConfigs. Abort.";
}
//----------------------------------------------------------

const MyConfig & MyGradModel::GetCurrentConfig() const
{
    if (iCurConfig < 0 || iCurConfig >= (int)Configs.size())
        throw std::runtime_error("iCurConfig < 0 || iCurConfig >= (int)Configs.size() in MyGradModel::GetCurrentConfig()");

    return Configs.at(iCurConfig);
}
//----------------------------------------------------------

MyConfig & MyGradModel::CurrentConfigAccess()
{
    if (iCurConfig < 0 || iCurConfig >= (int)Configs.size())
        throw std::runtime_error("iCurConfig < 0 || iCurConfig >= (int)Configs.size() in MyGradModel::CurrentConfigAccess()");

    return Configs.at(iCurConfig);
}
//----------------------------------------------------------

void MyGradModel::OnMousePress(QMouseEvent *pe)
{
    OldX = pe->pos().x();
    OldY = pe->pos().y();

    if (!DrawOnlyOne)
    {
        iCurConfig = CalcCurViewPortNumber(pe->pos().x(), pe->pos().y());
    }
}
//----------------------------------------------------------

void MyGradModel::OnMouseMove(QMouseEvent *pe)
{
    if (iCurConfig < 0 || iCurConfig >= (int)Configs.size())
        return;

    CurrentX = pe->pos().x();
    CurrentY = pe->pos().y();

    int dx = CurrentX - OldX;
    int dy = CurrentY - OldY;


    if (QApplication::keyboardModifiers() == Qt::ControlModifier) // Отследить состояние клавиши Ctrl
    {
        if (Configs.at(iCurConfig).Settings3d.IsPerspective)
            Configs.at(iCurConfig).Settings3d.RotZ += RotSpeed*dx;
    }
    else if (QApplication::keyboardModifiers() == Qt::AltModifier)
    {
        double k1 = Configs.at(iCurConfig).Settings3d.IsPerspective ? -Configs.at(iCurConfig).Settings3d.eyeZ / 2.5  : 1;

        const auto & vPort = DrawOnlyOne ? BigViewPort : ViewPorts.at(iCurConfig);

//        double k2 = Configs.at(iCurConfig).Settings3d.IsPerspective ?
//                    (vPort.width() + vPort.height()) / 2.0 / 300.0 :
//                    550 / ((vPort.width() + vPort.height()) / 2.0);


        double k2 = Configs.at(iCurConfig).Settings3d.IsPerspective ?
                    550.0 / ((vPort.width() + vPort.height()) / 2.0) :
                    550.0 / ((vPort.width() + vPort.height()) / 2.0);


//        qDebug() << "k1 =" << k1;
//        qDebug() << "k2 =" << k2;
//        qDebug() << "Height =" << Height;

//        Configs.at(iCurConfig).Settings3d.TrX += k1*k2*TransSpeed*dx;
//        Configs.at(iCurConfig).Settings3d.TrY -= k1*k2*TransSpeed*dy;

        Configs.at(iCurConfig).Settings3d.centerX += k1*k2*TransSpeed*dx;
        Configs.at(iCurConfig).Settings3d.centerY -= k1*k2*TransSpeed*dy;

        Configs.at(iCurConfig).Settings3d.eyeX += k1*k2*TransSpeed*dx;
        Configs.at(iCurConfig).Settings3d.eyeY -= k1*k2*TransSpeed*dy;
    }
    else
    {
        if (Configs.at(iCurConfig).Settings3d.IsPerspective)
        {
            Configs.at(iCurConfig).Settings3d.RotX += RotSpeed*dy;
            Configs.at(iCurConfig).Settings3d.RotY += RotSpeed*dx;
        }
    }


    OldX = CurrentX;
    OldY = CurrentY;
}
//----------------------------------------------------------

void MyGradModel::OnMouseWheel(QWheelEvent *pe, bool wExists,
                               double wx, double wy, double wz)
{
    if (iCurConfig < 0 || iCurConfig >= (int)Configs.size())
        return;

    //if (Configs.at(iCurConfig).Settings3d.IsPerpective)

    const auto & vPort = DrawOnlyOne ? BigViewPort : ViewPorts.at(iCurConfig);

    int x = pe->position().x();
    int y = pe->position().y();

    if (x < vPort.left() || x > vPort.right())
    {
        qDebug() << "x < vPort.left() || x > vPort.right()";
        return;
    }
    if (y < vPort.top() || y > vPort.bottom())
    {
        qDebug() << "y < vPort.top() || y > vPort.bottom()";
        return;
    }

    double dzWheel = pe->angleDelta().y() / 600.0f / 2.0f;


    qDebug() << "Center:" << Configs.at(iCurConfig).Settings3d.centerX << " - "
                          << Configs.at(iCurConfig).Settings3d.centerY << " - "
                          << Configs.at(iCurConfig).Settings3d.centerZ;

    qDebug() << "Eye:"    << Configs.at(iCurConfig).Settings3d.eyeX << " - "
                          << Configs.at(iCurConfig).Settings3d.eyeY << " - "
                          << Configs.at(iCurConfig).Settings3d.eyeZ;

    if (wExists)
    {
//        wz = 0;


        double dx = wx - Configs.at(iCurConfig).Settings3d.eyeX;
        double dy = wy - Configs.at(iCurConfig).Settings3d.eyeY;
        double dz = wz - Configs.at(iCurConfig).Settings3d.eyeZ;

        Configs.at(iCurConfig).Settings3d.eyeX += dx*dzWheel;
        Configs.at(iCurConfig).Settings3d.eyeY += dy*dzWheel;
        Configs.at(iCurConfig).Settings3d.eyeZ += dz*dzWheel;

        dx = wx - Configs.at(iCurConfig).Settings3d.centerX;
        dy = wy - Configs.at(iCurConfig).Settings3d.centerY;
        dz = wz - Configs.at(iCurConfig).Settings3d.centerZ;

        Configs.at(iCurConfig).Settings3d.centerX += dx*dzWheel;
        Configs.at(iCurConfig).Settings3d.centerY += dy*dzWheel;
        Configs.at(iCurConfig).Settings3d.centerZ += dz*dzWheel;


//        Configs.at(iCurConfig).Settings3d.centerX = wx;
//        Configs.at(iCurConfig).Settings3d.centerY = wy;
//        Configs.at(iCurConfig).Settings3d.centerZ = wz;
    }
    else
    {
        qDebug() << "NOT wExists";
        Configs.at(iCurConfig).Settings3d.eyeZ += dzWheel;
    }

//    Configs.at(iCurConfig).Settings3d.eyeZ -= dzWheel;

//    int xp = 2*x/vPort.width();
//    int yp = 2*y/vPort.height();

//    double dz = pe->angleDelta().y() / 600.0f / 2.0f;

//    double tgx = (pe->position().x() - vPort.center().x()) / Configs.at(iCurConfig).Settings3d.TrZ;
//    double a = dz*tgx;

//    double tgy = (pe->position().y() - vPort.center().y()) / Configs.at(iCurConfig).Settings3d.TrZ;
//    double b = dz*tgy;

//    double k = 0.08;

//    Configs.at(iCurConfig).Settings3d.TrZ += dz;

//    Configs.at(iCurConfig).Settings3d.TrX -= a*k*log(std::abs(Configs.at(iCurConfig).Settings3d.TrZ));
//    Configs.at(iCurConfig).Settings3d.TrY -= b*k*log(std::abs(Configs.at(iCurConfig).Settings3d.TrZ));


//    qDebug() << "TrZ = " << Configs.at(iCurConfig).Settings3d.TrZ;
//    qDebug() << "TrX = " << Configs.at(iCurConfig).Settings3d.TrX;




}
//----------------------------------------------------------

void MyGradModel::DrawSeveralConfigs()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (ViewPorts.size() > Configs.size())
    {
        cout << "WARNING: ViewPorts.size() > Configs.size()" << endl;
        nDraws = Configs.size(); // ?
    }

    //DrawOnlyOne = false;

    for (size_t i = 0; i < min(ViewPorts.size(), Configs.size()); ++i)
    {
//        qDebug() << "ViewPorts.size() = " << ViewPorts.size();
//        qDebug() << "Configs.size() = " << Configs.size();
//        qDebug() << "i = " << i;
//        if (i >= ViewPorts.size())
//            qDebug() << "!!!!!!!!!!!!";
        const QRect & rect = ViewPorts.at(i);
        int x = rect.left();
        int y = rect.top();

        glViewport(x, Height - 1 - y - rect.height(), rect.width(), rect.height());

//        GLint vport[4];
//        glGetIntegerv(GL_VIEWPORT, vport);

        DrawOneConfig(i, false);
    }
}
//----------------------------------------------------------

size_t MyGradModel::ParseJson(const QJsonObject &_jsonObject, const QJsonParseError &_parseError)
{
    (void)_parseError; // Возможно, использовать в дальнейшем

    qDebug() << "MyGradModel:";
    qDebug() << "Name = " << Name;

    const QJsonObject &configObject = _jsonObject["Configuration"].toObject();

    size_t ConfigCount = configObject["Count"].toInt(0);
//    Configs.reserve(ConfigCount);
    qDebug() << "ConfigCount = " << ConfigCount;

    IsRandomRelief = configObject["IsRandomRelief"].toBool(true);
    if (!IsRandomRelief)
    {
        QString ReliefFileName = configObject["ReliefFileName"].toString();

        bool isCorrected = CorrectFileNameIfDoesntExist(ReliefFileName, ReliefsDefaultDir, "Relief", ReliefsExtension);
        if (isCorrected)
            qDebug() << "ReliefFileName has been corrected";

        if (!Relief.LoadFromFile(ReliefFileName))
            throw std::runtime_error("Relief File Not Found or Couldn't be read");
    }
    else
    {
        const QJsonObject &areaObject = configObject["Area"].toObject();

        double l = areaObject["left"].toDouble(-1);
        double r = areaObject["right"].toDouble(-1);
        double t = areaObject["top"].toDouble(-1);
        double b = areaObject["bottom"].toDouble(-1);

        Relief.SetArea(l, b, r, t);
    }

    qDebug() << "Area = " << Relief.GetArea();

    size_t RouteCount = configObject["RouteCount"].toInt(0);
    Routes.reserve(RouteCount);
    IsRandomRoutes = configObject["IsRandomRoutes"].toBool(true);

    if (!IsRandomRoutes)
    {
        const QJsonArray &routesArray = configObject["Routes"].toArray();
        for (auto it = routesArray.begin(); it != routesArray.end();  ++it)
        {
            const QJsonObject &routeObject = it->toObject();
            //Route tempRoute;

            QString Name = routeObject["Name"].toString("No name");

            double w = routeObject["Weight"].toDouble(1);

            size_t pointCount = routeObject["PointCount"].toDouble(-1);
//            Routes.emplace_back(Route());
            Routes.emplace_back();
            Routes.back().Points.reserve(pointCount);
            Routes.back().SetName(Name);
            Routes.back().Set_w(w);
            Routes.back().AbonentDirectAccess().
                    LoadFromJsonObject(routeObject["Abonent"].toObject());

            const QJsonArray &pointsArray = routeObject["Points"].toArray();
            for (auto itP = pointsArray.begin(); itP != pointsArray.end();  ++itP)
            {
                const QJsonObject &pointObject = itP->toObject();

                Routes.back().Points.emplace_back(RoutePoint());
                Routes.back().Points.back().Pos.setX(pointObject["x"].toDouble(-1));
                Routes.back().Points.back().Pos.setY(pointObject["y"].toDouble(-1));

                Relief.CalcAndReWriteRealZforPos3d(Routes.back().Points.back().Pos);

                Routes.back().Points.back().Weight = pointObject["w"].toDouble(-1);
            }

            Routes.back().CalcOtherWeights();
            Routes.back().CalcRouteLength();

            if (pointCount != Routes.back().Points.size())
                throw std::runtime_error("pointCount != Routes.back().Points.size()");
        }

        if (RouteCount != Routes.size())
            throw std::runtime_error("RouteCount != Routes.size()");
    }
    else
    {
        // Сгенерировать маршруты программно случайным образом
    }

    size_t iRoute = 0;
    for (const auto & route : Routes)
    {
        qDebug() << "iRoute = " << iRoute;
        for (const auto & p : route.Points)
        {
            qDebug() << "\t" << (double)p.Pos.x() << "\t" <<
                        (double)p.Pos.y() << "\t" << (double)p.Pos.z() << "\tw = " << p.Weight;
        }
        iRoute++;
    }

    size_t SignalNodeCount = configObject["SignalNodeCount"].toInt(0);
    Nodes.reserve(SignalNodeCount);
    IsRandomNodeCoords = configObject["IsRandomNodeCoords"].toBool(true);

    QString NodesTypeStr = configObject["SignalNodeType"].toString();
    NodesType = SignalNode::ConvertStringToSignalNodeType(NodesTypeStr);
    if (NodesType == SignalNodeType::Unknown)
    {
        QMessageBox::warning(nullptr, "Warning", "SignalNodeType is unknown and will be set as Sphere");
        NodesType = SignalNodeType::Sphere;
//        throw std::runtime_error("SignalNodeType::Unknown");
    }

    const QJsonArray &nodesArray = configObject["Nodes"].toArray();
    for (auto it = nodesArray.begin(); it != nodesArray.end();  ++it)
    {
        Nodes.emplace_back(SignalNode());
        Nodes.back().LoadFromJsonObject(it->toObject());

    }
    if (SignalNodeCount != Nodes.size())
        throw std::runtime_error("SignalNodeCount != Nodes.size()");

    if (!IsRandomNodeCoords)
    {
        // Прочитать координаты из файла, иначе остаются нулями по умолчанию
    }

    for (const auto & node : Nodes)
    {
        cout << "node.R = " << node << endl;
    }


    GridSettings.LoadFromJsonObject(configObject["GridSettings"].toObject());

    BoundsRandCoords.LoadFromJsonObject(configObject["BoundsRandCoords"].toObject());
    BoundsGradDesc.LoadFromJsonObject(configObject["BoundsGradDesc"].toObject());


    const QJsonObject &gradDescObject = _jsonObject["GradDesc"].toObject();

    GradDescFileName = gradDescObject["GradDescFileName"].toString();

    bool isGradDescFileNameCorrected = CorrectFileNameIfDoesntExist(GradDescFileName,
                                                                    SettingsDefaultDir,
                                                                    "GradDesc",
                                                                    SettingsGDExtension);
    if (isGradDescFileNameCorrected)
        qDebug() << "GradDescFileName has been corrected";

    if (!GradDescLoadFromFile(ProtoGradDesc, GradDescFileName))
    {
        qDebug() << "GradDesc file not open or currupted!";
        QMessageBox::warning(nullptr, "Warning", "GradDesc file not found or currupted!");
    }


    const QJsonObject &targetFuncObject = _jsonObject["TargetFunctionSettings"].toObject();

    ActiveTargetFuncFirstPhase  = targetFuncObject["TargetFuncFirstPhase"].toString().toStdString();
    if (ActiveTargetFuncFirstPhase.empty())
    {
        qDebug() << "ActiveTargetFuncFirstPhase is not specified!";
        QMessageBox::warning(nullptr, "Warning", "ActiveTargetFuncFirstPhase is not specified!");
    }
    ActiveTargetFuncSecondPhase = targetFuncObject["TargetFuncSecondPhase"].toString().toStdString();
    if (ActiveTargetFuncSecondPhase.empty())
    {
        qDebug() << "ActiveTargetFuncSecondPhase is not specified!";
        QMessageBox::warning(nullptr, "Warning", "ActiveTargetFuncSecondPhase is not specified!");
    }

    QString TargetFuncFileName = targetFuncObject["TargetFuncFileName"].toString();

    bool isTargetFuncFileNameCorrected = CorrectFileNameIfDoesntExist(TargetFuncFileName, SettingsDefaultDir, "TargetFunc", SettingsTFExtension);
    if (isTargetFuncFileNameCorrected)
        qDebug() << "TargetFuncFileName has been corrected";

    if (!TargetFuncSettingsGlobal.LoadFromFile(TargetFuncFileName))
    {
        qDebug() << "TargetFunc file not open or currupted!";
        QMessageBox::warning(nullptr, "Warning", "TargetFunc file not found or currupted!");
    }

    return ConfigCount;
}
//----------------------------------------------------------

QJsonArray MyGradModel::RepresentRoutesAsJsonArray() const
{
    QJsonArray routesArray;

    for (const auto &r : Routes)
    {
        QJsonObject routeObject;

        routeObject.insert("Name", r.GetName());
        routeObject.insert("Weight", r.Get_w());

        routeObject.insert("Abonent", r.GetAbonent().RepresentAsJsonObject());

        routeObject.insert("PointCount", (int)r.Points.size());

        QJsonArray pointsArray;

        for (const auto &p : r.Points)
        {
            QJsonObject pointObject;
            pointObject.insert("x", (double)p.Pos.x());
            pointObject.insert("y", (double)p.Pos.y());
            pointObject.insert("z", (double)p.Pos.z());

            pointObject.insert("w", p.Weight);

            pointsArray.append(pointObject);
        }

        routeObject.insert("Points", pointsArray);
        routesArray.append(routeObject);
    }

    return routesArray;
}
//----------------------------------------------------------

QJsonArray MyGradModel::RepresentNodesAsJsonArray() const
{
    QJsonArray nodesArray;

     for (const auto &node : Nodes)
     {
//         QJsonObject nodeObject;
//         nodeObject.insert("R", (int)node.R);
//         nodeObject.insert("Beta", qRadiansToDegrees(node.Beta));

         // Сохранять другие характеристики? Если да, то из какой конфигурации?

//         nodesArray.append(nodeObject);

        nodesArray.append(node.RepresentAsJsonObject());
     }

     return nodesArray;
}
//----------------------------------------------------------

QRectF MyGradModel::CalcSomeAreaByBounds(const BoundsStruct &_bs)
{
    switch (_bs.BoundsType)
    {
    case BoundsTypeEnum::AllArea:
        return Relief.GetArea();
    break;
    case BoundsTypeEnum::ByRoutes:
    {
        double min_x = Relief.GetArea().right();
        double max_x = Relief.GetArea().left();
        double min_y = Relief.GetArea().bottom();
        double max_y = Relief.GetArea().top();

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

        return QRectF(min_x, min_y, max_x-min_x, max_y-min_y);
        break;
    }
    break;
    case BoundsTypeEnum::Selected:

        return QRectF(_bs.SelXstart, _bs.SelYstart,
                      _bs.SelW, _bs.SelH);
    break;
    default:
        throw std::runtime_error("Unknown BoundsTypeEnum in MyGradModel::CalcAreaByBounds");
    }
}
//----------------------------------------------------------

void MyGradModel::CorrectAreaBoundsIfItIsOutOfRelief(BoundsStruct &_bs)
{
    if (_bs.SelXstart < Relief.GetArea().left())
        _bs.SelXstart = Relief.GetArea().left();
    if (_bs.SelXstart >= Relief.GetArea().right())
        _bs.SelXstart = Relief.GetArea().right()-1;
    if (_bs.SelXstart + _bs.SelW > Relief.GetArea().right())
        _bs.SelW -= (_bs.SelXstart + _bs.SelW - Relief.GetArea().right());

    if (_bs.SelYstart < Relief.GetArea().top())
        _bs.SelYstart = Relief.GetArea().top();
    if (_bs.SelYstart >= Relief.GetArea().bottom())
        _bs.SelYstart = Relief.GetArea().bottom()-1;
    if (_bs.SelYstart + _bs.SelH > Relief.GetArea().bottom())
        _bs.SelH -= (_bs.SelYstart + _bs.SelH - Relief.GetArea().bottom());
}
//----------------------------------------------------------

void MyGradModel::ReCalcAreasByBounds()
{
     AreaRandCoords = CalcSomeAreaByBounds(BoundsRandCoords);
     AreaGradDesc = CalcSomeAreaByBounds(BoundsGradDesc);
}
//----------------------------------------------------------

/*
QRectF MyGradModel::CalcAreaByBoundsRandCoords()
{
    switch (BoundsRandCoords.BoundsType)
    {
    case BoundsTypeEnum::AllArea:
        AreaRandCoords =  Relief.GetArea();
    break;
    case BoundsTypeEnum::ByRoutes:
    {
        double min_x = Relief.GetArea().left(); // для жесткий ограничений без поиска
        double max_x = Relief.GetArea().right();
        double min_y = Relief.GetArea().top();
        double max_y = Relief.GetArea().bottom();

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

        AreaRandCoords = QRectF(min_x, min_y, max_x-min_x, max_y-min_y);
        break;
    }
    break;
    case BoundsTypeEnum::Selected:

        AreaRandCoords = QRectF(BoundsRandCoords.SelXstart, BoundsRandCoords.SelYstart,
                      BoundsRandCoords.SelW, BoundsRandCoords.SelH);
    break;
    default:
        throw std::runtime_error("Unknown BoundsTypeEnum in MyGradModel::CalcAreaByBoundsRandCoords");
    }

    return AreaRandCoords;
}*/
//----------------------------------------------------------

//QJsonObject MyGradModel::RepresentReliefInfoAsJsonObject() const
//{
//    QJsonObject ReliefInfoObject;

//    ReliefInfoObject.insert("IsUseReliefRandomSeed", ReliefMatInfo.IsUseReliefRandomSeed);
//    ReliefInfoObject.insert("ReliefRandomSeed", (int64_t)ReliefMatInfo.ReliefRandomSeed); // int64_t ?

//    QJsonObject ReliefCoeffsObject;
//    ReliefCoeffsObject.insert("A_r1", ReliefMatInfo.A_r1);
//    ReliefCoeffsObject.insert("A_r2", ReliefMatInfo.A_r2);

//    ReliefInfoObject.insert("ReliefCoeffs", ReliefCoeffsObject);

//    return ReliefInfoObject;
//}
//----------------------------------------------------------

const MyConfig & MyGradModel::GetActiveConfig() const
{
    if (iCurConfig < 0 || iCurConfig >= (int)Configs.size())
    {
        throw std::out_of_range("iCurConfig < 0 || iCurConfig >= (int)Configs.size()");
    }
    else
    {
        return Configs[iCurConfig];
    }

//    return Configs.at(iCurConfig);
}
//----------------------------------------------------------

bool MyGradModel::LoadFromFile(const QString &_fileName)
{
    Routes.clear();
    Nodes.clear();
    Configs.clear();

    size_t configCount = 0;

    QFile json(_fileName);
    if (json.open(QIODevice::ReadOnly))
    {
        FileName = _fileName;
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(json.readAll(), &parseError);
        if (parseError.error == QJsonParseError::NoError)
        {
            if (jsonDoc.isObject())
            {
                Name = _fileName.mid(0, _fileName.lastIndexOf('.'));
                configCount = ParseJson(jsonDoc.object(), parseError);
            }
        }
        else
        {
            qDebug() << parseError.errorString();
            return false;
        }

    }
    else
    {
        qDebug() << "json file not open";
        return false;
    }

//    ReCalcAreasByBounds();

    CreatePopulation(configCount);

    Relief.ReCreateReliefListsGL();
    Relief.BuildReliefToGL(false);
    Relief.BuildReliefToGL(true);


    //IsLoaded = true;
    IsSaved = true;
    return true;
}
//----------------------------------------------------------

void MyGradModel::CreatePopulation(size_t _count)
{
    ReCalcAreasByBounds();

    Configs.clear();

    MyConfig protoConfig;

//    protoConfig.SetArea(Relief.GetArea());

    protoConfig.SetRelief(&Relief);
    protoConfig.SetNodes(Nodes);
    protoConfig.SetRoutes(Routes);

    if (IsRandomNodeCoords)
    {
//        protoConfig.SetRandomNodeCoords(Relief.GetArea());
//        protoConfig.SetRandomNodeCoords(CalcAreaByBoundsRandCoords());
         protoConfig.SetRandomNodeCoords(AreaRandCoords);
    }


    Configs.resize(_count, protoConfig);

    nDraws = min(nDraws, Configs.size());

    for (auto & cnf : Configs)
    {
//        cnf.SetArea(Area);
//        cnf.SetNodes(Nodes);
//        cnf.;

        if (IsRandomNodeCoords)
        {
//            cnf.SetRandomNodeCoords(Relief.GetArea());
//            cnf.SetRandomNodeCoords(CalcAreaByBoundsRandCoords());
            cnf.SetRandomNodeCoords(AreaRandCoords);
        }
    }
}
//----------------------------------------------------------

void MyGradModel::CancelGradDescent()
{
    IsGradCalculating = false;
    for (auto & c : Configs)
    {
        c.CancelGradDescent();
    }
}
//----------------------------------------------------------

bool MyGradModel::StartGradDescent_Phase_1(IGradDrawable *pGLWidget)
{
    auto it_TargetFunc = TargetFunctions.find(ActiveTargetFuncFirstPhase);
    if (it_TargetFunc == TargetFunctions.end())
        throw std::logic_error("TargetFunctionFirstPhase not found in MyGradModel::StartGradDescent_Phase_1");

    int iDraw = nDraws;
    QApplication::setOverrideCursor(Qt::WaitCursor);

//    cout << "Costs Before GradDesc:" << endl;
//    for (auto & c : Configs)
//        cout << c.GradDesc.GetLastCost() << endl;

    auto & tf = *it_TargetFunc->second;
    tf.InitMembersByGlobalSettings(TargetFuncSettingsGlobal);

    IsGradCalculating = true;
    for (auto & c : Configs)
    {
        if (!IsGradCalculating)
            break;

//        c.StartGradDescent(iDraw, ProtoGradDesc, TargetFuncSettingsGlobal, *it_TargetFunc->second, NodesType, pGLWidget);
        c.StartGradDescent(iDraw, ProtoGradDesc, tf, NodesType, AreaGradDesc, pGLWidget);

        --iDraw;
    }

    cout << "Costs Before Sort:" << endl;
    for (auto & c : Configs)
        cout << c.GradDesc.GetLastCost() << endl;

    sort(Configs.begin(), Configs.end(), [](const MyConfig &a, const MyConfig &b)
        {
            return a.GradDesc.GetLastCost() < b.GradDesc.GetLastCost();
        } );

    cout << "Costs After Sort:" << endl;
    for (auto & c : Configs)
        cout << c.GradDesc.GetLastCost() << endl;

//    pGLWidget->repaint();
    pGLWidget->Repaint();
    QApplication::restoreOverrideCursor();
    //----------------------------

    return true;
}
//----------------------------------------------------------

bool MyGradModel::StartGradDescent_Phase_1_for_Current(IGradDrawable *pGLWidget)
{
    if (iCurConfig < 0 || iCurConfig >= (int)Configs.size())
        return false;

    auto it_TargetFunc = TargetFunctions.find(ActiveTargetFuncFirstPhase);
    if (it_TargetFunc == TargetFunctions.end())
        throw std::logic_error("TargetFunctionFirstPhase not found in MyGradModel::StartGradDescent_Phase_1_for_Current");

    QApplication::setOverrideCursor(Qt::WaitCursor);

    cout << "Cost Before Grad:" << endl;
    cout << Configs.at(iCurConfig).GradDesc.GetLastCost() << endl;

    auto & tf = *it_TargetFunc->second;
    tf.InitMembersByGlobalSettings(TargetFuncSettingsGlobal);

    IsGradCalculating = true;

//    Configs.at(iCurConfig).StartGradDescent(1, ProtoGradDesc, TargetFuncSettingsGlobal,
//                                            *it_TargetFunc->second, NodesType, pGLWidget);

    Configs.at(iCurConfig).StartGradDescent(1, ProtoGradDesc,
                                            tf, NodesType, AreaGradDesc, pGLWidget);

    cout << "Cost After Grad:" << endl;
    cout << Configs.at(iCurConfig).GradDesc.GetLastCost() << endl;


    pGLWidget->Repaint();
    QApplication::restoreOverrideCursor();

    return true;
}
//----------------------------------------------------------

bool MyGradModel::RemoveUncovered(IGradDrawable *pGLWidget)
{
    DeleteConfigsWithUncoveredPoints();

    for (const auto & c: Configs)
    {
        c.PrintBondsInfo();
    }

    pGLWidget->Repaint();

    return true;
}
//----------------------------------------------------------

bool MyGradModel::StartGradDescent_Phase_2(IGradDrawable *pGLWidget)
{
    auto it_TargetFunc = TargetFunctions.find(ActiveTargetFuncSecondPhase);
    if (it_TargetFunc == TargetFunctions.end())
        throw std::logic_error("TargetFunctionSecondPhase not found in MyGradModel::StartGradDescent_Phase_2");

    int iDraw = nDraws;
    QApplication::setOverrideCursor(Qt::WaitCursor);

//    cout << "Costs Before GradDesc:" << endl;
//    for (auto & c : Configs)
//        cout << c.GradDesc.GetLastCost() << endl;

    auto & tf = *it_TargetFunc->second;
    tf.InitMembersByGlobalSettings(TargetFuncSettingsGlobal);

    IsGradCalculating = true;
    for (auto & c : Configs)
    {
        if (!IsGradCalculating)
            break;
//        c.StartFinalGradDescent(iDraw, ProtoGradDesc, TargetFuncSettingsGlobal,
//                                NodesType, pGLWidget);
        c.StartFinalGradDescent(iDraw, ProtoGradDesc,
                                tf, NodesType, AreaGradDesc, pGLWidget);
        --iDraw;
    }

    cout << "Costs Before Sort (StartFinalGradDescent):" << endl;
    for (auto & c : Configs)
        cout << c.GradDesc.GetLastCost() << endl;

    sort(Configs.begin(), Configs.end(), [](const MyConfig &a, const MyConfig &b)
        {
            return a.GradDesc.GetLastCost() < b.GradDesc.GetLastCost();
        } );

    cout << "Costs After Sort (StartFinalGradDescent):" << endl;
    for (auto & c : Configs)
        cout << c.GradDesc.GetLastCost() << endl;

//    pGLWidget->repaint();
    pGLWidget->Repaint();
    QApplication::restoreOverrideCursor();

    return true;
}
//----------------------------------------------------------

bool MyGradModel::StartGradDescent_Phase_2_for_Current(IGradDrawable *pGLWidget)
{
    if (iCurConfig < 0 || iCurConfig >= (int)Configs.size())
        return false;

    auto it_TargetFunc = TargetFunctions.find(ActiveTargetFuncSecondPhase);
    if (it_TargetFunc == TargetFunctions.end())
        throw std::logic_error("TargetFunctionSecondPhase not found in MyGradModel::StartGradDescent_Phase_2_for_Current");

    QApplication::setOverrideCursor(Qt::WaitCursor);

    cout << "Cost Before Grad:" << endl;
    cout << Configs.at(iCurConfig).GradDesc.GetLastCost() << endl;

    auto & tf = *it_TargetFunc->second;
    tf.InitMembersByGlobalSettings(TargetFuncSettingsGlobal);

    IsGradCalculating = true;

//    Configs.at(iCurConfig).StartFinalGradDescent(1, ProtoGradDesc, TargetFuncSettingsGlobal,
//                                                 NodesType, pGLWidget);

    Configs.at(iCurConfig).StartFinalGradDescent(1, ProtoGradDesc,
                                                 tf, NodesType, AreaGradDesc, pGLWidget);

    cout << "Cost After Grad:" << endl;
    cout << Configs.at(iCurConfig).GradDesc.GetLastCost() << endl;

    pGLWidget->Repaint();
    QApplication::restoreOverrideCursor();

    return true;
}
//----------------------------------------------------------

void MyGradModel::DeleteConfigsWithUncoveredPoints()
{
    vector<MyConfig> filteredConfigs;

    for (auto & c : Configs)
    {
        if (c.CheckIsAllCovered())
        {
            filteredConfigs.emplace_back(std::move(c));
        }
    }
    filteredConfigs.shrink_to_fit();
    Configs = std::move(filteredConfigs);

    cout << "Configs.size() = " << Configs.size() << endl;
    cout << "Configs.capacity() = " << Configs.capacity() << endl;

    Set_nDraws(Configs.size());
    iCurConfig = Configs.empty() ? -1 : 0;
}
//----------------------------------------------------------

void MyGradModel::CalcBonds()
{
    // Важно! Здесь используются настройки для активной целевой функции первой фазы!
    // Возможно, имеет смысл переделать так, чтобе в c.CalcBonds передавались не все параметры, а только необходимые
    auto it_TargetFunc = TargetFunctions.find(ActiveTargetFuncFirstPhase);
    if (it_TargetFunc == TargetFunctions.end())
        throw std::logic_error("TargetFunctionFirstPhase not found in MyGradModel::CalcBonds");

    for (auto & c : Configs)
    {
        c.CalcPointStats();
        c.CalcBonds(*it_TargetFunc->second, NodesType);
    }
}
//----------------------------------------------------------

bool MyGradModel::SaveToFile(/*const QString &_fileName*/)
{
    if (FileName == "")
    {
        qDebug() << (FileName + " not found");
        return false;
    }

    QJsonDocument jsonDoc;

    QJsonObject mainObject;

    QJsonObject GradDescObject;

    GradDescObject.insert("GradDescFileName", GradDescFileName);

    mainObject.insert("GradDesc", GradDescObject);

    QJsonObject TargetFunctionSettingsObject;

    TargetFunctionSettingsObject.insert("TargetFuncFirstPhase", QString().fromStdString(ActiveTargetFuncFirstPhase));
    TargetFunctionSettingsObject.insert("TargetFuncSecondPhase", QString().fromStdString(ActiveTargetFuncSecondPhase));
    TargetFunctionSettingsObject.insert("TargetFuncFileName", TargetFuncSettingsGlobal.FileName);

    mainObject.insert("TargetFunctionSettings", TargetFunctionSettingsObject);

    QJsonObject ConfigurationObject;

    ConfigurationObject.insert("RouteCount", (int)Routes.size());
    ConfigurationObject.insert("Routes", RepresentRoutesAsJsonArray());

    ConfigurationObject.insert("SignalNodeCount", (int)Nodes.size());

    QString NodesTypeStr = SignalNode::ConvertSignalNodeTypeToString(NodesType);
    ConfigurationObject.insert("SignalNodeType", NodesTypeStr);

    ConfigurationObject.insert("Nodes", RepresentNodesAsJsonArray());

    ConfigurationObject.insert("Count", (int)Configs.size());
    ConfigurationObject.insert("IsRandomNodeCoords", IsRandomNodeCoords);
    ConfigurationObject.insert("IsRandomRelief", IsRandomRelief);
    ConfigurationObject.insert("IsRandomRoutes", IsRandomRoutes);

    ConfigurationObject.insert("ReliefFileName", Relief.GetFileName() );

    ConfigurationObject.insert("GridSettings", GridSettings.RepresentAsJsonObject());


    ConfigurationObject.insert("BoundsRandCoords", BoundsRandCoords.RepresentAsJsonObject());
    ConfigurationObject.insert("BoundsGradDesc", BoundsGradDesc.RepresentAsJsonObject());

    mainObject.insert("Configuration", ConfigurationObject);

    jsonDoc.setObject(mainObject);

    QFile jsonFile(FileName);
    if (jsonFile.open(QIODevice::WriteOnly))
    {        
        jsonFile.write(jsonDoc.toJson());
    }
    else
    {
        qDebug() << "json file not open to write";
        return false;
    }

    IsSaved = true;
    return true;
}
//----------------------------------------------------------

void MyGradModel::TestTwoLines()
{
    if (iCurConfig < 0 || iCurConfig >= (int)Configs.size())
        return;

    auto &c = Configs.at(iCurConfig);

    c.TestTwoLines();
}
//----------------------------------------------------------


void MyGradModel::TestGetLastCostForCurrent()
{
//    cout << "iCurConfig = " << iCurConfig;
    cout << "Last Cost for " << iCurConfig << " = ";
    cout << Configs.at(iCurConfig).GradDesc.GetLastCost();
    cout << endl;
}
//----------------------------------------------------------

void MyGradModel::ReCalcAboAccessRate(TargetFuncTypeEnum funcType)
{
    for (auto & c : Configs)
    {
        c.CalcAccessRateForAbos(TargetFuncSettingsGlobal.IsUseLineBetweenTwoPoints,
                                NodesType,
                                funcType);
    }
}
//----------------------------------------------------------

void MyGradModel::CalcAccessRateForCurrent()
{
    Configs.at(iCurConfig).CalcAccessRateForCurrent();
}
//----------------------------------------------------------

void MyGradModel::SelectCurNodeByPos(double wx, double wy)
{
//    if (wx < -1 || wx > 1 ||    // потом убрать????
//        wy < -1 || wy > 1 )
//    {
//        qDebug() << "wx < -1 || wx > 1 ||"
//                    "wy < -1 || wy > 1";
//        return;
//    }

    double realX, realY; //, realZ;
    /*realZ =*/ Relief.CalcRealXYZbyNormXY(wx, wy, realX, realY);

    if (iCurConfig >= 0)
    {
        auto & cnf = Configs.at(iCurConfig);

        cnf.SelectCurNodeByRealXY(realX, realY);

        const auto & sn = cnf.GetNodes().at(cnf.Get_iCurNode());

        emit SignalSendNodeCoords(cnf.Get_iCurNode(),
                                  sn.Pos.x(),
                                  sn.Pos.y(),
                                  sn.Pos.z());
    }
    else
    {
        qDebug() << "iCurConfig == -1. MyGradModel::SelectCurNodeByPos aborted";
    }
}
//----------------------------------------------------------

void MyGradModel::PutCurNodeByPos(double wx, double wy)
{
    double realX, realY, realZ;
    realZ = Relief.CalcRealXYZbyNormXY(wx, wy, realX, realY);

    if (iCurConfig >= 0)
    {
        Configs.at(iCurConfig).PutCurNodeByRealXYZ(realX, realY, realZ);
    }
    else
    {
        qDebug() << "iCurConfig == -1. MyGradModel::PutCurNodeByPos aborted";
    }
}
//----------------------------------------------------------

void MyGradModel::SetDirectCurNodeByPos(double wx, double wy)
{
    double realX, realY, realZ;
    realZ = Relief.CalcRealXYZbyNormXY(wx, wy, realX, realY);

    if (iCurConfig >= 0)
    {
        Configs.at(iCurConfig).SetDirectCurNodeByRealXYZ(realX, realY, realZ);
    }
    else
    {
        qDebug() << "iCurConfig == -1. MyGradModel::SetDirectCurNodeByPos aborted";
    }
}
//----------------------------------------------------------

void MyGradModel::SetShowGridOnRelief(bool _isShow)
{
    WhatShow.ShowGrid = _isShow;

    if (_isShow)
    {
        Relief.ReCreateGridListsGL();
//        Relief.ReBuildGridToGL(true, 5000, 3000, 201);
//        Relief.ReBuildGridToGL(false, 5000, 3000, 201);

//        Relief.ReBuildGridToGL(true, 100, 100, 201);
//        Relief.ReBuildGridToGL(false, 100, 100, 201);

        Relief.ReBuildGridToGL(false, GridSettings.dx, GridSettings.dy, GridSettings.nDetails);
        Relief.ReBuildGridToGL(true, GridSettings.dx, GridSettings.dy, 2);
    }
    else
    {
        Relief.ClearGrid();
    }
}
//----------------------------------------------------------

//TargetFuncEnum MyGradModel::ConvertStringToTargetFuncType(QString &str) // static member-function
//{
//    if (str.toUpper() == "ADDITIVE")
//        return TargetFuncEnum::Additive;
//    else if (str.toUpper() == "PROBABILISTIC")
//        return TargetFuncEnum::Probabilistic;
//    else
//        return TargetFuncEnum::Empty;
//}
////----------------------------------------------------------

//QString MyGradModel::ConvertTargetFuncTypeToString(TargetFuncEnum snt) // static member-function
//{
//    switch (snt)
//    {
//    case TargetFuncEnum::Additive:
//        return "Additive";
//    case TargetFuncEnum::Probabilistic:
//        return "Probabilistic";
//    case TargetFuncEnum::Empty:
//        return "Empty";
//    default:
//        return "Unknown";
//    }
//}
//----------------------------------------------------------

bool MyGradModel::SaveGlobalSettings() const
{
    QJsonDocument jsonDoc;

    QJsonObject mainObject;

    mainObject.insert("WhatShow", WhatShow.RepresentAsJsonObject());

    jsonDoc.setObject(mainObject);

    QFile jsonFile(GlobalSettingsFileName);
    if (jsonFile.open(QIODevice::WriteOnly))
    {
        jsonFile.write(jsonDoc.toJson());
    }
    else
    {
        qDebug() << "json file not open to write";
        return false;
    }

    return true;
}
//----------------------------------------------------------

bool MyGradModel::LoadGlobalSettings()
{
    QFile json(GlobalSettingsFileName);
    if (json.open(QIODevice::ReadOnly))
    {
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(json.readAll(), &parseError);
        if (parseError.error == QJsonParseError::NoError)
        {
            if (jsonDoc.isObject())
            {
                WhatShow.LoadFromJsonObject(jsonDoc["WhatShow"].toObject());
            }
        }
        else
        {
            qDebug() << parseError.errorString();
            return false;
        }

    }
    else
    {
        qDebug() << "json file not open";
        return false;
    }

    SetShowGridOnRelief(WhatShow.ShowGrid);

    return true;
}
//----------------------------------------------------------



