#include "Relief.h"
#include <ctime>
#include "MathUtils.h"
#include <cmath>
#include <QDebug>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

#include <vector>

using namespace std;

const QString ReliefsDefaultDir = "Reliefs";
const QString ReliefsImagesDefaultDir = "Reliefs/SrcImages";
const QString ReliefsExtension = "*.relief";
const QString ReliefsImagesExtension = "*.png *.jpg *.bmp";
const QString ReliefsLegendsDefaultDir = "Reliefs/Legends";
const QString ReliefsLegendsExtension = "*.rlgd";


//----------------------------------------------------------
//----------------------------------------------------------

Relief3D::~Relief3D()
{
    Clear();
}
//----------------------------------------------------------

double Relief3D::CalcRealXYZbyNormXY(double normX, double normY, double &_realX, double &_realY) const
{
    if (std::isnan(normY))
    {   // Временная ловушка - удалить потом !!!!!!!!!!!!!
        throw std::logic_error("normY == nan in CalcRealXYZbyNormXY");
    }

    _realX = (normX-xStartInside)/wInside*Area.width() + Area.left();
    _realY = (normY-yStartInside)/hInside*Area.height() + Area.top();
    if (std::isnan(_realY))
    {   // Временная ловушка - удалить потом !!!!!!!!!!!!!
        throw std::logic_error("_realY == nan in CalcRealXYZbyNormXY");
    }
    return CalcRealZbyRealXY(_realX, _realY);
}
//----------------------------------------------------------

double Relief3D::LinearInterpol(double x, double x1, double x0, double f1, double f0)
{
    if (fabs(x1-x0) < 1e-8) // ???
    {
        //qDebug() << "(fabs(x1-x0) < 1e-8)";
        return f0;
    }

    return f0 + (x - x0) * (f1 - f0) / (x1 - x0);
}
//----------------------------------------------------------

double Relief3D::LinearInterpol(std::map<int, int> const & _row, double x) const
{
    auto it_l = _row.lower_bound(x);
    auto it_u = _row.upper_bound(x);

    if (it_l != it_u && it_l != _row.end()) // попали на точное значение
    {
        return it_l->second;
    }

    if (it_l == _row.end()) // вышли за границу справа, берем две крайние точки
    {
        //qDebug() << "x = " << x;

        it_l = std::prev(_row.end());
        it_l = std::prev(it_l);
        it_u = std::prev(_row.end());
    }
    else if (it_l == it_u) // где-то между точками или слева от точек
    {
        if (x < MinX) // вышли за границу слева
        {
            it_u = std::next(it_u);
        }
        else // где-то между точками
        {
            it_l = std::prev(it_l);
        }
    }

    if (it_u == _row.end() || it_l == _row.end())
    {
        throw std::logic_error("It's not supposed to be in Relief3D::LinearInterpol");
//        it_l = std::prev(it_l);
//        it_u = std::prev(_row.end());
//        qDebug() << "!!!! right position in ReliefMap for x !!!!";
    }


    return LinearInterpol(x, it_u->first, it_l->first, it_u->second, it_l->second);
}
//----------------------------------------------------------

double Relief3D::CalcRealZbyRealXY(double x, double y) const
{
    if (ReliefMap.empty())
        return 0;

    auto it_l = ReliefMap.lower_bound(y);
    auto it_u = ReliefMap.upper_bound(y);

    if (it_l != it_u && it_l != ReliefMap.end()) // попали на точное значение
    {
        return LinearInterpol(it_l->second, x);
    }

    if (it_l == ReliefMap.end()) // вышли за границу справа, берем две крайние точки
    {
        //qDebug() << "y = " << y;

        it_l = std::prev(ReliefMap.end());
        it_l = std::prev(it_l);
        it_u = std::prev(ReliefMap.end());
    }
    else if (it_l == it_u) // где-то между точками или слева от точек
    {
        if (y < MinY) // вышли за границу слева
        {
            it_u = std::next(it_u);
        }
        else // где-то между точками
        {
            it_l = std::prev(it_l);
        }
    }

    if (it_u == ReliefMap.end() || it_l == ReliefMap.end())
    {       
        if (std::isnan(y))
        {
            throw std::logic_error("y == nan in Relief3D::CalcRealZbyRealXY");
        }

        // Случилось пару раз, когда y был nan
        throw std::logic_error("It's not supposed to be in Relief3D::CalcRealZbyRealXY - x = " + to_string(x) + " , y = " + to_string(y) );
//        it_l = std::prev(it_l);
//        it_u = std::prev(ReliefMap.end());
//        qDebug() << "!!!! right position in ReliefMap for y";
    }

    return LinearInterpol(y, it_u->first, it_l->first, LinearInterpol(it_u->second, x), LinearInterpol(it_l->second, x));
}
//----------------------------------------------------------

