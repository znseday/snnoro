#include "GradConfig.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include <algorithm>
#include <numeric>

#include <QApplication>
#include <QDebug>
#include <fstream>

#include "TargetFunctions/TargetFunctionBase.h"

static constexpr double arfThreshold = 0.5;

const QString ConfigsExtension = "*.cnf";

static constexpr float zOffset = 0.01f;

using namespace std;


//----------------------------------------------------------
BoundsTypeEnum BoundsStruct::ConvertStringToBoundsTypeEnum(const QString &str)  // static
{
    if (str.toUpper() == "ALLAREA" || str.toUpper() == "ALL_AREA")
        return BoundsTypeEnum::AllArea;
    else if (str.toUpper() == "BYROUTES" || str.toUpper() == "BY_ROUTES")
        return BoundsTypeEnum::ByRoutes;
    else if (str.toUpper() == "SELECTED")
        return BoundsTypeEnum::Selected;
    else
    {
        qDebug() << "Warning! Unknown BoundsTypeEnum in BoundsStruct::ConvertStringToBoundsTypeEnum";
        qDebug() << "BoundsTypeEnum::AllArea will be set as default";
        return BoundsTypeEnum::AllArea;
    }
}
//----------------------------------------------------------

QString BoundsStruct::ConvertBoundsTypeEnumToString(BoundsTypeEnum bt)  // static
{
    switch (bt)
    {
    case BoundsTypeEnum::AllArea:
        return "AllArea";
    case BoundsTypeEnum::ByRoutes:
        return "ByRoutes";
    case BoundsTypeEnum::Selected:
        return "Selected";
    default:
        throw std::runtime_error("Unknown BoundsTypeEnum in BoundsStruct::ConvertBoundsTypeEnumToString");
    }
}
//----------------------------------------------------------

QJsonObject BoundsStruct::RepresentAsJsonObject() const
{
    QJsonObject res;
    res.insert("BoundsType", ConvertBoundsTypeEnumToString(BoundsType));
    res.insert("SelXstart", SelXstart);
    res.insert("SelYstart", SelYstart);
    res.insert("SelW", SelW);
    res.insert("SelH", SelH);
    return res;
}
//----------------------------------------------------------

void BoundsStruct::LoadFromJsonObject(const QJsonObject &_jsonObject)
{
    BoundsType = ConvertStringToBoundsTypeEnum(_jsonObject["BoundsType"].toString("Unknown"));
    SelXstart = _jsonObject["SelXstart"].toDouble(0);
    SelYstart = _jsonObject["SelYstart"].toDouble(0);
    SelW = _jsonObject["SelW"].toDouble(100);
    SelH = _jsonObject["SelH"].toDouble(100);
}
//----------------------------------------------------------
//----------------------------------------------------------

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

void MyConfig::SetRandomNodeCoords(const QRectF &_area)
//void MyConfig::SetRandomNodeCoords()
{
    if (!Relief)
        throw std::runtime_error("There is no relief in MyConfig");

    for (auto & node : Nodes)
    {
//        node.SetRandomCoord(*Relief);
        node.SetRandomCoord(_area, *Relief);
        node.SetRandomAlpha();
    }
}
//----------------------------------------------------------

void MyConfig::ReCreateIsolinesARFListsGL()
{
    ClearIsolinesARF();

    IsolinesARFCompileList = glGenLists(1);
    IsolinesARF2dCompileList = glGenLists(1);

//    qDebug() << "GridCompileList =" << GridCompileList;
//    qDebug() << "Grid2dCompileList =" << Grid2dCompileList;
}
//----------------------------------------------------------

void MyConfig::ClearIsolinesARF()
{
    if (IsolinesARFCompileList)
        glDeleteLists(IsolinesARFCompileList, 1);
    if (IsolinesARF2dCompileList)
        glDeleteLists(IsolinesARF2dCompileList, 1);

    IsolinesARFCompileList = 0;
    IsIsolinesARFBuilt = false;
    IsolinesARF2dCompileList = 0;
    IsIsolinesARF2dBuilt = false;
}
//----------------------------------------------------------

