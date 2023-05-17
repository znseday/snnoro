#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include <QStateMachine>
#include <QMainWindow>

class MainGLWidget;

#include "TypesAndUtils.h"

#include "GradModel.h"

#include "DialogGradConfig.h"
#include "DialogChangePopulationSize.h"
#include "DialogNewGradConfig.h"
#include "DialogEditSignalNodes.h"
#include "DialogEditRoutes.h"


class FormGradGeneral;

#include "FormRelief.h"

#include "FormLegendIsolines.h"

#include "FormAboCalc.h"
#include "DialogAboReport.h"

#include "MyStateMachine.h"

class QCloseEvent;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int GetGLWidth() const;
    int GetGLHeight() const;

    void AfterShow();

private slots:
    void on_actionFileNew_Grad_Config_triggered();
    void on_actionFileOpen_Grad_Descent_triggered();
    void on_actionFileSave_Grad_Config_triggered();
    void on_actionFileSave_Grad_Config_As_triggered();
    void on_actionFileExit_triggered();

    void on_actionGradSwitch_Show_One_All_triggered();
    void on_actionGradSetDraw2_triggered();
    void on_actionGradSetDraw3_triggered();
    void on_actionGradSetDraw4_triggered();
    void on_actionGradSetDraw6_triggered();
    void on_actionGradSetDraw9_triggered();
    void on_actionGrad_SetDrawCount_Custom_triggered();
    void on_actionGradSwitch_Pespective_for_Current_triggered();
    void on_actionGradSwitch_Pespective_for_All_triggered();
    void on_actionGradReset_View_Point_for_Current_triggered();
    void on_actionGradReset_View_Point_for_All_triggered();
    void on_actionGradChange_Population_size_triggered();
    void on_actionGradSettings_triggered();
    void on_actionGradCalc_Bonds_triggered();
    void on_actionGradStart_Phase_1_triggered();
    void on_actionGradStart_Phase_1_for_Current_Config_triggered();
    void on_actionGradRemove_Uncovered_triggered();
    void on_actionGradStart_Phase_2_triggered();
    void on_actionGradStart_Phase_2_for_Current_Config_triggered();
    void on_actionGradStop_triggered();
    void on_actionGradShow_Tables_triggered();

    void on_actionWorld_Show_Abonents_triggered();
    void on_actionWorld_Show_Grid_triggered();

    void on_actionRelief_Relief_Creator_triggered();

    void on_actionEdit_Add_New_Route_triggered();
    void on_actionEdit_Finish_Route_triggered();
    void on_actionEdit_Delete_Route_triggered(); 
    void on_actionEdit_Select_Cur_Node_triggered();
    void on_actionEdit_Editing_Pos_Cur_Node_triggered();
    void on_actionEdit_Editing_Angle_Cur_Node_triggered();
    void on_actionEdit_Edit_Signal_Nodes_for_All_triggered();
    void on_actionEdit_Edit_Signal_Nodes_for_Current_triggered();
    void on_actionEdit_Edit_Routes_triggered();
    void on_actionEdit_Change_Count_of_Nodes_triggered();
    void on_actionEdit_Apply_Cur_Node_to_All_Configs_triggered();

    void on_actionDebug_TwoLines_triggered();
    void on_actionDebug_Get_Last_Cost_For_Current_triggered();
    void on_actionDebug_Calc_Access_Rate_for_current_triggered();

    void on_actionWorld_Show_Cones_triggered();

    void on_actionWorld_Show_Radii_triggered();

    void on_actionWorld_Show_Ellipses_triggered();

    void on_actionWorld_Show_Lines_Between_SN_and_Points_triggered();

    void on_actionWorld_Show_Points_on_Radii_triggered();

    void on_actionWorld_Show_Area_For_Random_Nodes_triggered();

    void on_actionWorld_Show_Area_For_Grad_Descent_triggered();

    void on_actionWorld_Show_Isolines_of_Access_Rate_triggered();

    void on_actionWorld_Show_Routes_triggered();

    void on_actionWorld_Show_Legend_Isolines_triggered();

private:
    Ui::MainWindow *ui;

    MainGLWidget *mainGLWidget = nullptr;

    WorkModeType WorkMode = WorkModeType::Nothing;
    WorldModeType WorldMode = WorldModeType::Nothing;

    MyGradModel GradModel;

    DialogGradConfig DialogGradSettings;
    DialogChangePopulationSize DialogNewPopulationSize;
    DialogNewGradConfig DialogGradConfigNew;

    DialogEditSignalNodes DialogSignalNodesEdit;
    DialogEditRoutes DialogRoutesEdit;

    FormGradGeneral *formGradGeneral = nullptr;

    FormRelief formRelief;

    FormLegendIsolines formLegendIsolines;

    FormAboCalc formAboCalc;
    DialogAboReport dlgAboReport;

    QLabel *lblGlX;
    QLabel *lblGlY;
    QLabel *lblGlZ;

    QLabel *lblWorldX;
    QLabel *lblWorldY;
    QLabel *lblWorldZ;

    QLabel *lbl_iCurConfig;

    QLabel *lblCoord;

    bool IsGradDescFileSavedSuccessfully = false;
    void TryToSaveGradDescToFile();
    bool CheckIsSavedAndSaveIfNecessary();

    void UpdateCurNodeCoordsOnLabel();

    MyStateMachine StateMachine;

    void Init_UI_AccordingGlobalSettings();

    void IsolinesTurnOff();
    void SendLegendIsolines();

public slots:
    void SlotReceiveWorldCoords(double wx, double wy, double wz, bool wExists);
    void SlotReceiveRouteDeleted(bool isDeleted);

    void SlotReceiveAboTime(int t, TargetFuncTypeEnum funcType);
    void SlotReceiveFormAboCalcClose();

    void SlotReceiveAddTimePointToReport(int t);
    void SlotReceiveShowAboReport(TargetFuncTypeEnum funcType);

    void SlotReceive_iCurConfigChanged(int i);

    void SlotReceiveNodeCoords(int n, double x, double y, double z);

    // QPaintDevice interface
public:
    //QPaintEngine *paintEngine() const override;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;

};

#endif // MAINWINDOW_H