void Relief3D::CalcAndReWriteRealZforPos3d(Pos3d &_pos3d) const
{
    if (std::isnan(_pos3d.y())) // Временная ловушка - потом удалить!!!
    {
        throw std::logic_error("y == nan in Relief3D::CalcAndReWriteRealZforPos3d");
    }
    _pos3d.setZ( CalcRealZbyRealXY(_pos3d.x(), _pos3d.y()) );
}
//----------------------------------------------------------

double Relief3D::CalcNormZbyRealXY(double x, double y) const
{
    if (std::isnan(y)) // Временная ловушка - потом удалить!!!
    {
        throw std::logic_error("y == nan in CalcNormZbyRealXY");
    }
    return CalcRealZbyRealXY(x, y) / maxZ;
}
//----------------------------------------------------------

double Relief3D::CalcNormToRealZbyRealXY(double x, double y) const
{
    if (std::isnan(y)) // Временная ловушка - потом удалить!!!
    {
        throw std::logic_error("y == nan in CalcNormToRealZbyRealXY");
    }
    return CalcRealZbyRealXY(x, y) * Global_kz;
}
//----------------------------------------------------------

QColor Relief3D::CalcColorByZ(double z) const
{
    constexpr double startHue = 160;
    int h = startHue - (z-minZ)/(maxZ-minZ)*startHue;
    QColor res = QColor::fromHsv(h, 200, 220);
    res = res.toRgb();
    return res;
}
//----------------------------------------------------------

void Relief3D::ReCreateListsGL()
{
    if (ReliefCompileList)
    {
        glDeleteLists(ReliefCompileList, 1);
        ReliefCompileList = 0;
    }
    if (Relief2dCompileList)
    {
        glDeleteLists(Relief2dCompileList, 1);
        Relief2dCompileList = 0;
    }

    ReliefCompileList = glGenLists(1);
    Relief2dCompileList = glGenLists(1);
}
//----------------------------------------------------------

