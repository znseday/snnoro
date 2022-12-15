#ifndef RELIEF_H
#define RELIEF_H

#include <vector>
#include <GL/gl.h>
#include <QColor>

#include "TypesAndUtils.h"
#include <iostream>
#include <map>
#include <QRectF>

class QJsonObject;
class QJsonParseError;

extern const QString ReliefsDefaultDir;
extern const QString ReliefsExtension;
extern const QString ReliefsImagesDefaultDir;
extern const QString ReliefsImagesExtension;
extern const QString ReliefsLegendsDefaultDir;
extern const QString ReliefsLegendsExtension;


class Relief3D
{
protected:

    double LinearInterpol(std::map<int, int> const & _row, double x) const;

    double MinX = std::numeric_limits<double>::max();
    double MinY = std::numeric_limits<double>::max();

    QRectF Area;
    QString FileName;

    GLuint ReliefCompileList = 0;
    bool IsReliefBuilt = false;
    GLuint Relief2dCompileList = 0;
    bool IsRelief2dBuilt = false;

//    bool IsReliefCreated = false; // ?

    double minZ = std::numeric_limits<double>::max();  // in meters
    double maxZ = -1; // in meters

    double DeltaDiag = 0;

    std::map<int, std::map<int, int>> ReliefMap;

    void ParseJson(const QJsonObject &_jsonObject, const QJsonParseError &_parseError);

    double wInside, hInside, xStartInside, yStartInside;

    void CalcDeltaDiag();

    double Global_kz = 1.0;
    double AverZ = 0; // in Gl coords

    QString ImageFileName;
    QString LegendFileName;

public:
    Relief3D() = default;
    ~Relief3D();

    double Get_kz() const {return Global_kz;}
    double GetAverZ() const {return AverZ;}

    static double LinearInterpol(double x, double x1, double x0, double f1, double f0);

    double CalcRealXYZbyNormXY(double normX, double normY, double &_realX, double &_realY) const;

    double CalcRealZbyRealXY(double x, double y) const;
    void CalcAndReWriteRealZforPos3d(Pos3d &_pos3d) const;

    double CalcNormZbyRealXY(double x, double y) const;
    double CalcNormToRealZbyRealXY(double x, double y) const;

//    double CalcRealZbyNormXY(double x, double y) const;
//    double CalcNormZbyNormXY(double x, double y) const;

    QColor CalcColorByZ(double z) const;

    void ReCreateListsGL();
    void BuildReliefToGL(bool _is2d);

    void Draw(bool _is2d);

    const QRectF & GetArea() const {return Area;}
    void SetArea(double l, double b, double r, double t) {Area.setCoords(l, b, r, t);}

    //void AddPoint(double x, double y, int z);
    void AddRow(int y, const std::vector<std::pair<int, int>> &_row);
    void SaveToFile(const QString &_fileName);

    void Clear();

    bool LoadFromFile(const QString &_fileName);

    const QString & GetFileName() const {return FileName;}

    double GetDeltaDiag() const {return DeltaDiag;}

    void SetImageFileName(const QString &_fn) {ImageFileName = _fn;}
    const QString & GetImageFileName() const {return ImageFileName;}

    void SetLegendFileName(const QString &_fn) {LegendFileName = _fn;}
    const QString & GetLegendFileName() const {return LegendFileName;}

    const auto & GetReliefMap() const {return ReliefMap;}
};

#endif // RELIEF_H
