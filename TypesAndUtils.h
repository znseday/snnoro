#ifndef MYTYPES_H
#define MYTYPES_H

//#include <QVector3D>
//using Pos3d = QVector3D;

#include <QJsonObject>

enum class WorkModeType
{
    Nothing,
//    OldWork,
    GradWork
};

enum class WorldModeType
{
    Nothing,
    AddingRoutePoints,
    DeletingRoute,
    SelectingSignalNode,
    EditingPosSignalNode,
    EditingAngleSignalNode
};

enum class NeedToSave
{
    Need,
    DoNotNeed
};

enum class TargetFuncTypeEnum
{
    Unknown,
    Additive,
    Probabilistic
};

struct WhatShowStruct
{
    bool ShowGrid = false;
    bool ShowCones = false;
    bool ShowRadii = false;
    bool ShowEllipses = false;
    bool ShowLinesBetweenSNandPoints = false;
    bool ShowPointsOnRadii = false;
    bool ShowAreaForRandomNodes = false;
    bool ShowAreaForGradDesc = false;
    bool ShowRoutes = false;

    enum class WhatIsolinesEnum
    {
        Relief, ARF
    } WhatIsolines = WhatIsolinesEnum::Relief;

    bool ShowIsolinesOfAccessRate = false;

    QJsonObject RepresentAsJsonObject() const;
    void LoadFromJsonObject(const QJsonObject &_jsonObject);
};

using LegendIsolinesType = std::vector<std::pair<QColor, double>>;


class IGradDrawable
{
public:
    virtual void Repaint() = 0;
    virtual ~IGradDrawable() = default;
};

[[nodiscard]] bool CorrectFileNameIfDoesntExist(QString &_fileName,
                                  const QString &_defDir,
                                  const QString &_what,
                                  const QString &_extension);

#endif // MYTYPES_H