void Relief3D::BuildReliefToGL(bool _is2d)
{
    if (!_is2d && IsReliefBuilt)
        throw std::runtime_error("IsReliefBuilt == true in Relief3D::BuildReliefToGL");
    if (_is2d && IsRelief2dBuilt)
        throw std::runtime_error("IsRelief2dBuilt == true in Relief3D::BuildReliefToGL");


    int RowCount = ReliefMap.size();
    int ColCount = ReliefMap.begin()->second.size();

    const double aspect = Area.width()/Area.height();

    if (aspect > 1)
    {
        wInside = 2.0;
        hInside = wInside/aspect;
        xStartInside = -1.0;
        yStartInside = -1.0 + (2.0 - hInside)/2.0;
    }
    else
    {
        hInside = 2.0;
        wInside = hInside*aspect;
        yStartInside = -1.0;
        xStartInside = -1.0 + (2.0 - wInside)/2.0;
    }

    const double dx = wInside / (ColCount-1);
    const double dy = hInside / (RowCount-1);

    std::vector<QVector3D> oneRow(ColCount);
    std::vector<std::vector<QVector3D>> points(RowCount, oneRow);

    std::vector<QColor> oneRowColor(ColCount);
    std::vector<std::vector<QColor>> colors(RowCount, oneRowColor);

    if (!_is2d)
        glNewList(ReliefCompileList, GL_COMPILE);
    else
        glNewList(Relief2dCompileList, GL_COMPILE);

    maxZ = 1;
    minZ = numeric_limits<double>::max();
    double mz = -1;
    for (int i = 0; i < RowCount; i++)
    {
        for (int j = 0; j < ColCount; j++)
        {
            points[i][j] = QVector3D(xStartInside + j * dx, yStartInside + i * dy, 0);

            if (points[i][j].x() < -1)
                qDebug() << "points[i][j].x() < -1";

            if (points[i][j].x() > 1)
                qDebug() << "points[i][j].x() > 1";

            if (points[i][j].y() < -1)
                qDebug() << "points[i][j].y() < -1";

            if (points[i][j].y() > 1)
                qDebug() << "points[i][j].y() > 1";

            points[i][j].setZ(CalcRealZbyRealXY((points[i][j].x()-xStartInside)/wInside*Area.width()+Area.left(),
                                                (points[i][j].y()-yStartInside)/hInside*Area.height()+Area.top()));

            if (points[i][j].z() > mz)
                mz = points[i][j].z();

            if (points[i][j].z() < minZ)
                minZ = points[i][j].z();

        }
    }

    maxZ = mz; // meters

    for (int i = 0; i < RowCount; i++)
        for (int j = 0; j < ColCount; j++)
            colors[i][j] = CalcColorByZ(points[i][j].z());

    Global_kz = 2.0/max(Area.width(), Area.height());

    AverZ = 0;
    for (int i = 0; i < RowCount; i++)
    {
        for (int j = 0; j < ColCount; j++)
        {
            double z = points[i][j].z()*Global_kz;
            AverZ += z;
            points[i][j].setZ(z);
        }
    }

    AverZ /= (RowCount*ColCount);

//    Global_kz = 1/maxZ;

    glBegin(GL_QUADS);
    for (int i = 0; i < RowCount-1; i++)
    {
        for (int j = 0; j < ColCount-1; j++)
        {
            QVector3D
                p0 = points[i][j],
                p1 = points[i][j+1],
                p2 = points[i+1][j+1],
                p3 = points[i+1][j];

            QColor
                c0 = colors[i][j],
                c1 = colors[i][j+1],
                c2 = colors[i+1][j+1],
                c3 = colors[i+1][j];

            glColor3f(c0.redF(), c0.greenF(), c0.blueF());
            glVertex3f(p0.x(), p0.y(), _is2d ? 0 : p0.z());

            //qDebug() << "p0.z() = " << p0.z();

            glColor3f(c1.redF(), c1.greenF(), c1.blueF());
            glVertex3f(p1.x(), p1.y(), _is2d ? 0 : p1.z());

            glColor3f(c2.redF(), c2.greenF(), c2.blueF());
            glVertex3f(p2.x(), p2.y(), _is2d ? 0 : p2.z());

            glColor3f(c3.redF(), c3.greenF(), c3.blueF());
            glVertex3f(p3.x(), p3.y(), _is2d ? 0 : p3.z());
        }
    }
    glEnd();

    glEndList(); // закончить список

    if (!_is2d)
        IsReliefBuilt = true;
    else
        IsRelief2dBuilt = true;
}
//----------------------------------------------------------

void Relief3D::Draw(bool _is2d)
{
    if (!_is2d && !IsReliefBuilt)
        throw runtime_error("IsReliefBuilt == false in Relief3D::Draw");
    if (_is2d && !IsRelief2dBuilt)
        throw runtime_error("IsRelief2dBuilt == false in Relief3D::Draw");

    if (!_is2d)
        glCallList(ReliefCompileList);
    else
        glCallList(Relief2dCompileList);
}
//----------------------------------------------------------

void Relief3D::AddRow(int y, const std::vector<std::pair<int, int>> &_row)
{
    std::map<int, int> row;
    row.insert(_row.cbegin(), _row.cend());

    ReliefMap.insert({y, row});
}
//----------------------------------------------------------

//void Relief3D::AddPoint(double x, double y, int z)
//{
//    ReliefMap.try_emplace()
//}
//----------------------------------------------------------