/*static*/ void MyConfig::HandleSquare(const MyPos3d<> &p0, const MyPos3d<> &p1,
                         const MyPos3d<> &p2, const MyPos3d<> &p3,
                         double level, double offsetX, double offsetY, double offsetZ, double k, bool _is2d)
{
    int Case = 0;

    if (p0.z() >= level)
        Case |= 1 << 3;
    if (p1.z() >= level)
        Case |= 1 << 2;
    if (p2.z() >= level)
        Case |= 1 << 1;
    if (p3.z() >= level)
        Case |= 1;

    if (Case == 0b1111 || Case == 0b0000)
    {
        return; // no contour
    }

    double dx = p2.x() - p3.x();
    double dy = p0.y() - p3.y();

    double x1, y1, x2, y2, z1, z2;

    if (Case == 0b1110 || Case == 0b0001)
    {
        x1 = p3.x() + (level - p3.z()) / (p2.z() - p3.z()) * dx;
        y1 = p3.y();
        z1 = Relief->CalcRealZbyRealXY(x1, y1);

        x2 = p3.x();
        y2 = p3.y() + (level - p3.z()) / (p0.z() - p3.z()) * dy;
        z2 = Relief->CalcRealZbyRealXY(x2, y2);
    }
    else if (Case == 0b1101 || Case == 0b0010)
    {
        x1 = p2.x() - (level - p2.z()) / (p3.z() - p2.z()) * dx;
        y1 = p3.y();
        z1 = Relief->CalcRealZbyRealXY(x1, y1);

        x2 = p2.x();
        y2 = p2.y() + (level - p2.z()) / (p1.z() - p2.z()) * dy;
        z2 = Relief->CalcRealZbyRealXY(x2, y2);
    }
    else if (Case == 0b1011 || Case == 0b0100)
    {
        x1 = p1.x() - (level - p1.z()) / (p0.z() - p1.z()) * dx;
        y1 = p0.y();
        z1 = Relief->CalcRealZbyRealXY(x1, y1);

        x2 = p1.x();
        y2 = p1.y() - (level - p1.z()) / (p2.z() - p1.z()) * dy;
        z2 = Relief->CalcRealZbyRealXY(x2, y2);
    }
    else if (Case == 0b0111 || Case == 0b1000)
    {
        x1 = p0.x() + (level - p0.z()) / (p1.z() - p0.z()) * dx;
        y1 = p0.y();
        z1 = Relief->CalcRealZbyRealXY(x1, y1);

        x2 = p0.x();
        y2 = p0.y() - (level - p0.z()) / (p3.z() - p0.z()) * dy;
        z2 = Relief->CalcRealZbyRealXY(x2, y2);
    }
    else if (Case == 0b1100 || Case == 0b0011)
    {
        x1 = p3.x();
        y1 = p3.y() + (level - p3.z()) / (p0.z() - p3.z()) * dy;
        z1 = Relief->CalcRealZbyRealXY(x1, y1);

        x2 = p2.x();
        y2 = p2.y() + (level - p2.z()) / (p1.z() - p2.z()) * dy;
        z2 = Relief->CalcRealZbyRealXY(x2, y2);
    }
    else if (Case == 0b1001 || Case == 0b0110)
    {
        x1 = p2.x() - (level - p2.z()) / (p3.z() - p2.z()) * dx;
        y1 = p2.y();
        z1 = Relief->CalcRealZbyRealXY(x1, y1);

        x2 = p1.x() - (level - p1.z()) / (p0.z() - p1.z()) * dx;
        y2 = p1.y();
        z2 = Relief->CalcRealZbyRealXY(x2, y2);
    }
    else if ((Case == 0b1010 && Relief->CalcRealZbyRealXY((p2.x() + p3.x()) / 2.0, (p3.y() + p0.y()) / 2.0) < 0) ||
             (Case == 0b0101 && Relief->CalcRealZbyRealXY((p2.x() + p3.x()) / 2.0, (p3.y() + p0.y()) / 2.0) >= 0) )
    {
        x1 = p3.x() + (level - p3.z()) / (p2.z() - p3.z()) * dx;
        y1 = p3.y();
        z1 = Relief->CalcRealZbyRealXY(x1, y1);

        x2 = p2.x();
        y2 = p1.y() - (level - p1.z()) / (p2.z() - p1.z()) * dy;
        z2 = Relief->CalcRealZbyRealXY(x2, y2);

        double x, y, z;

        x = (x1 - offsetX)*k;
        y = (y1 - offsetY)*k;
        z = (z1 - offsetZ)*Relief->GetGlobal_kz();
        glVertex3f(x, y, (_is2d ? 0 : z) + zOffset);

        x = (x2 - offsetX)*k;
        y = (y2 - offsetY)*k;
        z = (z2 - offsetZ)*Relief->GetGlobal_kz();
        glVertex3f(x, y, (_is2d ? 0 : z) + zOffset);
        //---

        x1 = p1.x() - (level - p1.z()) / (p0.z() - p1.z()) * dx;
        y1 = p1.y();
        z1 = Relief->CalcRealZbyRealXY(x1, y1);

        x2 = p0.x();
        y2 = p3.y() + (level - p3.z()) / (p0.z() - p3.z()) * dy;
        z2 = Relief->CalcRealZbyRealXY(x2, y2);

    }
    else
    {

        x1 = p3.x() + (level - p3.z()) / (p2.z() - p3.z()) * dx;
        y1 = p3.y();
        z1 = Relief->CalcRealZbyRealXY(x1, y1);

        x2 = p3.x();
        y2 = p3.y() + (level - p3.z()) / (p0.z() - p3.z()) * dy;
        z2 = Relief->CalcRealZbyRealXY(x2, y2);

        double x, y, z;

        x = (x1 - offsetX)*k;
        y = (y1 - offsetY)*k;
        z = (z1 - offsetZ)*Relief->GetGlobal_kz();
        glVertex3f(x, y, (_is2d ? 0 : z) + zOffset);

        x = (x2 - offsetX)*k;
        y = (y2 - offsetY)*k;
        z = (z2 - offsetZ)*Relief->GetGlobal_kz();
        glVertex3f(x, y, (_is2d ? 0 : z) + zOffset);
        //---

        x1 = p1.x() - (level - p1.z()) / (p0.z() - p1.z()) * dx;
        y1 = p1.y();
        z1 = Relief->CalcRealZbyRealXY(x1, y1);

        x2 = p1.x();
        y2 = p1.y() - (level - p1.z()) / (p2.z() - p1.z()) * dy;

        z2 = Relief->CalcRealZbyRealXY(x2, y2);

    }

    double x, y, z;

    x = (x1 - offsetX)*k;
    y = (y1 - offsetY)*k;
    z = (z1 - offsetZ)*Relief->GetGlobal_kz();
    glVertex3f(x, y, (_is2d ? 0 : z) + zOffset);

    x = (x2 - offsetX)*k;
    y = (y2 - offsetY)*k;
    z = (z2 - offsetZ)*Relief->GetGlobal_kz();
    glVertex3f(x, y, (_is2d ? 0 : z) + zOffset);
}
//----------------------------------------------------------

