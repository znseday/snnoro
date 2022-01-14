#ifndef MYGRADMODEL_H
#define MYGRADMODEL_H


#include <vector>

#include "Route.h"
#include "GradConfig.h"
#include "Types.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QString>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include "Relief.h"


class MyGradModel
{
protected:

    int OldX = 0, CurrentX = 0;
    int OldY = 0, CurrentY = 0;

    bool IsRandomNodeCoords = true;
    std::vector<SignalNode> Nodes;
    SignalNodeType NodesType = SignalNodeType::Sphere;
    bool IsDrawAbonents = false;

    std::vector<Route> Routes;
    bool IsRandomRoutes = false;

    ReliefMatInfoStruct ReliefMatInfo; // ?
    Relief3D Relief;
    bool IsRandomRelief = true;

    std::vector<MyConfig> Configs;

    int Width = 200, Height = 200;
    int iCurConfig = -1;
    bool DrawOnlyOne = false;
    std::vector<QRect> ViewPorts;
    bool IsPerspectiveForAll = false;

    QString Name = "Unknown";
    QString FileName;
    //bool IsLoaded = false; // Not needed anymore
    bool IsSaved = true;

    size_t nDraws = 1;
    bool IsGradCalculating = false;

    size_t ParseJson(const QJsonObject &_jsonObject, const QJsonParseError &_parseError);

    QJsonArray RepresentRoutesAsJsonArray() const;
    QJsonArray RepresentNodesAsJsonArray() const;
    QJsonObject RepresentReliefInfoAsJsonObject() const;

public:

    tf_gd_lib::GradDescent ProtoGradDesc;
    TargetFuncSettingsStruct TargetFuncSettings;

    const std::vector<MyConfig> & GetConfigs() const {return Configs;}

    MyGradModel();

    void Set_nDraws(size_t _nDraws);

    bool IsDrawOnlyOne() const {return DrawOnlyOne;}
    const std::vector<QRect> & GetViewPorts() const {return ViewPorts;}

    void SetWidthAndHeight(int _w, int _h) {Width = _w; Height = _h;}

    bool LoadFromFile(const QString &_fileName);

    int GetPopulationSize() const {return Configs.size();}
    void CreatePopulation(size_t _count);

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
    void OnMouseWheel(QWheelEvent *pe);

    bool StartGradDescent_Phase_1(IGradDrawable *pGLWidget);
    bool StartGradDescent_Phase_1_for_Current(IGradDrawable *pGLWidget);
    bool RemoveUncovered(IGradDrawable *pGLWidget);
    bool StartGradDescent_Phase_2(IGradDrawable *pGLWidget);
    bool StartGradDescent_Phase_2_for_Current(IGradDrawable *pGLWidget);
    void CancelGradDescent();

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

    void ChangeFileName(const QString &_newFileName);
    const QString &GetFileName() const {return FileName;}

    void NewGradModelBulk();
    void ReCreateSignalNodes(size_t _count, double _R, double _betha);  // ???

    void ApplySignalNodesToAllConfigs(); //????????????????????

    const std::vector<SignalNode> & GetSignalNodes() const {return Nodes;}
    std::vector<SignalNode> & SignalNodesDirectAccess() {return Nodes;}

    SignalNodeType GetNodesType() const {return NodesType;}
    void SetNodesType(SignalNodeType _snt) {NodesType = _snt;}

    const MyConfig & GetCurrentConfig() const;
    MyConfig & CurrentConfigAccess();

    void TestTwoLines();
};

#endif // MYGRADMODEL_H