void Relief3D::SaveToFile(const QString &_fileName)
{
    QJsonDocument jsonDoc;
    QJsonObject resultObject;

    resultObject.insert("ImageFileName", ImageFileName);
    resultObject.insert("LegendFileName", LegendFileName);

    QJsonObject areaObject;
    areaObject.insert("left", Area.left());
    areaObject.insert("right", Area.right());
    areaObject.insert("top", Area.top());
    areaObject.insert("bottom", Area.bottom());

    resultObject.insert("Area", areaObject);

    QJsonArray rowsArray;
    for (const auto & row : ReliefMap)
    {
        QJsonObject rowObject;
        rowObject.insert("y", row.first);
        QJsonArray rowArray;

        for (const auto & p : row.second)
        {
            QJsonObject pObject;
            pObject.insert("x", p.first);
            pObject.insert("z", p.second);
            rowArray.append(pObject);
        }
        rowObject.insert("row", rowArray);

        rowsArray.append(rowObject);
    }

    resultObject.insert("Rows", rowsArray);

    jsonDoc.setObject(resultObject);
    QFile jsonFile(_fileName);
    jsonFile.open(QFile::WriteOnly);
    jsonFile.write(jsonDoc.toJson());
}
//----------------------------------------------------------

void Relief3D::Clear()
{
    Area.setCoords(0, 0, 0, 0);
    FileName.clear();

    if (ReliefCompileList)
        glDeleteLists(ReliefCompileList, 1);
    if (Relief2dCompileList)
        glDeleteLists(Relief2dCompileList, 1);

    ReliefCompileList = 0;
    IsReliefBuilt = false;
    Relief2dCompileList = 0;
    IsRelief2dBuilt = false;

    MinX = std::numeric_limits<double>::max();
    MinY = std::numeric_limits<double>::max();

    minZ = std::numeric_limits<double>::max();
    maxZ = -1;

    ReliefMap.clear();
}
//----------------------------------------------------------

void Relief3D::ParseJson(const QJsonObject &_jsonObject, const QJsonParseError &_parseError)
{
    if ( !_jsonObject["Area"].isObject() )
    {
        qDebug() << _parseError.errorString();
        return;
    }

    ImageFileName  = _jsonObject["ImageFileName"].toString("empty");
    LegendFileName = _jsonObject["LegendFileName"].toString("empty");

    const QJsonObject &areaObject = _jsonObject["Area"].toObject();
    double l = areaObject["left"].toDouble(-1);
    double r = areaObject["right"].toDouble(-1);
    double t = areaObject["top"].toDouble(-1);
    double b = areaObject["bottom"].toDouble(-1);
    SetArea(l, t, r, b);

    if (!Area.isValid())
        throw std::runtime_error("!Area.isValid()");

    const QJsonArray &rowsArray = _jsonObject["Rows"].toArray();
    for (auto itRows = rowsArray.begin(); itRows != rowsArray.end();  ++itRows)
    {
        const QJsonObject &rowObject = itRows->toObject();

        double y = rowObject["y"].toDouble(-1);
        if (y < MinY)
            MinY = y;

        const QJsonArray &rowArray = rowObject["row"].toArray();

        std::vector<std::pair<int, int>> row;

        for (auto itRow = rowArray.begin(); itRow != rowArray.end();  ++itRow)
        {
            const QJsonObject &pObject = itRow->toObject();
            double x = pObject["x"].toDouble(-1);
            if (x < MinX)
                MinX = x;
            double z = pObject["z"].toDouble(-1);
            row.emplace_back(x, z);
        }
        AddRow(y, row);
    }
}
//----------------------------------------------------------

void Relief3D::CalcDeltaDiag()
{
    DeltaDiag = 0.5 * sqrt( pow(Area.width()/ReliefMap.begin()->second.size(), 2) +
                            pow(Area.height()/ReliefMap.size(), 2) );
}
//----------------------------------------------------------

bool Relief3D::LoadFromFile(const QString &_fileName)
{
    Clear();

    QFile json(_fileName);
    if (json.open(QIODevice::ReadOnly))
    {
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(json.readAll(), &parseError);
        if (parseError.error == QJsonParseError::NoError)
        {
            if (jsonDoc.isObject())
            {
                FileName = _fileName;
//                Name = _fileName.mid(0, _fileName.lastIndexOf('.'));
                ParseJson(jsonDoc.object(), parseError);
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


    CalcDeltaDiag();

//    ReCreateListsGL();       // ?? Создается в другом месте - там, где это действительно необходимо
//    BuildReliefToGL(false);  // ??
//    BuildReliefToGL(true);   // ??

    return true;
}
//----------------------------------------------------------


