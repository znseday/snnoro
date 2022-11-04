#ifndef MYCONFIG_H
#define MYCONFIG_H

#include <vector>

#include "SignalNode.h"
#include "UnitGradDescent.h"
#include "Route.h"
#include "Types.h"
#include "Relief.h"

#include <GL/glu.h>

class TargetFunctionBase;

struct Settings3dType
{
    float TrX = 0;
    float TrY = 0;
    float TrZ = -3.5f;
    float RotX = 0;
    float RotY = 0;
    float RotZ = 0;
    //float OrthoZoom = 1.0f;
    bool IsPerspective = false;
};


class MyConfig
{
protected:

    mutable GLint vport[4];
    mutable GLdouble modl[16], proj[16];

    std::vector<SignalNode> Nodes;
    int iCurNode = -1;

    double CurCost = std::numeric_limits<double>::max();

    std::vector<Route> Routes;

    bool IsGradCalculating = false;

    static GLUquadric * Quadric();

    Relief3D *Relief = nullptr;

    void InitNodeCoordsFromParams(const std::vector<double> & _params, SignalNodeType _snt);
    void InitParamsFromNodeCoords(const int _param_count, SignalNodeType _snt);

    struct StatsStruct
    {
        double PercentOfCovered = 0;
        int UncoveredCount = -1;
        double LastCostPhase1 = 0;
        double LastCostPhase2 = 0;
        void Reset();
    } Stats;


    void DrawIntersectsWithEllipses(const Settings3dType & _settings3d) const;

public:

    Settings3dType Settings3d;
    tf_gd_lib::GradDescent GradDesc;

    const std::vector<SignalNode> & GetNodes() const {return Nodes;}
    std::vector<SignalNode> & NodesAccess() {return Nodes;}

    std::vector<Route> & RoutesAccess() {return Routes;}

    MyConfig() = default;

    MyConfig(const MyConfig &) = default;
    MyConfig & operator=(const MyConfig &) = default;

    MyConfig(MyConfig &&) = default;
    MyConfig & operator=(MyConfig &&) = default;

//    void SetArea(const QRectF &_area) {Area = _area;}
    void SetNodes(const std::vector<SignalNode> &_nodes) {Nodes = _nodes;}
    void SetRoutes(const std::vector<Route> &_routes) {Routes = _routes;}
    void SetRelief(Relief3D *_relief3D) {Relief = _relief3D;}

    const Relief3D * GetRelief() const {return Relief;}

//    void SetRandomNodeCoords(const QRectF &_area);
    void SetRandomNodeCoords();

    void DrawIn3D(SignalNodeType _snt, bool isDrawAbonents) const;

    bool StartGradDescent(int nDraw, const tf_gd_lib::GradDescent &_protoGD,
//                          const TargetFuncSettingsStruct &_targetFuncSettings,
                          /*const*/ TargetFunctionBase &_targetFunction,
                          SignalNodeType _snt,
                          IGradDrawable *pGLWidget = nullptr);

    bool StartFinalGradDescent(int nDraw, const tf_gd_lib::GradDescent &_protoGD,
//                               const TargetFuncSettingsStruct &_targetFuncSettings,
                               /*const*/ TargetFunctionBase &_targetFunction,
                               SignalNodeType _snt,
                               IGradDrawable *pGLWidget = nullptr);
    void CancelGradDescent();

    bool CheckIsAllCovered() const;

    void PrintBondsInfo() const;

    void CalcPointStats();
    const StatsStruct & GetStats() const {return Stats;}

    void CalcBonds(const TargetFunctionBase &_targetFuncSettingsBase, SignalNodeType _snt);

    void FillExternVportModlAndProj(GLint _vport[4], GLdouble _modl[16], GLdouble _proj[16]) const;

    double IsLineBetweenTwoPoints(const Pos3d &p1, const Pos3d &p2) const;

    void TestTwoLines() const;

    void CalcAccessRateForAbos(bool _isUseLineBetweenTwoPoints, SignalNodeType _snt);

    void FindCoveredPointsUsingParams(const std::vector<double>& params, SignalNodeType _snt);


    void CalcAccessRateForCurrent(); // DEBUG

    void SelectCurNodeByRealXY(double x, double y);
    void PutCurNodeByRealXYZ(double x, double y, double z);
    void SetDirectCurNodeByRealXYZ(double x, double y, double z);

    int Get_iCurNode() const {return iCurNode;}
};

//bool operator<(const MyConfig &a, const MyConfig &b)
//{
//    return a.GradDesc.GetLastCost() < b.GradDesc.GetLastCost();
//}

#endif // MYCONFIG_H