void MyConfig::ReBuildIsolinesARFToGL(bool _is2d, int nLevels,
                                  int nDetail, bool _isShowPoints,
                                  bool _isUseLineBetweenTwoPoints,
                                  SignalNodeType _snt,
                                  TargetFuncTypeEnum funcType,
                                  WhatShowStruct::WhatIsolinesEnum wi)
{
    if (!_is2d && IsIsolinesARFBuilt)
        throw std::runtime_error("IsGridBuilt == true in Relief3D::ReBuildGridToGL");
    if (_is2d && IsIsolinesARF2dBuilt)
        throw std::runtime_error("IsGrid2dBuilt == true in Relief3D::ReBuildGridToGL");

    int RowCount = nDetail;
    int ColCount = nDetail;

    if (!_is2d)
        glNewList(IsolinesARFCompileList, GL_COMPILE);
    else
        glNewList(IsolinesARF2dCompileList, GL_COMPILE);


    const auto & _area = Relief->GetArea();
    constexpr float zOffset = 0.0001f;
    double kx = 2.0/_area.width();
    double ky = 2.0/_area.height();
    double k = min(kx, ky);
//    double k = max(kx, ky);

    double hW = _area.width()/2.0;
    double hH = _area.height()/2.0;

    double offsetX = _area.x()+hW; // in meters
    double offsetY = _area.y()+hH; // in meters
    double offsetZ = 0;

    std::vector<MyVector3D<>> oneRow(ColCount);
    std::vector<std::vector<MyVector3D<>>> points(RowCount, oneRow);

//    const double aspect = _area.width()/_area.height();

//    double dx = _area.width() / double(nDetail);   // in meters
//    double dy = _area.height() / double(nDetail);  // in meters

    double dx = _area.width() / double(nDetail-1);   // in meters
    double dy = _area.height() / double(nDetail-1);  // in meters

//    double dxInside = 2.0*dx/_area.width();
//    double dyInside = 2.0*dy/_area.height();

//    if (aspect > 1)
//        dyInside /= aspect;
//    else
//        dxInside *= aspect;


    for (int i = 0; i < RowCount; ++i)
    {
        for (int j = 0; j < ColCount; ++j)
        {
            double x = _area.left() + /*dx/2.0*/ + dx*j;
            double y = _area.top() + /*dy/2.0*/ + dy*i;
            double z = Relief->CalcRealZbyRealXY(x, y);

            if (wi == WhatShowStruct::WhatIsolinesEnum::Relief)
            {
                points.at(i).at(j) = {x, y, z};
            }
            else if (wi == WhatShowStruct::WhatIsolinesEnum::ARF)
            {
                double arf = CalcAccessRateForAnyPos({x, y, z}, _isUseLineBetweenTwoPoints, _snt, funcType);
                points.at(i).at(j) = {x, y, arf};
            }
            else
            {
                throw std::runtime_error("Unknown WhatShowStruct::WhatIsolinesEnum in MyConfig::ReBuildIsolinesARFToGL");
            }
        }
    }
    //--------------------------------------------------

    const double dColor = 1.0 / nLevels;
    double levelMin = 0;
    double levelMax = 0;

    for (int i = 0; i < RowCount; ++i)
    {
        for (int j = 0; j < ColCount; ++j)
        {
            levelMin = std::min(levelMin, (double)points.at(i).at(j).z());
            levelMax = std::max(levelMax, (double)points.at(i).at(j).z());
        }
    }

//    qDebug() << "levelMin = " << levelMin;
//    qDebug() << "levelMax = " << levelMax;

    const double dLevel = (levelMax - levelMin) / double(nLevels);


    glColor3f(0.5f, 0.5f, 0.5f);

    if (_isShowPoints)
    {
        glPointSize(3.0);
        glBegin(GL_POINTS);
        for (int i = 0; i < RowCount; ++i)
        {
            for (int j = 0; j < ColCount; ++j)
            {
                double x = (points.at(i).at(j).x() - offsetX)*k;
                double y = (points.at(i).at(j).y() - offsetY)*k;
                double z = (points.at(i).at(j).z() - offsetZ)*Relief->GetGlobal_kz();

                glVertex3f(x, y, (_is2d ? 0 : z) + zOffset);
            }
        }
        glEnd();
    }


    for (int n = 0; n < nLevels; ++n)
    {
        double level = dLevel/2.0 + dLevel*n;
//        qDebug() << "level = " << level;

        double color = dColor/2.0 + dColor*n;

        glColor3f(color/2.0, 0, color);

        glBegin(GL_LINES);
        for (int i = 0; i < RowCount-1; ++i)
        {
            for (int j = 0; j < ColCount-1; ++j)
            {
                HandleSquare(points.at(i+1).at(j), points.at(i+1).at(j+1),
                             points.at(i).at(j+1), points.at(i).at(j),
                             level, offsetX, offsetY, offsetZ, k, _is2d);
            }
        }
        glEnd();
    }

    glEndList(); // закончить список

    if (!_is2d)
        IsIsolinesARFBuilt = true;
    else
        IsIsolinesARF2dBuilt = true;
}
//----------------------------------------------------------

