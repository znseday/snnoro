#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "MainGLWidget.h"

#include "FormGeneral.h"
#include "FormConfigInfo.h"

#include "Types.h"

#include "GradModel.h"

#include "DialogGradConfig.h"
#include "DialogChangePopulationSize.h"
#include "DialogNewGradConfig.h"
#include "DialogEditSignalNodes.h"
#include "DialogEditRoutes.h"

#include "FormGradGeneral.h"

#include "FormRelief.h"

#include "FormAboCalc.h"
#include "DialogAboReport.h"

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
    void on_actionFileExit_triggered();

    void on_actionFileStart_Old_triggered();
    void on_actionOldTablesGeneral_triggered();
    void on_actionOldTablesConfig_0_triggered();
    void on_actionOldTablesConfig_1_triggered();
    void on_actionOldTablesConfig_2_triggered();
    void on_actionOldTablesConfig_3_triggered();
    void on_actionFileOpen_Grad_Descent_triggered();
    void on_actionGradSetDraw2_triggered();
    void on_actionGradSetDraw3_triggered();
    void on_actionGradSwitch_Show_One_All_triggered();
    void on_actionGradSwitch_Pespective_for_Current_triggered();
    void on_actionGradSwitch_Pespective_for_All_triggered();
    void on_actionGradReset_View_Point_for_Current_triggered();
    void on_actionGradReset_View_Point_for_All_triggered();
    void on_actionGradSetDraw4_triggered();
    void on_actionGradSetDraw6_triggered();
    void on_actionGradSetDraw9_triggered();

    void on_actionGradStart_Phase_1_triggered();
    void on_actionGradStop_triggered();
    void on_actionGradSettings_triggered();
    void on_actionGradStart_Phase_2_triggered();
    void on_actionGradRemove_Uncovered_triggered();

    void on_actionGradChange_Population_size_triggered();

    void on_actionShow_Tables_triggered();

    void on_actionGradCalc_Bonds_triggered();

    void on_actionRelief_Relief_Creator_triggered();

    void on_actionFileSave_Grad_Config_triggered();

    void on_actionWorld_Show_Coords_toggled(bool arg1);

    void on_actionEdit_Add_New_Route_triggered();

    void on_actionEdit_Finish_Route_triggered();

    void on_actionFileSave_Grad_Config_As_triggered();

    void on_actionFileNew_Grad_Config_triggered();

    void on_actionEdit_Delete_Route_triggered();

    void on_actionEdit_Edit_Signal_Nodes_for_All_triggered();

    void on_actionTwoLines_triggered();

    void on_actionGradStart_Phase_1_for_Current_Config_triggered();

    void on_actionGradStart_Phase_2_for_Current_Config_triggered();

    void on_actionEdit_Edit_Signal_Nodes_for_Current_triggered();

    void on_actionEdit_Edit_Routes_triggered();

    void on_actionWorld_Show_Abonents_triggered();

private:
    Ui::MainWindow *ui;

    MainGLWidget *mainGLWidget = nullptr;

    FormGeneral *formGeneral = nullptr;
    FormConfigInfo *formConfigInfo[4] = {nullptr};

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

    FormAboCalc formAboCalc;
    DialogAboReport dlgAboReport;

    QLabel *lblGlX;
    QLabel *lblGlY;
    QLabel *lblGlZ;

    QLabel *lblWorldX;
    QLabel *lblWorldY;
    QLabel *lblWorldZ;

    bool IsGradDescFileSavedSuccessfully = false;
    void TryToSaveGradDescToFile();
    bool CheckIsSavedAndSaveIfNecessary();

public slots:
    void SlotReceiveWorldCoords(double wx, double wy, double wz, bool wExists);
    void SlotReceiveRouteDeleted(bool isDeleted);

    void SlotReceiveAboTime(int t);
    void SlotReceiveFormAboCalcClose();

    void SlotReceiveAddTimePointToReport(int t);
    void SlotReceiveShowAboReport();

    // QPaintDevice interface
public:
    //QPaintEngine *paintEngine() const override;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H
