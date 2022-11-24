#ifndef MYTYPES_H
#define MYTYPES_H

#include <QVector3D>

using Pos3d = QVector3D;

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

//enum class TargetFuncEnum
//{
//    Empty,
//    Additive,
//    Probabilistic
//};

class IGradDrawable
{
public:
    virtual void Repaint() = 0;
    virtual ~IGradDrawable() = default;
};

void CorrectFileNameIfDoesntExist(QString &_fileName,
                                  const QString &_defDir,
                                  const QString &_what,
                                  const QString &_extension);

#endif // MYTYPES_H