void MyConfig::DrawSomeArea(const QRectF & _area, double offsetX, double offsetY,
                         double offsetZ, double k, bool _isPerpective) const
{
    int n = _isPerpective ? 101 : 2;
    double dx = _area.width()  / (n-1);
    double dy = _area.height() / (n-1);
    glBegin(GL_LINE_STRIP);
        for (int i = 0; i < n; ++i) // горизонтальная
        {
            double x = _area.left() + i*dx;
            double y = _area.bottom();
            double z = (Relief->CalcRealZbyRealXY(x, y) - offsetZ)*Relief->GetGlobal_kz();
            x = (x-offsetX)*k;
            y = (y-offsetY)*k;
            glVertex3f(x, y, zOffset + (Settings3d.IsPerspective ? z : 0));
        }
    glEnd();
    glBegin(GL_LINE_STRIP);
        for (int i = 0; i < n; ++i) // горизонтальная
        {
            double x = _area.left() + i*dx;
            double y = _area.top();
            double z = (Relief->CalcRealZbyRealXY(x, y) -offsetZ)*Relief->GetGlobal_kz();
            x = (x-offsetX)*k;
            y = (y-offsetY)*k;
            glVertex3f(x, y, zOffset + (Settings3d.IsPerspective ? z : 0));
        }
    glEnd();
    glBegin(GL_LINE_STRIP);
        for (int i = 0; i < n; ++i) // вертикальная
        {
            double x = _area.left();
            double y = _area.top() + i*dy;
            double z = (Relief->CalcRealZbyRealXY(x, y) -offsetZ)*Relief->GetGlobal_kz();
            x = (x-offsetX)*k;
            y = (y-offsetY)*k;
            glVertex3f(x, y, zOffset + (Settings3d.IsPerspective ? z : 0));
        }
    glEnd();
    glBegin(GL_LINE_STRIP);
        for (int i = 0; i < n; ++i) // вертикальная
        {
            double x = _area.right();
            double y = _area.top() + i*dy;
            double z = (Relief->CalcRealZbyRealXY(x, y) -offsetZ)*Relief->GetGlobal_kz();
            x = (x-offsetX)*k;
            y = (y-offsetY)*k;
            glVertex3f(x, y, zOffset + (Settings3d.IsPerspective ? z : 0));
        }
    glEnd();
}
//----------------------------------------------------------

