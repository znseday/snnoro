#ifndef MYGRADMODEL_H
#define MYGRADMODEL_H


#include <vector>
#include <QString>

#include "Route.h"
#include "GradConfig.h"
#include "TypesAndUtils.h"
#include "TargetFunctions/TargetFunctionBase.h"

#include "Relief.h"

class QJsonObject;
class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

extern const QString SettingsGDExtension;
extern const QString SettingsTFExtension;

class MyGradModel : public QObject
{
    Q_OBJECT
protected:

    size_t nDraws = 1;
    std::vector<SignalNode> Nodes;
    std::vector<Route> Routes;
    std::vector<MyConfig> Configs;
    std::vector<QRect> ViewPorts;

    QRect BigViewPort;

    QString Name = "Unknown";
    QString FileName;
    bool IsSaved = true;

    int OldX = 0, CurrentX = 0;
    int OldY = 0, CurrentY = 0;

    bool IsRandomNodeCoords = true;

    SignalNodeType NodesType = SignalNodeType::Sphere;
    bool IsDrawAbonents = false;

    bool IsRandomRoutes = false;

    GridSettingsStruct GridSettings;
    IsolinesSettingsStruct IsolinesSettings;

    Relief3D Relief;
    bool IsRandomRelief = false;

    int Width = 200, Height = 200;
    int iCurConfig = -1;
    bool DrawOnlyOne = false;

    bool IsPerspectiveForAll = false;

    bool IsGradCalculating = false;

    size_t ParseJson(const QJsonObject &_jsonObject, const QJsonParseError &_parseError);

    QJsonArray RepresentRoutesAsJsonArray() const;
    QJsonArray RepresentNodesAsJsonArray() const;

    std::map<std::string, TargetFunctionBase*> TargetFunctions;

    std::string ActiveTargetFuncFirstPhase = "AdditiveSphereFirstPhase";
    std::string ActiveTargetFuncSecondPhase = "AdditiveSphereSecondPhase";

    QString GradDescFileName;

    BoundsStruct BoundsRandCoords;   QRectF AreaRandCoords;
    BoundsStruct BoundsGradDesc;     QRectF AreaGradDesc;

    QRectF CalcSomeAreaByBounds(const BoundsStruct &_bs);
    void CorrectAreaBoundsIfItIsOutOfRelief(BoundsStruct &_bs);

    WhatShowStruct WhatShow;

    TargetFuncTypeEnum GetTargetFuncTypeByCurrentFuncNameName();

public:

    void ReCalcAreasByBounds();

    void SetActiveTargetFuncFirstPhase(const std::string &s) {ActiveTargetFuncFirstPhase = s;}
    const std::string & GetActiveTargetFuncFirstPhase() const {return ActiveTargetFuncFirstPhase;}
    void SetActiveTargetFuncSecondPhase(const std::string &s) {ActiveTargetFuncSecondPhase = s;}
    const std::string & GetActiveTargetFuncSecondPhase() const {return ActiveTargetFuncSecondPhase;}

    void SetGradDescFileName(const QString &fn) {GradDescFileName = fn;}
    const QString GetGradDescFileName() const {return GradDescFileName;}

    tf_gd_lib::GradDescent ProtoGradDesc;
    TargetFuncSettingsStruct TargetFuncSettingsGlobal;

    const std::vector<MyConfig> & GetConfigs() const {return Configs;}
    const MyConfig & GetActiveConfig() const;
    int Get_iCurConfig() const {return iCurConfig;}

    MyGradModel();
    ~MyGradModel();

    void Set_nDraws(size_t _nDraws);
    size_t Get_nDraw() const {return nDraws;}

    void SetWhatShowWhatIsolines(WhatShowStruct::WhatIsolinesEnum wi) {WhatShow.WhatIsolines = wi;}

    bool IsDrawOnlyOne() const {return DrawOnlyOne;}
    const std::vector<QRect> & GetViewPorts() const {return ViewPorts;}

    void SetWidthAndHeight(int _w, int _h) {Width = _w; Height = _h;}

    bool LoadFromFile(const QString &_fileName);

    int GetPopulationSize() const {return Configs.size();}
    void CreatePopulation(size_t _count);

    void SetRandomNodeCoords();

    void DrawMeIn3D();
    void DrawOneConfig(size_t ind, bool OnlyOne);
    void DrawSeveralConfigs();

    void SwitchDrawOnlyOne();
    void SetDrawOnlyOne(bool _drawOnlyOne) {DrawOnlyOne = _drawOnlyOne;}

    void SwitchPerspective();
    void SwitchPerspectiveForAll();

    void ResetViewPoint();
    void ResetViewPointForAll();

    void OnResize(int _width, int _height);

    void OnKeyPress(QKeyEvent *pe);

    void OnMousePress(QMouseEvent *pe);
    void OnMouseMove(QMouseEvent *pe);
    void OnMouseWheel(QWheelEvent *pe, bool wExists,
                      double wx, double wy, double wz);

    bool StartGradDescent_Phase_1(IGradDrawable *pGLWidget);
    bool StartGradDescent_Phase_1_for_Current(IGradDrawable *pGLWidget);
    bool RemoveUncovered(IGradDrawable *pGLWidget);
    bool StartGradDescent_Phase_2(IGradDrawable *pGLWidget);
    bool StartGradDescent_Phase_2_for_Current(IGradDrawable *pGLWidget);
    void CancelGradDescent();

