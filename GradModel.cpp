#include "GradModel.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include <QApplication>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileInfo>
#include <QFileDialog>


#include <iostream>
using namespace std;

constexpr float RotSpeed = 0.12f;
constexpr float TransSpeed = 0.006f;

MyGradModel::MyGradModel()
{
    ProtoGradDesc.SetIsUseUserTargetFunction(true);
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

        double minSize = min(Width, Height);

        glViewport(5, 5, minSize-5*2, minSize-5*2);
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

    Configs.at(ind).DrawIn3D(NodesType, IsDrawAbonents);
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

    QVector3D PosToDel = {(float)realX, (float)realY, (float)realZ};
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

    ReliefMatInfo = ReliefMatInfoStruct();
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

void MyGradModel::ReCreateSignalNodes(size_t _count, double _R, double _betha)
{
    Nodes.clear();
    Nodes.reserve(_count);

    for (size_t i = 0; i < _count; ++i)
    {
        Nodes.emplace_back(QVector3D(), _R, 0, _betha);
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
            cnf.SetRandomNodeCoords();
        }
    }
    IsSaved = false;
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

const MyConfig &MyGradModel::GetCurrentConfig() const
{
    if (iCurConfig < 0 || iCurConfig >= (int)Configs.size())
        throw std::exception();

    return Configs.at(iCurConfig);
}
//----------------------------------------------------------

MyConfig & MyGradModel::CurrentConfigAccess()
{
    if (iCurConfig < 0 || iCurConfig >= (int)Configs.size())
        throw std::exception();

    return Configs.at(iCurConfig);
}
//----------------------------------------------------------

void MyGradModel::OnMousePress(QMouseEvent *pe)
{
    OldX = pe->pos().x();
    OldY = pe->pos().y();

    //qDebug() << "x =" << OldX;
    //qDebug() << "y =" << OldY;

    if (!DrawOnlyOne)
    {
        iCurConfig = CalcCurViewPortNumber(pe->pos().x(), pe->pos().y());
//        size_t i = 0;
//        for (const auto & v : ViewPorts)
//        {
//            //qDebug() << v;

//            if ( !v.isValid() )
//                throw std::runtime_error("!v.isValid() ");

//            if ( v.contains(pe->x(), pe->y()) )
//                iCurConfig = i;

//            i++;
//        }
    }

    if (pe->type() == QMouseEvent::MouseButtonDblClick && iCurConfig >= 0)
    {
        SwitchDrawOnlyOne();
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
        Configs.at(iCurConfig).Settings3d.TrX += TransSpeed*dx;
        Configs.at(iCurConfig).Settings3d.TrY -= TransSpeed*dy;
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

void MyGradModel::OnMouseWheel(QWheelEvent *pe)
{
    if (iCurConfig < 0 || iCurConfig >= (int)Configs.size())
        return;

    //if (Configs.at(iCurConfig).Settings3d.IsPerpective)
    Configs.at(iCurConfig).Settings3d.TrZ += pe->angleDelta().y() / 600.0f;
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
    qDebug() << "MyGradModel:";
    qDebug() << "Name = " << Name;

    const QJsonObject &configObject = _jsonObject["Configuration"].toObject();

    size_t ConfigCount = configObject["Count"].toInt(0);
//    Configs.reserve(ConfigCount);
    qDebug() << "ConfigCount = " << ConfigCount;

    if ( !configObject["Area"].isObject() )
    {
        qDebug() << _parseError.errorString(); return 0;
    }


//    Area.setLeft(areaObject["left"].toDouble(-1));
//    Area.setRight(areaObject["right"].toDouble(-1));
//    Area.setTop(areaObject["top"].toDouble(-1));
//    Area.setBottom(areaObject["bottom"].toDouble(-1));


    // Здесь должна быть загрузка параметров рельефа
    const QJsonObject &reliefInfoObject = configObject["ReliefInfo"].toObject();

    ReliefMatInfo.IsUseReliefRandomSeed = reliefInfoObject["IsUseReliefRandomSeed"].toBool(false);
    ReliefMatInfo.ReliefRandomSeed = reliefInfoObject["ReliefRandomSeed"].toInt(200);

    const QJsonObject &reliefCoeffsObject = reliefInfoObject["ReliefCoeffs"].toObject();

    ReliefMatInfo.A_r1 = reliefCoeffsObject["A_r1"].toDouble(-1);
    ReliefMatInfo.A_r2 = reliefCoeffsObject["A_r2"].toDouble(-1);

    cout << "ReliefInfo:" << endl << ReliefMatInfo << endl;

    IsRandomRelief = configObject["IsRandomRelief"].toBool(true);
    if (!IsRandomRelief)
    {
        QString ReliefFileName = configObject["ReliefFileName"].toString();

        if ( !QFile::exists(ReliefFileName) )
        {
            QFileInfo fileInfo(ReliefFileName);
            ReliefFileName = fileInfo.fileName();
        }

        if ( !QFile::exists(ReliefFileName) )
        {
            auto res = QMessageBox::question(nullptr, "Question", "Relief File not Found. Would you like to choose Relif file?");
            if (res == QMessageBox::Yes)
            {
                ReliefFileName = QFileDialog::getOpenFileName(nullptr,
                                          "Choose Relief file", ".", "Relif Files (*.json)");

                if (ReliefFileName.isEmpty())
                {
                    QMessageBox::critical(nullptr, "Error", "Relief file not set and won't be loaded");
                }

            }
            else
            {
                QMessageBox::critical(nullptr, "Error", "Relief file not set and won't be loaded");
            }
        }

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
            qDebug() << "\t" << p.Pos << "\tw = " << p.Weight;
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
//        const QJsonObject &nodeObject = it->toObject();
        Nodes.emplace_back(SignalNode());
        Nodes.back().LoadFromJsonObject(it->toObject());

//        double R = nodeObject["R"].toDouble(-1);
//        double Beta = qDegreesToRadians(nodeObject["Beta"].toDouble(0));
//        Nodes.emplace_back(QVector3D(), R, 0, Beta);

//        Routes.back().Points.emplace_back(RoutePoint());

//        Routes.back().AbonentDirectAccess().
//                LoadFromJsonObject(routeObject["Abonent"].toObject());

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

    const QJsonObject &gradDescObject = _jsonObject["GradDesc"].toObject();

    ProtoGradDesc.SetAlpha(gradDescObject["Alpha"].toDouble(-1));
    ProtoGradDesc.SetEps(gradDescObject["Eps"].toDouble(-1));
    ProtoGradDesc.SetEta_FirstJump(gradDescObject["Eta_FirstJump"].toDouble(-1));
    ProtoGradDesc.SetEta_k_inc(gradDescObject["Eta_k_inc"].toDouble(-1));
    ProtoGradDesc.SetEta_k_dec(gradDescObject["Eta_k_dec"].toDouble(-1));
//    ProtoGradDesc.SetEta_FirstJump(gradDescObject["Eta_FirstJump"].toDouble(-1));
    ProtoGradDesc.SetMin_Eta(gradDescObject["Min_Eta"].toDouble(-1));
    ProtoGradDesc.SetFinDifMethod(gradDescObject["FinDifMethod"].toBool(false));
    ProtoGradDesc.SetMaxIters(gradDescObject["MaxIters"].toInt(0));
    ProtoGradDesc.SetMaxTime(gradDescObject["MaxTime"].toDouble(-1));
    ProtoGradDesc.SetCallBackFreq(gradDescObject["CallBackFreq"].toInt(1));


    const QJsonObject &targetFuncObject = _jsonObject["TargetFunctionSettings"].toObject();

    TargetFuncSettings.Aarf             = targetFuncObject["Aarf"].toDouble(-1);
    TargetFuncSettings.A2               = targetFuncObject["A2"].toDouble(-1);
    TargetFuncSettings.p                = targetFuncObject["p"].toDouble(-1);
    TargetFuncSettings.offX             = targetFuncObject["offX"].toDouble(-1);
    TargetFuncSettings.k_step_ot        = targetFuncObject["k_step_ot"].toDouble(-1);
    TargetFuncSettings.R_nodeOverlap    = targetFuncObject["R_nodeOverlap"].toDouble(-1);
    TargetFuncSettings.IsUseCoveredFlag = targetFuncObject["IsUseCoveredFlag"].toBool(false);

    TargetFuncSettings.IsUseLineBetweenTwoPoints = targetFuncObject["IsUseLineBetweenTwoPoints"].toBool(false);

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
            pointObject.insert("x", p.Pos.x());
            pointObject.insert("y", p.Pos.y());
            pointObject.insert("z", p.Pos.z());
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

QJsonObject MyGradModel::RepresentReliefInfoAsJsonObject() const
{
    QJsonObject ReliefInfoObject;

    ReliefInfoObject.insert("IsUseReliefRandomSeed", ReliefMatInfo.IsUseReliefRandomSeed);
    ReliefInfoObject.insert("ReliefRandomSeed", (int64_t)ReliefMatInfo.ReliefRandomSeed); // int64_t ?

    QJsonObject ReliefCoeffsObject;
    ReliefCoeffsObject.insert("A_r1", ReliefMatInfo.A_r1);
    ReliefCoeffsObject.insert("A_r2", ReliefMatInfo.A_r2);

    ReliefInfoObject.insert("ReliefCoeffs", ReliefCoeffsObject);

    return ReliefInfoObject;
}
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


    CreatePopulation(configCount);

    if (Relief.GetIsMathRelief())
        Relief.CreateMathRelief(ReliefMatInfo);

    Relief.ReCreateListsGL();
    Relief.BuildReliefToGL(false);
    Relief.BuildReliefToGL(true);

    //IsLoaded = true;
    IsSaved = true;
    return true;
}
//----------------------------------------------------------

void MyGradModel::CreatePopulation(size_t _count)
{
    Configs.clear();

    MyConfig protoConfig;

//    protoConfig.SetArea(Relief.GetArea());

    protoConfig.SetRelief(&Relief);
    protoConfig.SetNodes(Nodes);
    protoConfig.SetRoutes(Routes);

    if (IsRandomNodeCoords)
    {
//        protoConfig.SetRandomNodeCoords(Relief.GetArea());
        protoConfig.SetRandomNodeCoords();
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
            cnf.SetRandomNodeCoords();
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
    int iDraw = nDraws;
    QApplication::setOverrideCursor(Qt::WaitCursor);

//    cout << "Costs Before GradDesc:" << endl;
//    for (auto & c : Configs)
//        cout << c.GradDesc.GetLastCost() << endl;

    IsGradCalculating = true;
    for (auto & c : Configs)
    {
        if (!IsGradCalculating)
            break;
        c.StartGradDescent(iDraw, ProtoGradDesc, TargetFuncSettings, NodesType, pGLWidget);
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

    QApplication::setOverrideCursor(Qt::WaitCursor);

    cout << "Cost Before Grad:" << endl;
    cout << Configs.at(iCurConfig).GradDesc.GetLastCost() << endl;

    IsGradCalculating = true;

    Configs.at(iCurConfig).StartGradDescent(1, ProtoGradDesc, TargetFuncSettings,
                                            NodesType, pGLWidget);

//    for (auto & c : Configs)
//    {
//        if (!IsGradCalculating)
//            break;
//        c.StartGradDescent(iDraw, ProtoGradDesc, TargetFuncSettings, pGLWidget);
//        --iDraw;
//    }

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
    int iDraw = nDraws;
    QApplication::setOverrideCursor(Qt::WaitCursor);

//    cout << "Costs Before GradDesc:" << endl;
//    for (auto & c : Configs)
//        cout << c.GradDesc.GetLastCost() << endl;

    IsGradCalculating = true;
    for (auto & c : Configs)
    {
        if (!IsGradCalculating)
            break;
        c.StartFinalGradDescent(iDraw, ProtoGradDesc, TargetFuncSettings,
                                NodesType, pGLWidget);
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

    QApplication::setOverrideCursor(Qt::WaitCursor);

    cout << "Cost Before Grad:" << endl;
    cout << Configs.at(iCurConfig).GradDesc.GetLastCost() << endl;

    IsGradCalculating = true;

    Configs.at(iCurConfig).StartFinalGradDescent(1, ProtoGradDesc, TargetFuncSettings,
                                                 NodesType, pGLWidget);

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
    for (auto & c : Configs)
    {
        c.CalcPointStats();
        c.CalcBonds(TargetFuncSettings, NodesType);
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

    GradDescObject.insert("Alpha", ProtoGradDesc.GetAlpha());
    GradDescObject.insert("CallBackFreq", (int)ProtoGradDesc.GetCallBackFreq());
    GradDescObject.insert("Eps", ProtoGradDesc.GetEps());
    GradDescObject.insert("Eta_FirstJump", ProtoGradDesc.GetEta_FirstJump());
    GradDescObject.insert("Eta_k_dec", ProtoGradDesc.GetEta_k_dec());
    GradDescObject.insert("Eta_k_inc", ProtoGradDesc.GetEta_k_inc());
    GradDescObject.insert("FinDifMethod", ProtoGradDesc.GetFinDifMethod());
    GradDescObject.insert("MaxIters", (int)ProtoGradDesc.GetMaxIters());
    GradDescObject.insert("MaxTime", ProtoGradDesc.GetMaxTime());
    GradDescObject.insert("Min_Eta", ProtoGradDesc.GetMin_Eta());

    mainObject.insert("GradDesc", GradDescObject);


    QJsonObject TargetFunctionSettingsObject;

    TargetFunctionSettingsObject.insert("A2", TargetFuncSettings.A2);
    TargetFunctionSettingsObject.insert("Aarf", TargetFuncSettings.Aarf);
    TargetFunctionSettingsObject.insert("IsUseCoveredFlag", TargetFuncSettings.IsUseCoveredFlag);
    TargetFunctionSettingsObject.insert("R_nodeOverlap", TargetFuncSettings.R_nodeOverlap);
    TargetFunctionSettingsObject.insert("k_step_ot", TargetFuncSettings.k_step_ot);
    TargetFunctionSettingsObject.insert("offX", TargetFuncSettings.offX);
    TargetFunctionSettingsObject.insert("p", TargetFuncSettings.p);
    TargetFunctionSettingsObject.insert("IsUseLineBetweenTwoPoints", TargetFuncSettings.IsUseLineBetweenTwoPoints);

    mainObject.insert("TargetFunctionSettings", TargetFunctionSettingsObject);

    QJsonObject ConfigurationObject;

    ConfigurationObject.insert("RouteCount", (int)Routes.size());
    ConfigurationObject.insert("Routes", RepresentRoutesAsJsonArray());

    ConfigurationObject.insert("SignalNodeCount", (int)Nodes.size());
    QString NodesTypeStr = SignalNode::ConvertSignalNodeTypeToString(NodesType);
    ConfigurationObject.insert("SignalNodeType", NodesTypeStr);
    ConfigurationObject.insert("Nodes", RepresentNodesAsJsonArray());

    ConfigurationObject.insert("TargetCostFunction", "MyFunction"); // Not used

    QJsonObject AreaObject; // Для математичесокго рельефа
    AreaObject.insert("bottom", 0);
    AreaObject.insert("left", 0);
    AreaObject.insert("right", 10000);
    AreaObject.insert("top", 8000);
    ConfigurationObject.insert("Area", AreaObject); // Для математичесокго рельефа

    ConfigurationObject.insert("Count", (int)Configs.size());
    ConfigurationObject.insert("IsRandomNodeCoords", IsRandomNodeCoords);
    ConfigurationObject.insert("IsRandomRelief", IsRandomRelief);
    ConfigurationObject.insert("IsRandomRoutes", IsRandomRoutes);

    ConfigurationObject.insert("ReliefInfo", RepresentReliefInfoAsJsonObject());
    ConfigurationObject.insert("ReliefFileName", Relief.GetFileName() );

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

void MyGradModel::ReCalcAboAccessRate()
{
    for (auto & c : Configs)
    {
        c.CalcAccessRateForAbos(TargetFuncSettings.IsUseLineBetweenTwoPoints,
                                NodesType); // Заменить на мембер или типа того ?
    }
}
//----------------------------------------------------------