void MyConfig::DrawIn3D(SignalNodeType _snt, bool isDrawAbonents,
                        const QRectF &_areaNodeCoords, const QRectF &_areaGradDesc,
                        const WhatShowStruct &_whatShow) const
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
//        glTranslatef(Settings3d.TrX, Settings3d.TrY, Settings3d.TrZ);

        gluLookAt(Settings3d.eyeX, Settings3d.eyeY, Settings3d.eyeZ,
                  Settings3d.centerX, Settings3d.centerY, Settings3d.centerZ,
                  0, 1, 0);

        glRotatef(Settings3d.RotX, 1, 0, 0);
        glRotatef(Settings3d.RotY, 0, 1, 0);
        glRotatef(Settings3d.RotZ, 0, 0, 1);
    }
    else
    {
//        glTranslatef(Settings3d.TrX, Settings3d.TrY, 0);
//        float ks = fabs(1.0/(Settings3d.TrZ+2.5));
//        glScalef(ks, ks, 1.0);

//        glTranslatef(Settings3d.eyeX, Settings3d.eyeY, 0);
        glTranslatef(Settings3d.centerX, Settings3d.centerY, 0);

//        gluLookAt(Settings3d.eyeX, Settings3d.eyeY, Settings3d.eyeZ,
//                  Settings3d.centerX, Settings3d.centerY, Settings3d.centerZ,
//                  0, 1, 0);


        float ks = fabs(1.0/(Settings3d.eyeZ-2));
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

    if (_whatShow.ShowAreaForRandomNodes)
    {
        glLineWidth(0.5f);
        glColor3f(0.9f, 0.1f, 0.1f);
        DrawSomeArea(_areaNodeCoords, offsetX, offsetY, offsetZ, k, Settings3d.IsPerspective);
    }

    if (_whatShow.ShowAreaForGradDesc)
    {
        glLineWidth(0.25f);
        glColor3f(0.1f, 0.1f, 0.9f);
        DrawSomeArea(_areaGradDesc, offsetX, offsetY, offsetZ, k, Settings3d.IsPerspective);
    }

    if (_whatShow.ShowRoutes)
    {
        for (const auto & r : Routes)
        {
            glColor3f(0.1f, 0.7f, 0.7f);

            glBegin(GL_LINE_STRIP);
            for (const auto & p : r.Points)
            {
                double x = (p.Pos.x()-offsetX)*k;
                double y = (p.Pos.y()-offsetY)*k;
                double z = (p.Pos.z()-offsetZ)*Relief->GetGlobal_kz();

                glVertex3f(x, y, zOffset + (Settings3d.IsPerspective ? z : 0));
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
                double z = (p.Pos.z()-offsetZ)*Relief->GetGlobal_kz();
                glTranslatef(x, y, zOffset + (Settings3d.IsPerspective ? z : 0));

                gluQuadricDrawStyle(Quadric(), GLU_FILL);
                gluSphere(Quadric(), (0.01 + 0.025*sqrt(p.Weight)) * pow(fabs(Settings3d.eyeZ), 0.25), 12, 12);
                glPopMatrix();
            }

            if (/*false && */ isDrawAbonents)
            {
                const auto & abo = r.GetAbonent();
                double x = (abo.Pos.x()-offsetX)*k;
                double y = (abo.Pos.y()-offsetY)*k;
                double z = (abo.Pos.z()-offsetZ)*Relief->GetGlobal_kz();

                constexpr float kTri = 0.02f;

                glPushMatrix();

                const auto & q = abo.q;

                double tetha = acos(q.z() / q.length()) * 180.0 / M_PI;
                double fi;
                if (qFuzzyCompare(q.toVector2D().length(), 0.0))
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
    }

    glPointSize(3.0f);
    glLineWidth(1.0f);
    for (size_t i = 0; i < Nodes.size(); ++i)
    {
        if ((int)i == iCurNode)
            Nodes[i].DrawIn3D(_snt, Relief, Settings3d, SignalNodeStatus::Selected, _whatShow);
        else
            Nodes[i].DrawIn3D(_snt, Relief, Settings3d, SignalNodeStatus::NotSelected, _whatShow);
    }

    if (_snt == SignalNodeType::Cone)
        DrawIntersectsWithEllipses(Settings3d, _whatShow);


    if (Settings3d.IsPerspective)
    {
        if (IsIsolinesARFBuilt)
        {
//            glDepthFunc(GL_ALWAYS);
            glCallList(IsolinesARFCompileList);
//            glDepthFunc(GL_LESS);
        }
    }
    else
    {
        if (IsIsolinesARF2dBuilt)
        {
            glCallList(IsolinesARF2dCompileList);
        }
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

bool MyConfig::StartGradDescent(int nDraw,
                                const tf_gd_lib::GradDescent &_protoGD,
//                                const TargetFuncSettingsStruct &_targetFuncSettings,
                                /*const*/ TargetFunctionBase &_targetFunction,
                                SignalNodeType _snt,
                                const QRectF &_area,
                                IGradDrawable *pGLWidget  // = nullptr
                                )
{
    cout << endl << "Grad Descent (First Phase) Started" << endl;

    for (auto & node : Nodes)
    {
        node.Bonds.clear();
    }
    Stats.Reset();

    GradDesc = _protoGD;

    _targetFunction.Init(this);

    auto lambdaTargetFunc = [&_targetFunction](const std::vector<long double>&params)
    {
        return _targetFunction(params);
    };

    // why doesn't std::function work???

//    std::function<double(const std::vector<double>&)> f_TargetFunc = _targetFunction;
//    GradDesc.SetUseUserTargetFunction(f_TargetFunc);
//    GradDesc.SetUseUserTargetFunction(std::function<double(const std::vector<double>&)>(_targetFunction(const std::vector<double>&)));
    GradDesc.SetUseUserTargetFunction(lambdaTargetFunc);

    bool GradReport = false;
    std::ofstream f_out;

    if (GradReport)
    {
        f_out.open("GradReport.txt");
        f_out.precision(16);
    }

    if (nDraw > 0)
    {
        GradDesc.SetCallback([this, pGLWidget, _snt, GradReport, &f_out]()
        {
            if (!IsGradCalculating)
                GradDesc.Stop();

            if (GradReport)
            {
                for (const auto & item: GradDesc.GetParams())
                {
                    f_out << item << "\t";
                }
                f_out << endl;

                for (const auto & item: GradDesc.Get_dCost_dp())
                {
                    f_out << item << "\t";
                }
                f_out << endl;

                for (const auto & item: GradDesc.GetCur_Eta())
                {
                    f_out << item << "\t";
                }
                f_out << endl;

                f_out << endl;
            }

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

    InitParamsFromNodeCoords(_targetFunction.Get_param_count(), _snt, _area);

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

    FindCoveredPointsUsingParams(GradDesc.GetParams(), _snt); // Добавил, раньше не было

    InitNodeCoordsFromParams(GradDesc.GetParams(), _snt);
    CalcPointStats();
    CalcBonds(_targetFunction, _snt);

    Stats.LastCostPhase1 = GradDesc.GetLastCost();
    cout << "gd.GetLastCost() = " << GradDesc.GetLastCost() << endl;

    cout << "Iters: " << GradDesc.GetLastIters() << " out of " << GradDesc.GetMaxIters() << endl;
    cout << "Time: " << GradDesc.GetLastTime() << " out of " << GradDesc.GetMaxTime() << endl;

    cout << "alpha = " << GradDesc.GetParams()[2] << endl;

    cout << "Grad Descent Finished (First Phase)" << endl << endl;

//    PrintBondsInfo();

    return true;
}
//----------------------------------------------------------

bool MyConfig::StartFinalGradDescent(int nDraw, const tf_gd_lib::GradDescent &_protoGD,
//                                     const TargetFuncSettingsStruct &_targetFuncSettings,
                                     /*const*/ TargetFunctionBase &_targetFunction,
                                     SignalNodeType _snt,
                                     const QRectF &_area,
                                     IGradDrawable *pGLWidget // = nullptr
                                     )
{
    cout << endl << "Final Grad Descent Started" << endl;

    GradDesc = _protoGD;

    _targetFunction.Init(this);

    auto lambdaTargetFunc = [&_targetFunction](const std::vector<long double>&params)
    {
        return _targetFunction(params);
    };

    // why doesn't std::function work without lambda???

//    std::function<double(const std::vector<double>&)> f_TargetFunc = _targetFunction;
//    GradDesc.SetUseUserTargetFunction(f_TargetFunc);
//    GradDesc.SetUseUserTargetFunction(std::function<double(const std::vector<double>&)>(_targetFunction(const std::vector<double>&)));
    GradDesc.SetUseUserTargetFunction(lambdaTargetFunc);

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

    InitParamsFromNodeCoords(_targetFunction.Get_param_count(), _snt, _area);

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


    FindCoveredPointsUsingParams(GradDesc.GetParams(), _snt);

//    int iRoute = 0;
//    for (/*const*/ auto & route : Routes) // сделать это всё и для Cone !!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
//    {
//        int iPoint = 0;
//        for (/*const*/ auto & p1 : route.Points)
//        {
//            p1.IsCovered = false;
//            for (size_t k = 0; k < Nodes.size(); ++k)
//            {
//                if (p1.Pos.distanceToPoint(Nodes[k].Pos) < Nodes[k].R) // А для Cone ??????!!!!!!
//                {
//                    p1.IsCovered = true;
//                    break;
//                }
//            }
//            iPoint++;
//        }
//        iRoute++;
//    }

    InitNodeCoordsFromParams(GradDesc.GetParams(), _snt);
    CalcPointStats();

    Stats.LastCostPhase2 = GradDesc.GetLastCost();
    cout << "gd.GetLastCost() = " << GradDesc.GetLastCost() << endl;

    cout << "Iters: " << GradDesc.GetLastIters() << " out of " << GradDesc.GetMaxIters() << endl;
    cout << "Time: " << GradDesc.GetLastTime() << " out of " << GradDesc.GetMaxTime() << endl;

    cout << "Final Grad Descent Finished" << endl << endl;

    return true;
}
//----------------------------------------------------------

void MyConfig::InitNodeCoordsFromParams(const std::vector<long double> & _params, SignalNodeType _snt)
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
            if (std::isnan(_params[i]))
                qDebug() << "isnan(_params[i])";

            if (std::isnan(_params[i+1]))
                qDebug() << "isnan(_params[i+1])";

            if (std::isnan(_params[i+2]))
                qDebug() << "isnan(_params[i+2])";


            Nodes.at(i/3).Pos.setX(_params[i]);
            Nodes.at(i/3).Pos.setY(_params[i+1]);
            Nodes.at(i/3).Pos.setZ( Relief->CalcRealZbyRealXY(_params[i], _params[i+1]) );

            Nodes.at(i/3).Alpha = _params[i+2] / WierdCoeffAlpha;
        }
    }
    else
        throw std::runtime_error("Unknown _snt in MyConfig::InitNodeCoordsFromParams");
}
//----------------------------------------------------------

void MyConfig::InitParamsFromNodeCoords(const int _param_count, SignalNodeType _snt, const QRectF &_area)
{
    if (!(_snt == SignalNodeType::Sphere || _snt == SignalNodeType::Cone))
        throw std::runtime_error("Unknown _snt in MyConfig::InitParamsFromNodeCoords");

    vector<long double> params(_param_count);
    vector<long double> min_constrains(_param_count);
    vector<long double> max_constrains(_param_count);
    vector<long double> rel_constrains(_param_count); // relative constrains in %
    vector<bool> type_constrains(_param_count);  // set 'false' to use absolute constrains, 'true' - for relative

//    const auto & area = Relief->GetArea();

    double min_x = _area.left(); // для жесткий ограничений без поиска
    double max_x = _area.right();
    double min_y = _area.top();
    double max_y = _area.bottom();


    size_t i = 0;
    for (const auto & node : Nodes)
    {
        params[i] = node.Pos.x();

        min_constrains[i] = min_x;
        max_constrains[i] = max_x;

        rel_constrains[i] = 50; // not used yet
        type_constrains[i] = false; // use absolute constrains
        i++;

        params[i] = node.Pos.y();

        min_constrains[i] = min_y;
        max_constrains[i] = max_y;

        rel_constrains[i] = 50; // not used yet
        type_constrains[i] = false; // use absolute constrains
        i++;

        if (_snt == SignalNodeType::Cone)
        {
            params[i] = node.Alpha * WierdCoeffAlpha;

//            min_constrains[i] = -2*M_PI  * WierdCoeffAlpha; // ???
//            max_constrains[i] = +2*M_PI  * WierdCoeffAlpha; // ???

//            min_constrains[i] = -M_PI  * WierdCoeffAlpha; // ???
//            max_constrains[i] = +M_PI  * WierdCoeffAlpha; // ???

            min_constrains[i] = -1e20; // ???
            max_constrains[i] = +1e20; // ???

//            min_constrains[i] = -2; // ???
//            max_constrains[i] = +2; // ???

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

void MyConfig::FindCoveredPointsUsingParams(const std::vector<long double> &params, SignalNodeType _snt)
{
    int iRoute = 0;
    for (/*const*/ auto & route : Routes)
    {
        int iPoint = 0;
        for (/*const*/ auto & p1 : route.Points)
        {
            p1.IsCovered = false;
            size_t dk = _snt == SignalNodeType::Sphere ? 2 : 3;
            for (size_t k = 0; k < Nodes.size()*dk; k += dk)
            {
                if (_snt == SignalNodeType::Sphere)
                {
                    SignalNode sn(MyVector3D<>(params[k],
                                            params[k+1],
                                            Relief->CalcRealZbyRealXY(params[k], params[k+1])),
                                            Nodes[k/dk].R);


                    if (p1.Pos.distanceToPoint(sn.Pos) < sn.R) // Сравниваем расстояния в 3d! Придумать что-то еще?
                    {
                        p1.IsCovered = true;
                        break;
                    }
                }
                else if (_snt == SignalNodeType::Cone)
                {
                    SignalNode sn(MyVector3D<>(params[k],
                                            params[k+1],
                                            Relief->CalcRealZbyRealXY(params[k], params[k+1])),
                                            Nodes[k/dk].R,
                                            params[k+2], // Nodes[k/dk].Alpha,
                                            Nodes[k/dk].Beta);

//                    qDebug() << "FindCovered: sn.accessRateCone(p1.Pos) = " << sn.accessRateCone(p1.Pos);

//                    qDebug() << p1.Pos;

                    if (sn.accessRateCone(p1.Pos) > arfThreshold)
                    {
                        p1.IsCovered = true;
                        break;
                    }
                }

            }
            iPoint++;
        }
        iRoute++;
    }
}
//----------------------------------------------------------

void MyConfig::CalcAccessRateForCurrent()
{
    qDebug() << (double)Nodes.front().accessRateCone(Routes.front().Points.front().Pos);
}
//----------------------------------------------------------

void MyConfig::SelectCurNodeByRealXY(double x, double y)
{
    double minDist = std::numeric_limits<double>::max();
    for (size_t i = 0; i < Nodes.size(); ++i)
    {
        double dist = QLineF(Nodes[i].Pos.toPointF(), {x, y}).length();

//        qDebug() << "i =" << i << "   dist =" << dist << "    x =" << x << "    y = " << y;

        if (dist < minDist)
        {
            minDist = dist;
            iCurNode = i;
        }
    }
}
//----------------------------------------------------------

void MyConfig::PutCurNodeByRealXYZ(double x, double y, double z)
{
    if (iCurNode < 0)
    {
        qDebug() << "iCurNode < 0 in MyConfig::PutCurNodeByRealXYZ";
        return;
    }

    Nodes.at(iCurNode).Pos = {(float)x, (float)y, (float)z};
}
//----------------------------------------------------------

void MyConfig::SetDirectCurNodeByRealXYZ(double x, double y, double z)
{
    (void)z;

    if (iCurNode < 0)
    {
        qDebug() << "iCurNode < 0 in MyConfig::PutCurNodeByRealXYZ";
        return;
    }

    double xLoc = x - Nodes.at(iCurNode).Pos.x();
    double yLoc = y - Nodes.at(iCurNode).Pos.y();

    double alpha = 0;

    if (xLoc > 0)
    {
        alpha = atan(yLoc / xLoc);
    }
    else if (xLoc < 0)
    {
        if (yLoc >= 0)
            alpha = atan(yLoc / xLoc) + M_PI;
        else
            alpha = atan(yLoc / xLoc) - M_PI;
    }
    else // xLoc == 0
    {
        if (yLoc > 0)
            alpha = M_PI / 2.0;
        else if (yLoc < 0)
            alpha = -M_PI / 2.0;
        else
        {
            alpha = Nodes.at(iCurNode).Alpha;   // xLoc = 0 and yLoc = 0
        }
    }

    Nodes.at(iCurNode).Alpha = alpha;
}
//----------------------------------------------------------

const SignalNode & MyConfig::GetCurNode() const
{
    if (iCurNode < 0 || iCurNode >= (int)Nodes.size())
        throw std::runtime_error("iCurNode < 0 || iCurNode >= (int)Nodes.size() in MyConfig::GetCurNode()");

    return Nodes.at(iCurNode);
}
//----------------------------------------------------------

void MyConfig::SetNode(int ind, const SignalNode &_node)
{
    Nodes.at(ind) = _node;
}
//----------------------------------------------------------

void MyConfig::DrawIntersectsWithEllipses(const Settings3dType &_settings3d,
                                          const WhatShowStruct &_whatShow) const
{
    constexpr float zOffset = 0.01f;
    const auto & area = Relief->GetArea();

    double kx = 2.0/area.width();
    double ky = 2.0/area.height();
    double k = min(kx, ky);
//    double k = max(kx, ky);

    double hW = area.width()/2.0;
    double hH = area.height()/2.0;

    double offsetX = area.x()+hW; // in meters
    double offsetY = area.y()+hH; // in meters
    double offsetZ = 0.0001;

    glLineWidth(1.0f);
    gluQuadricDrawStyle(Quadric(), GLU_FILL);

    for (const auto &node : Nodes)
    {
        double xNode = (node.Pos.x()-offsetX)*k;
        double yNode = (node.Pos.y()-offsetY)*k;
        double zNode;

        for (const auto & r : Routes)
        {
            for (const auto & p : r.Points)
            {
                glColor3f(0.5f, 0.3f, 0.9f);

                if (_whatShow.ShowLinesBetweenSNandPoints)
                {
                    glBegin(GL_LINES);

                    zNode = (node.Pos.z()-offsetZ)*Relief->GetGlobal_kz();
                    glVertex3f(xNode, yNode, zOffset + (_settings3d.IsPerspective ? zNode : 0));

                    double xPoint = (p.Pos.x()-offsetX)*k;
                    double yPoint = (p.Pos.y()-offsetY)*k;
                    double zPoint;

                    zPoint = (p.Pos.z()-offsetZ)*Relief->GetGlobal_kz();
                    glVertex3f(xPoint, yPoint, zOffset + (Settings3d.IsPerspective ? zPoint : 0));

                    glEnd(); // LINES
                }

                if (_whatShow.ShowPointsOnRadii)
                {
                    QPointF Result;
                    bool isOk = node.CalcIntersectWithLineToPoint(p.Pos, Result);
                    if (!isOk)
                        throw std::logic_error("There is not any instersection with ellipse in MyConfig::DrawIntersectsWithEllipses");

                    glPushMatrix();

                    double x = (Result.x()-offsetX)*k;
                    double y = (Result.y()-offsetY)*k;
                    double z = zOffset + (_settings3d.IsPerspective ? Relief->CalcNormToRealZbyRealXY(Result.x(), Result.y()) : 0);

                    glTranslatef(x, y, zOffset + (_settings3d.IsPerspective ? z : 0));

                    gluSphere(Quadric(), 0.015, 6, 6);
                    glPopMatrix();
                }
            }
        }

    }
}
//----------------------------------------------------------

void MyConfig::SetAbonentsPosForRoutes(const std::vector<Route> &_routes)
{
    assert(_routes.size() == Routes.size());

    for (size_t i = 0; i < _routes.size(); ++i)
    {
        Routes.at(i).AbonentDirectAccess().Pos = _routes.at(i).GetAbonent().Pos;
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

void MyConfig::CalcBonds(const TargetFunctionBase &_targetFuncSettingsBase, SignalNodeType _snt)
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

                        if ( _targetFuncSettingsBase.GetIsUseLineBetweenTwoPoints() )
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
                    distToPoint = Routes[iRoute].Points[iPoint].Pos.distanceToPoint(Nodes[iNode].Pos);
//                    distToPoint = для Cone (учесть, что центр в другом месте)

                    double arf = Nodes[iNode].accessRateCone(Routes[iRoute].Points[iPoint].Pos);

                    if ( _targetFuncSettingsBase.GetIsUseLineBetweenTwoPoints() )
                    {
                        // Что делать с этим?
                        arf *= IsLineBetweenTwoPoints(Nodes[iNode].Pos, Routes[iRoute].Points[iPoint].Pos);

//                      if ( IsLineBetweenTwoPoints(Nodes[iNode].Pos, Routes[iRoute].Points[iPoint].Pos) )
//                          arf = Nodes[iNode].accessRateF(Routes[iRoute].Points[iPoint].Pos);
//                      else
//                          arf = 0;
                    }

                    if (arf > arfThreshold)
                    {
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

double MyConfig::IsLineBetweenTwoPoints(const MyPos3d<> &_p1, const MyPos3d<> &_p2) const
{
    double result = 1.0;

    long double dRay = Relief->GetDeltaDiag();
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

    MyVector2D d = dRay * (_p2.toVector2D() - _p1.toVector2D()).normalized();
//    Pos3d p;
    MyVector2D p;
    MyVector2D p0 = _p1.toVector2D();

    double lenZ = _p2.z() - _p1.z();
    double dz = lenZ / (n-1);
    double LineZ;

    for (int i = 0; i < n; ++i)
    {
        p = p0 + (MyDoubleType)i*d;
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
            cout << "{"  << b.iRoute << "; " << b.iPoint <<
                    "; " << b.arf << "; " << b.relDist << "}" << endl;
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

double MyConfig::CalcAccessRateForAnyPos(const MyPos3d<> &_pos, bool _isUseLineBetweenTwoPoints,
                                     SignalNodeType _snt,
                                     TargetFuncTypeEnum funcType)
{
    double y1 = 0;

    for (const auto & sn : Nodes)
    {
        double y;// = sn.accessRateSphere(abo.Pos);
        if (_snt == SignalNodeType::Sphere)
            y = sn.accessRateSphere(_pos);
        else if (_snt == SignalNodeType::Cone)
            y = sn.accessRateCone(_pos);
        else
            throw std::runtime_error("Unknown _snt in MyConfig::CalcAccessRateForAbos");

        if (_isUseLineBetweenTwoPoints)
        {
            y *= IsLineBetweenTwoPoints(sn.Pos, _pos); // осторожно для Cone!
        }


        if (funcType == TargetFuncTypeEnum::Additive)
        {
            y1 += y;
        }
        else if (funcType == TargetFuncTypeEnum::Probabilistic)
        {
            if (y1 == 0)  // Для первой итерации - для первого сигнального узла
                y1 = y;
            else
                y1 = y1 + y - y1*y;
        }
        else
        {
            throw std::runtime_error("funcType is TargetFuncTypeEnum::Unknown in MyConfig::CalcAccessRateForAbos");
        }
    }

    return y1;
}
//----------------------------------------------------------

void MyConfig::CalcAccessRateForAbos(bool _isUseLineBetweenTwoPoints,
                                     SignalNodeType _snt,
                                     TargetFuncTypeEnum funcType)
{
    for (auto & route : Routes)
    {
        auto & abo = route.AbonentDirectAccess();
        double y1 = 0;

        y1 = CalcAccessRateForAnyPos(abo.Pos, _isUseLineBetweenTwoPoints, _snt, funcType);

//        for (const auto & sn : Nodes)
//        {
//            double y;// = sn.accessRateSphere(abo.Pos);
//            if (_snt == SignalNodeType::Sphere)
//                y = sn.accessRateSphere(abo.Pos);
//            else if (_snt == SignalNodeType::Cone)
//                y = sn.accessRateCone(abo.Pos);
//            else
//                throw std::runtime_error("Unknown _snt in MyConfig::CalcAccessRateForAbos");

//            if (_isUseLineBetweenTwoPoints)
//            {
//                y *= IsLineBetweenTwoPoints(sn.Pos, abo.Pos); // осторожно для Cone!
//            }


//            if (funcType == TargetFuncTypeEnum::Additive)
//            {
//                y1 += y;
//            }
//            else if (funcType == TargetFuncTypeEnum::Probabilistic)
//            {
//                if (y1 == 0)  // Для первой итерации - для первого сигнального узла
//                    y1 = y;
//                else
//                    y1 = y1 + y - y1*y;
//            }
//            else
//            {
//                throw std::runtime_error("funcType is TargetFuncTypeEnum::Unknown in MyConfig::CalcAccessRateForAbos");
//            }
//        }

        abo.accessRate = y1;
    }



    // Ниже в комментариях - просто образец пробабилистика
//    for (auto & route : Routes) // все точки всех маршрутов
//    {
//        for (auto & p1 : route.Points) // цикл по точкаи одного маршрута
//        {
//            double s = 0;
//            for (size_t k = 0; k < param_count; k += dk)
//            {
//                SignalNode sn(QVector3D(params[k],
//                                            params[k+1],
//                                            Relief->CalcRealZbyRealXY(params[k], params[k+1])  ),
//                                  Nodes[k/dk].R);
//                double y;
//                y = sn.accessRateSphere(p1.Pos);

//                if (IsUseLineBetweenTwoPoints)
//                {
//                    y *= myConfig->IsLineBetweenTwoPoints(sn.Pos, p1.Pos);
//                }

//                if (k == 0)
//                    s = y;
//                else
//                {
//                    s = s + y - s*y;
//                }

//            }

//            y1 += s;
//        }
//    }

}
//----------------------------------------------------------