    bool GetIsGradCalculating() const {return IsGradCalculating;}

    void DeleteConfigsWithUncoveredPoints();

    void CalcBonds();

//    bool GetIsSettingsSaved() const {return IsSettingsSaved;}
    bool GetIsSaved() const {return IsSaved;}
//    void MarkAsSaved() {IsSaved = true;}
    void MarkAsNotSaved() {IsSaved = false;}
    bool SaveToFile(/*const QString &_fileName*/);

    int CalcCurViewPortNumber(int ClientX, int ClientY) const;

    const Relief3D & GetRelief() const {return Relief;}
    bool LoadReliefFromFile(const QString &_reliefFileName);

    bool AddNewRoute();
    void AddNewPointToLastRoute(double wx, double wy);
    void FinishRoute();
    bool DeleteRoute(double wx, double wy);
    void CalcAbonentsPos(int t);

    void ChangeFileName(const QString &_newFileName);
    const QString &GetFileName() const {return FileName;}

    void NewGradModelBulk();
    void ReCreateReliefListsAndBuildRelief();
    void ReCreateSignalNodes(size_t _count, double _R, double _betha);  // ???

    void ApplySignalNodesToAllConfigs();
    void ApplyRoutesToAllConfigs(NeedToSave _NeedToSave);

    void ApplyAbonentsPosInRoutesToAllConfigs();

    void ApplyCurNodeFromCurConfigToAllConfigs();

    const std::vector<SignalNode> & GetSignalNodes() const {return Nodes;}
    std::vector<SignalNode> & SignalNodesDirectAccess() {return Nodes;}

    const std::vector<Route> & GetRoutes() const {return Routes;}
    std::vector<Route> & RoutesDirectAccess() {return Routes;}

    SignalNodeType GetNodesType() const {return NodesType;}
    void SetNodesType(SignalNodeType _snt) {NodesType = _snt;}

    const MyConfig & GetCurrentConfig() const;
    MyConfig & CurrentConfigAccess();

    void TestTwoLines();
    void TestGetLastCostForCurrent();

    void SetIsDrawAbonents(bool _isDrawAbo) {IsDrawAbonents = _isDrawAbo;}

    void ReCalcAboAccessRate(TargetFuncTypeEnum funcType);

    const auto & GetTargetFunctions() const {return TargetFunctions;}

//    static TargetFuncEnum ConvertStringToTargetFuncType(QString &str);
//    static QString ConvertTargetFuncTypeToString(TargetFuncEnum snt);

    void CalcAccessRateForCurrent();

    void SelectCurNodeByPos(double wx, double wy);
    void PutCurNodeByPos(double wx, double wy);
    void SetDirectCurNodeByPos(double wx, double wy);

    void SetShowGridOnRelief(bool _isShow);
    void SetShowIsolinesOfAccessRate(bool _isShow);

    void SetShowCones(bool _isShow)    {WhatShow.ShowCones = _isShow;}
    void SetShowRadii(bool _isShow)    {WhatShow.ShowRadii = _isShow;}
    void SetShowEllipses(bool _isShow) {WhatShow.ShowEllipses = _isShow;}
    void SetShowLinesBetweenSNandPoints(bool _isShow) {WhatShow.ShowLinesBetweenSNandPoints = _isShow;}
    void SetShowPointsOnRadii(bool _isShow)           {WhatShow.ShowPointsOnRadii = _isShow;}
    void SetShowAreaForRandomNodes(bool _isShow)      {WhatShow.ShowAreaForRandomNodes = _isShow;}
    void SetShowAreaForGradDesc(bool _isShow)         {WhatShow.ShowAreaForGradDesc = _isShow;}
    void SetShowRoutes(bool _isShow)   {WhatShow.ShowRoutes = _isShow;}

    void SetGridSettings(const GridSettingsStruct &_gs) {GridSettings = _gs;}
    const GridSettingsStruct & GetGridSettings() const {return GridSettings;}

    void SetIsolinesSettings(const IsolinesSettingsStruct &_is) {IsolinesSettings = _is;}
    const IsolinesSettingsStruct & GetIsolinesSettings() const {return IsolinesSettings;}

    void SetBoundsRandCoords(const BoundsStruct &_bs) {BoundsRandCoords = _bs; CorrectAreaBoundsIfItIsOutOfRelief(BoundsRandCoords);}
    const BoundsStruct & GetBoundsRandCoords() const {return BoundsRandCoords;}

    void SetBoundsGradDesc(const BoundsStruct &_bs) {BoundsGradDesc = _bs; CorrectAreaBoundsIfItIsOutOfRelief(BoundsRandCoords);}
    const BoundsStruct & GetBoundsGradDesc() const {return BoundsGradDesc;}

    bool SaveGlobalSettings() const;
    bool LoadGlobalSettings();

    const WhatShowStruct & GetWhatShow() const {return WhatShow;}

signals:

    void SignalSendNodeCoords(int, double, double, double);

};

#endif // MYGRADMODEL_H
