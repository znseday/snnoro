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
//    ShowCoords,
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

//struct TargetFuncSettingsStruct // Переделать, чтобы у каждый целевой функции
//                                // был свой пакет настроек
//{
//    double Aarf = 1e8;
//    double A2 = 4e7;
//    double p = 1e-5;
//    double offX = 0.01;
//    double k_step_ot = 0.03;
//    double R_nodeOverlap = 50;
//    bool IsUseCoveredFlag = true;
//    bool IsUseLineBetweenTwoPoints = false;

////    TargetFuncEnum TargetFuncType = TargetFuncEnum::Empty;
//    std::string ActiveTargetFuncFirstPhase = "AdditiveSphereFirstPhase";
//    std::string ActiveTargetFuncSecondPhase = "AdditiveSphereSecondPhase";
//};


class IGradDrawable
{
public:
    virtual void Repaint() = 0;
    virtual ~IGradDrawable() = default;
};

#endif // MYTYPES_H
