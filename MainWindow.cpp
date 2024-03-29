#include "MainWindow.h"
#include "ui_MainWindow.h"


#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

#include <QInputDialog>
#include <QCloseEvent>

#include <limits>

#include "FormGradGeneral.h"
#include "MainGLWidget.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle(QApplication::applicationName());

    lblGlX = new QLabel("n/a");
    lblGlY = new QLabel("n/a");
    lblGlZ = new QLabel("n/a");
    lblWorldX = new QLabel("n/a");
    lblWorldY = new QLabel("n/a");
    lblWorldZ = new QLabel("n/a");
    lbl_iCurConfig = new QLabel("n/a");
    lblCoord = new QLabel("n/a");

    lblGlX->setFixedWidth(100);
    lblGlY->setFixedWidth(100);
    lblGlZ->setFixedWidth(100);
    lblWorldX->setFixedWidth(120);
    lblWorldY->setFixedWidth(120);
    lblWorldZ->setFixedWidth(120);
    lbl_iCurConfig->setFixedWidth(100);
    lblCoord->setFixedWidth(220);

    ui->statusbar->addWidget(lblGlX);
    ui->statusbar->addWidget(lblGlY);
    ui->statusbar->addWidget(lblGlZ);
    ui->statusbar->addWidget(lblWorldX);
    ui->statusbar->addWidget(lblWorldY);
    ui->statusbar->addWidget(lblWorldZ);
    ui->statusbar->addWidget(lbl_iCurConfig);
    ui->statusbar->addWidget(lblCoord);

    mainGLWidget = new MainGLWidget(WorkMode, GradModel, WorldMode);

    //mainGLWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //mainGLWidget->sizePolicy().setHorizontalPolicy(QSizePolicy::Expanding);
    //ui->horizontalLayout->addWidget(mainGLWidget);

    //ui->wgtCentral->sizePolicy().setHorizontalPolicy(QSizePolicy::Expanding);
    //ui->wgtCentral->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //setCentralWidget(ui->wgtCentral);

    setCentralWidget(mainGLWidget);

    connect(mainGLWidget, SIGNAL(SignalSendWorldCoords(double,double,double,bool)),
            this, SLOT(SlotReceiveWorldCoords(double,double,double,bool)));

    connect(mainGLWidget, SIGNAL(SignalRouteDeleted(bool)),
            this, SLOT(SlotReceiveRouteDeleted(bool)));

    connect(mainGLWidget, SIGNAL(Signal_iCurConfigChanged(int)),
            this, SLOT(SlotReceive_iCurConfigChanged(int)));

    mainGLWidget->setFocus();



    formGradGeneral = new FormGradGeneral(GradModel.GetConfigs());

    formGradGeneral->setWindowTitle("Grad descent Results");


    connect(&formAboCalc, SIGNAL(SignalSendAboTime(int,TargetFuncTypeEnum)),
            this, SLOT(SlotReceiveAboTime(int,TargetFuncTypeEnum)));

    connect(&formAboCalc, SIGNAL(SignalFormAboCalcClose()),
            this, SLOT(SlotReceiveFormAboCalcClose()));


    connect(&formAboCalc, SIGNAL(SignalAddTimePointToReport(int)),
            this, SLOT(SlotReceiveAddTimePointToReport(int)));

    connect(&formAboCalc, SIGNAL(SignalShowAboReport(TargetFuncTypeEnum)),
            this, SLOT(SlotReceiveShowAboReport(TargetFuncTypeEnum)));

    connect(&GradModel, SIGNAL(SignalSendNodeCoords(int, double, double, double)),
            this, SLOT(SlotReceiveNodeCoords(int, double, double, double)));


    if ( !GradModel.LoadGlobalSettings() )
    {
        qDebug() << "Warning! GlobalSettings not loaded!";
    }
    Init_UI_AccordingGlobalSettings();

    StateMachine.InitStateMachine(ui);

    mainGLWidget->setMouseTracking(true);

    dlgAboReport.InitDialog();
}
//-------------------------------------------------------------

void MainWindow::AfterShow()
{
    on_actionFileOpen_Grad_Descent_triggered();  // for debug
//    on_actionRelief_Relief_Creator_triggered();    // for debug
}
//-------------------------------------------------------------

void MainWindow::IsolinesTurnOff()
{
    ui->actionWorld_Show_Isolines_of_Access_Rate->setChecked(false);
    on_actionWorld_Show_Isolines_of_Access_Rate_triggered();
}
//-------------------------------------------------------------

int MainWindow::GetGLWidth() const
{
    return mainGLWidget->width();
}
//-------------------------------------------------------------

int MainWindow::GetGLHeight() const
{
    return mainGLWidget->height();
}
//-------------------------------------------------------------

MainWindow::~MainWindow()
{
    delete ui;
}
//-------------------------------------------------------------

void MainWindow::on_actionFileNew_Grad_Config_triggered()
{
//    if (StateMachine.isRunning())
    if (StateMachine.IsRunning())
    {
        qDebug() << "StateMachine is already running";
    }

    if (!CheckIsSavedAndSaveIfNecessary())
        return;

    // Здесь сделать что-нибудь еще: новая конфигурация
    // DialogGradConfigNew.InitDialog(GradModel);

    if (DialogGradConfigNew.exec() == QDialog::Accepted)
    {
        GradModel.NewGradModelBulk();
        GradModel.SetWidthAndHeight(mainGLWidget->width(), mainGLWidget->height());

        if (!DialogGradConfigNew.CreateNewGradModel(GradModel))
        {
            QMessageBox::warning(this, "Warning", "Failed to Create New Grad Config");
            return;
        }
    }
    else
    {
        return;   // Rejected
    }

    WorkMode = WorkModeType::GradWork;
    GradModel.MarkAsNotSaved();

    on_actionWorld_Show_Grid_triggered();

    on_actionWorld_Show_Isolines_of_Access_Rate_triggered();

    StateMachine.ToNormal();

    GradModel.Set_nDraws(35);
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionFileOpen_Grad_Descent_triggered()
{
    if (!CheckIsSavedAndSaveIfNecessary())
        return;

    QString fileName = QFileDialog::getOpenFileName(this,
        "Open Config file", ".", "Config Files (" + ConfigsExtension + ")");


    if (fileName.isEmpty())
    {
        qDebug() << "Canceled by user";
        return;
    }

    if (!GradModel.LoadFromFile(fileName))
    {
        qDebug() << (fileName + " not found");
        return;
    }

    WorkMode = WorkModeType::GradWork;


    GradModel.SetWidthAndHeight(mainGLWidget->width(), mainGLWidget->height());

    on_actionWorld_Show_Grid_triggered();

    on_actionWorld_Show_Isolines_of_Access_Rate_triggered();

    GradModel.Set_nDraws(35);
    mainGLWidget->repaint();

    StateMachine.ToNormal();
}
//-------------------------------------------------------------

void MainWindow::on_actionFileSave_Grad_Config_triggered()
{
    if ( GradModel.GetFileName() == "" )
    {
        QString fileName = QFileDialog::getSaveFileName(this,
            "Save Config file", ".", "Config Files (" + ConfigsExtension + ")");

        if (fileName == "")
        {
            qDebug() << "Warning: File name is not set. Abort saving.";
            return;
        }
        GradModel.ChangeFileName(fileName);
    }

    TryToSaveGradDescToFile();
}
//-------------------------------------------------------------

void MainWindow::on_actionFileSave_Grad_Config_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Config file", ".", "Config Files (" + ConfigsExtension + ")");

    if (fileName == "")
    {
        qDebug() << "Warning: File name is not set. Abort saving.";
        return;
    }
    GradModel.ChangeFileName(fileName);

    TryToSaveGradDescToFile();
}
//-------------------------------------------------------------

void MainWindow::on_actionFileExit_triggered()
{
    QApplication::closeAllWindows();
//    this->close();
}
//-------------------------------------------------------------

void MainWindow::on_actionGradSwitch_Show_One_All_triggered()
{
    if (WorkMode == WorkModeType::GradWork)
    {
        GradModel.SwitchDrawOnlyOne();
        mainGLWidget->repaint();
    }
}
//-------------------------------------------------------------

void MainWindow::on_actionGradSetDraw2_triggered()
{
    GradModel.Set_nDraws(2);
    GradModel.SetDrawOnlyOne(false);
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionGradSetDraw3_triggered()
{
    GradModel.Set_nDraws(3);
    GradModel.SetDrawOnlyOne(false);
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionGradSetDraw4_triggered()
{
    GradModel.Set_nDraws(4);
    GradModel.SetDrawOnlyOne(false);
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionGradSetDraw6_triggered()
{
    GradModel.Set_nDraws(6);
    GradModel.SetDrawOnlyOne(false);
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionGradSetDraw9_triggered()
{
    GradModel.Set_nDraws(9);
    GradModel.SetDrawOnlyOne(false);
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionGrad_SetDrawCount_Custom_triggered()
{
    bool isOk = false;
    int newDrawCount = QInputDialog::getInt(this, "Count of Viewports", "Input new count",
                          GradModel.Get_nDraw(), 1, 100, 1, &isOk);

    if (!isOk)
    {
        qDebug() << "Count of Viewports input canceled by user";
        return;
    }

    if (newDrawCount > 0 && newDrawCount <= 100)
    {
        GradModel.Set_nDraws(newDrawCount);
        GradModel.SetDrawOnlyOne(false);
        mainGLWidget->repaint();
    }
    else
    {
        QMessageBox::critical(this, "Incorrect input", "Invalid value for Count of Viewports");
    }
}
//-------------------------------------------------------------

void MainWindow::on_actionGradSwitch_Pespective_for_Current_triggered()
{
    if (WorkMode == WorkModeType::GradWork)
    {
        GradModel.SwitchPerspective();
        mainGLWidget->repaint();
    }
}
//-------------------------------------------------------------

void MainWindow::on_actionGradSwitch_Pespective_for_All_triggered()
{
    if (WorkMode == WorkModeType::GradWork)
    {
        GradModel.SwitchPerspectiveForAll();
        mainGLWidget->repaint();
    }
}
//-------------------------------------------------------------

void MainWindow::on_actionGradReset_View_Point_for_Current_triggered()
{
    GradModel.ResetViewPoint();
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionGradReset_View_Point_for_All_triggered()
{
    GradModel.ResetViewPointForAll();
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionGradChange_Population_size_triggered()
{
    DialogNewPopulationSize.InitDialog(GradModel.GetPopulationSize());

    if (DialogNewPopulationSize.exec() == QDialog::Accepted)
    {
        GradModel.CreatePopulation(DialogNewPopulationSize.GetNewPopulationSize());
        GradModel.MarkAsNotSaved();
        IsolinesTurnOff();
        this->repaint();
    }
    else
    {
        // Rejected
    }
}
//-------------------------------------------------------------

void MainWindow::on_actionGradSettings_triggered()
{
    DialogGradSettings.InitDialog(GradModel);

    if (DialogGradSettings.exec() == QDialog::Accepted)
    {
        DialogGradSettings.ReInitGradDesc(GradModel.ProtoGradDesc);
        DialogGradSettings.ReInitTargetFuncSettings(GradModel.TargetFuncSettingsGlobal);

        DialogGradSettings.ReInitGradModel(GradModel);

        GradModel.MarkAsNotSaved();
//        GradModel.SetGridSettings(ui->actionWorld_Show_Grid->ac)

        GradModel.ReCalcAreasByBounds();

        on_actionWorld_Show_Grid_triggered();

        on_actionWorld_Show_Isolines_of_Access_Rate_triggered();

        mainGLWidget->repaint();
    }
    else
    {
        // Rejected
    }
}
//-------------------------------------------------------------

void MainWindow::on_actionGradCalc_Bonds_triggered()
{
    GradModel.CalcBonds();
}
//-------------------------------------------------------------

void MainWindow::on_actionGradStart_Phase_1_triggered()
{   
    if (GradModel.GetIsGradCalculating())
        return;

    this->setWindowTitle(QApplication::applicationName() + " - Calculating Phase 1...");

    IsolinesTurnOff();

    GradModel.SetShowIsolinesOfAccessRate(false);

//    emit SignalStateToGradDesc();
    StateMachine.ToGradDesc();

    GradModel.StartGradDescent_Phase_1(mainGLWidget);

    mainGLWidget->repaint();
    UpdateCurNodeCoordsOnLabel();
    this->setWindowTitle(QApplication::applicationName());

//    emit SignalStateToNormal();
    StateMachine.ToNormal();
}
//-------------------------------------------------------------

void MainWindow::on_actionGradStart_Phase_1_for_Current_Config_triggered()
{
    if (GradModel.GetIsGradCalculating())
        return;

    this->setWindowTitle(QApplication::applicationName() + " - Calculating Phase 1 for Current Config... ");

//    emit SignalStateToGradDesc();
    IsolinesTurnOff();
    StateMachine.ToGradDesc();

    GradModel.StartGradDescent_Phase_1_for_Current(mainGLWidget);

    mainGLWidget->repaint();
    UpdateCurNodeCoordsOnLabel();
    this->setWindowTitle(QApplication::applicationName());

//    emit SignalStateToNormal();
    StateMachine.ToNormal();
}
//-------------------------------------------------------------

void MainWindow::on_actionGradRemove_Uncovered_triggered()
{
    GradModel.RemoveUncovered(mainGLWidget);
}
//-------------------------------------------------------------

void MainWindow::on_actionGradStart_Phase_2_triggered()
{
    if (GradModel.GetIsGradCalculating())
        return;

    this->setWindowTitle(QApplication::applicationName() + " - Calculating Phase 2...");

//    emit SignalStateToGradDesc();
    StateMachine.ToGradDesc();
    IsolinesTurnOff();

    GradModel.StartGradDescent_Phase_2(mainGLWidget);

    mainGLWidget->repaint();
    UpdateCurNodeCoordsOnLabel();
    this->setWindowTitle(QApplication::applicationName());

//    emit SignalStateToNormal();
    StateMachine.ToNormal();
}
//-------------------------------------------------------------

void MainWindow::on_actionGradStart_Phase_2_for_Current_Config_triggered()
{
    if (GradModel.GetIsGradCalculating())
        return;

    this->setWindowTitle(QApplication::applicationName() + " - Calculating Phase 1 for Current Config... ");

//    emit SignalStateToGradDesc();
    IsolinesTurnOff();
    StateMachine.ToGradDesc();

    GradModel.StartGradDescent_Phase_2_for_Current(mainGLWidget);

    mainGLWidget->repaint();
    UpdateCurNodeCoordsOnLabel();
    this->setWindowTitle(QApplication::applicationName());

//    emit SignalStateToNormal();
    StateMachine.ToNormal();
}
//-------------------------------------------------------------

void MainWindow::on_actionGradStop_triggered()
{
    GradModel.CancelGradDescent();

//    emit SignalStateToNormal();
    StateMachine.ToNormal();
}
//-------------------------------------------------------------

void MainWindow::on_actionGradShow_Tables_triggered()
{
    formGradGeneral->ShowTable(/*GradModel.GetConfigs()*/);
    formGradGeneral->show();
}
//-------------------------------------------------------------

void MainWindow::on_actionWorld_Show_Abonents_triggered()
{
    if (WorkMode != WorkModeType::GradWork)
        return;

    if (ui->actionWorld_Show_Routes->isChecked() == false)
    {
        ui->actionWorld_Show_Routes->setChecked(true);
        on_actionWorld_Show_Routes_triggered();
    }

    GradModel.SetIsDrawAbonents(true);
    formAboCalc.show();
//    SlotReceiveAboTime(0);

    mainGLWidget->repaint();

//    emit SignalStateToShowAbonents();
    StateMachine.ToShowAbonents();
}
//-------------------------------------------------------------

void MainWindow::on_actionWorld_Show_Grid_triggered()
{
    GradModel.SetShowGridOnRelief(ui->actionWorld_Show_Grid->isChecked());
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionWorld_Show_Cones_triggered()
{
    GradModel.SetShowCones(ui->actionWorld_Show_Cones->isChecked());
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionWorld_Show_Radii_triggered()
{
    GradModel.SetShowRadii(ui->actionWorld_Show_Radii->isChecked());
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionWorld_Show_Ellipses_triggered()
{
    GradModel.SetShowEllipses(ui->actionWorld_Show_Ellipses->isChecked());
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionWorld_Show_Lines_Between_SN_and_Points_triggered()
{
    GradModel.SetShowLinesBetweenSNandPoints(ui->actionWorld_Show_Lines_Between_SN_and_Points->isChecked());
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionWorld_Show_Points_on_Radii_triggered()
{
    GradModel.SetShowPointsOnRadii(ui->actionWorld_Show_Points_on_Radii->isChecked());
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionWorld_Show_Area_For_Random_Nodes_triggered()
{
    GradModel.SetShowAreaForRandomNodes(ui->actionWorld_Show_Area_For_Random_Nodes->isChecked());
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionWorld_Show_Area_For_Grad_Descent_triggered()
{
    GradModel.SetShowAreaForGradDesc(ui->actionWorld_Show_Area_For_Grad_Descent->isChecked());
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionWorld_Show_Isolines_of_Access_Rate_triggered()
{
    GradModel.SetShowIsolinesOfAccessRate(ui->actionWorld_Show_Isolines_of_Access_Rate->isChecked());
    SendLegendIsolines();
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionWorld_Show_Routes_triggered()
{
    GradModel.SetShowRoutes(ui->actionWorld_Show_Routes->isChecked());
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionRelief_Relief_Creator_triggered()
{
    formRelief.show();
}
//-------------------------------------------------------------

void MainWindow::on_actionEdit_Add_New_Route_triggered()
{
    if ( !GradModel.AddNewRoute() )
        return;

    WorldMode = WorldModeType::AddingRoutePoints;
//    emit SignalStateToAddNewRoute();
    StateMachine.ToAddNewRoute();
}
//-------------------------------------------------------------

void MainWindow::on_actionEdit_Finish_Route_triggered()
{
    GradModel.FinishRoute();
    GradModel.CreatePopulation(GradModel.GetPopulationSize());
    GradModel.MarkAsNotSaved();
    IsolinesTurnOff();

    WorldMode = WorldModeType::Nothing;
//    emit SignalStateToNormal();
    StateMachine.ToNormal();
}
//-------------------------------------------------------------

void MainWindow::on_actionEdit_Delete_Route_triggered()
{
    WorldMode = WorldModeType::DeletingRoute;
}
//-------------------------------------------------------------

void MainWindow::on_actionEdit_Select_Cur_Node_triggered()
{
    WorldMode = WorldModeType::SelectingSignalNode;
}
//-------------------------------------------------------------

void MainWindow::on_actionEdit_Editing_Pos_Cur_Node_triggered()
{
    if (ui->actionEdit_Editing_Pos_Cur_Node->isChecked())
    {
        if (GradModel.Get_iCurConfig() < 0 || GradModel.GetActiveConfig().Get_iCurNode() < 0)
        {
            ui->actionEdit_Editing_Pos_Cur_Node->setChecked(false);
            qDebug() << "Cur Node is not selected";
            return;
        }

        WorldMode = WorldModeType::EditingPosSignalNode;
        ui->actionEdit_Editing_Angle_Cur_Node->setChecked(false);
//        emit SignalStateToCurPosOrAngleEditing();
        StateMachine.ToCurPosOrAngleEditing();
        IsolinesTurnOff();
    }
    else
    {
        WorldMode = WorldModeType::Nothing;
//        emit SignalStateToNormal();
        StateMachine.ToNormal();
    }
}
//-------------------------------------------------------------

void MainWindow::on_actionEdit_Editing_Angle_Cur_Node_triggered()
{
    if (ui->actionEdit_Editing_Angle_Cur_Node->isChecked())
    {
        if (GradModel.Get_iCurConfig() < 0 || GradModel.GetActiveConfig().Get_iCurNode() < 0)
        {
            ui->actionEdit_Editing_Angle_Cur_Node->setChecked(false);
            qDebug() << "Cur Node is not selected";
            return;
        }

        WorldMode = WorldModeType::EditingAngleSignalNode;
        ui->actionEdit_Editing_Pos_Cur_Node->setChecked(false);
        StateMachine.ToCurPosOrAngleEditing();
        IsolinesTurnOff();
    }
    else
    {
        WorldMode = WorldModeType::Nothing;
        StateMachine.ToNormal();
    }
}
//-------------------------------------------------------------

void MainWindow::on_actionEdit_Edit_Signal_Nodes_for_All_triggered()
{
    DialogSignalNodesEdit.InitDialog_ForAll(GradModel.GetNodesType(), GradModel.GetSignalNodes());

    if (DialogSignalNodesEdit.exec() == QDialog::Accepted)
    {
        DialogSignalNodesEdit.ChangeSignalNodesParameters_ForAll(GradModel.GetNodesType(), GradModel.SignalNodesDirectAccess());

        GradModel.ApplySignalNodesToAllConfigs();
        IsolinesTurnOff();
        mainGLWidget->repaint();
    }
    else
    {
        // Rejected
    }
}
//-------------------------------------------------------------

void MainWindow::on_actionEdit_Edit_Signal_Nodes_for_Current_triggered()
{
    try
    {
        DialogSignalNodesEdit.InitDialog_ForCurrent(GradModel.GetNodesType(), GradModel.GetCurrentConfig().GetNodes(), GradModel.GetRelief() );
    }
    catch (std::exception &e)
    {
        QMessageBox::warning(this, "Warning", "There is not current config to change");
        return;
    }

    if (DialogSignalNodesEdit.exec() == QDialog::Accepted)
    {
        DialogSignalNodesEdit.ChangeSignalNodesParameters_ForCurrent(GradModel.GetNodesType(),
                                                                     GradModel.CurrentConfigAccess().NodesAccess(),
                                                                     GradModel.GetRelief());

        UpdateCurNodeCoordsOnLabel();
        IsolinesTurnOff();
        //GradModel.ApplySignalNodesToAllConfigs();
        mainGLWidget->repaint();
    }
    else
    {
        // Rejected
    }
}
//-------------------------------------------------------------

void MainWindow::on_actionEdit_Edit_Routes_triggered()
{
    DialogRoutesEdit.InitDialog(GradModel.GetRoutes());

    if (DialogRoutesEdit.exec() == QDialog::Accepted)
    {
        DialogRoutesEdit.ChangeRoutes(GradModel.RoutesDirectAccess());

        GradModel.ApplyRoutesToAllConfigs(NeedToSave::Need);
//        IsolinesTurnOff();
        mainGLWidget->repaint();
    }
    else
    {
        // Rejected
    }
}
//-------------------------------------------------------------

void MainWindow::on_actionEdit_Change_Count_of_Nodes_triggered()
{
    int curCount = GradModel.GetSignalNodes().size();
    bool isOk = false;

    int newCount = -777;

    newCount = QInputDialog::getInt(this, "Count of Nodes", "Input new count",
                         curCount, 1, 100, 1, &isOk);

    qDebug() << newCount;

    if (newCount == curCount)
    {
        return;
    }

    if (!isOk)
    {
        QMessageBox::critical(this, "Error", "Incorrect Count of Nodes");
        return;
    }

    GradModel.SignalNodesDirectAccess().resize(newCount);

    GradModel.ApplySignalNodesToAllConfigs();
    IsolinesTurnOff();

    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionEdit_Apply_Cur_Node_to_All_Configs_triggered()
{
    GradModel.ApplyCurNodeFromCurConfigToAllConfigs();
    IsolinesTurnOff();
}
//-------------------------------------------------------------

void MainWindow::on_actionDebug_TwoLines_triggered()
{
    GradModel.TestTwoLines();
}
//-------------------------------------------------------------

void MainWindow::on_actionDebug_Get_Last_Cost_For_Current_triggered()
{
    GradModel.TestGetLastCostForCurrent();
}
//-------------------------------------------------------------

void MainWindow::on_actionDebug_Calc_Access_Rate_for_current_triggered()
{
    GradModel.CalcAccessRateForCurrent();
}
//-------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent *event)
{
//    if (event->spontaneous())
//    {
//        QMessageBox::information(this, "test spontaneous", "test spontaneous");
//    }

    if ( !GradModel.SaveGlobalSettings() )
    {
        qDebug() << "Warning! GlobalSettings not Saved!";
    }

    formAboCalc.close();

    if (CheckIsSavedAndSaveIfNecessary())
        event->accept();
    else
        event->ignore();
}
//-------------------------------------------------------------

bool MainWindow::CheckIsSavedAndSaveIfNecessary()
{
    if ( !GradModel.GetIsSaved() )
    {
        auto res = QMessageBox::question(this, "Question",
                                         "Grad Config file is not saved. Would you like to save it?",
                                         QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (res == QMessageBox::Yes)
        {
            on_actionFileSave_Grad_Config_triggered(); // try save
            return IsGradDescFileSavedSuccessfully;

        }
        else if (res == QMessageBox::No)
            return true;
        else if (res == QMessageBox::Cancel)
            return false;
        else
        {
            QMessageBox::critical(this, "Error", "Something wrong with QMessageBox::question result");
            return false;
        }
    }
    else
        return true;
}
//-------------------------------------------------------------

void MainWindow::UpdateCurNodeCoordsOnLabel()
{
    try
    {
        int n = GradModel.GetCurrentConfig().Get_iCurNode();
        const auto & sn = GradModel.GetCurrentConfig().GetCurNode();
        SlotReceiveNodeCoords(n, sn.Pos.x(), sn.Pos.y(), sn.Pos.z());
    }
    catch (std::exception & e)
    {
//        qDebug() << "catch (std::exception & e) in MainWindow::on_actionEdit_Edit_Signal_Nodes_for_Current_triggered()";
//        qDebug() << "e.what() =" << e.what();
        lblCoord->setText("n/a");
    }
}
//-------------------------------------------------------------

void MainWindow::TryToSaveGradDescToFile()
{
    if ( GradModel.SaveToFile() )
    {
        IsGradDescFileSavedSuccessfully = true;
    }
    else
    {
        IsGradDescFileSavedSuccessfully = false;
        QMessageBox::critical(this, "Error", "Failure to save GradDesc File");
        return;
    }
}
//-------------------------------------------------------------

void MainWindow::SlotReceiveWorldCoords(double wx, double wy, double wz, bool wExists)
{
//    if (wExists) // Пытаемся отловить ошибку, когда в интерполяцию рельефа залетает nan
//    {
////        if (wx == std::numeric_limits<double>::quiet_NaN() ||
////            wx == std::numeric_limits<double>::signaling_NaN() )
//        if (std::isnan(wx))
//        {
//            throw std::logic_error("wx == nan in SlotReceiveWorldCoords");
//        }
//        if (std::isnan(wy))
//        {
//            throw std::logic_error("wy == nan in SlotReceiveWorldCoords");
//        }
//        if (std::isnan(wz))
//        {
//            throw std::logic_error("wz == nan in SlotReceiveWorldCoords");
//        }
//    }

    lblGlX->setText("Gl X = " + (wExists?QString().setNum(wx):"n/a"));
    lblGlY->setText("Gl Y = " + (wExists?QString().setNum(wy):"n/a"));
    lblGlZ->setText("Gl Z = " + (wExists?QString().setNum(wz):"n/a"));

    double realX, realY, realZ;
//    if (std::isnan(wy))   // Пытаемся отловить ошибку, когда в интерполяцию рельефа залетает nan
//    {
//        throw std::logic_error("wy == nan in SlotReceiveWorldCoords");
//    }

    if (!std::isnan(wx) && !std::isnan(wy) && !std::isnan(wy))   //  Тпереь должно работать
    {                                                // Если всё будет работать, то удалить все подобные ловушки
        realZ = GradModel.GetRelief().CalcRealXYZbyNormXY(wx, wy, realX, realY);

        lblWorldX->setText("WorldX = " + (wExists?QString().setNum(realX):"n/a") + " m");
        lblWorldY->setText("WorldY = " + (wExists?QString().setNum(realY):"n/a") + " m");
        lblWorldZ->setText("WorldZ = " + (wExists?QString().setNum(realZ):"n/a") + " m");
    }
}
//-------------------------------------------------------------

void MainWindow::SlotReceiveRouteDeleted(bool isDeleted)
{
    if (isDeleted)
    {
        GradModel.CreatePopulation(GradModel.GetPopulationSize());
        GradModel.MarkAsNotSaved();
        IsolinesTurnOff();
    }
    else
        qDebug() << "Warning: SlotReceiveRouteDeleted: isDeleted == false";

//    if (ui->actionWorld_Show_Coords->isChecked())
//        WorldMode = WorldModeType::ShowCoords;
//    else

    WorldMode = WorldModeType::Nothing;
}
//-------------------------------------------------------------

void MainWindow::SlotReceiveAboTime(int t, TargetFuncTypeEnum funcType) // in sec
{
//    qDebug() << "t =" << t << " - " << t/3600.0;

    if (WorkMode == WorkModeType::GradWork)
    {
        GradModel.CalcAbonentsPos(t);
//        GradModel.ApplyRoutesToAllConfigs(NeedToSave::DoNotNeed);
        GradModel.ApplyAbonentsPosInRoutesToAllConfigs();
        GradModel.ReCalcAboAccessRate(funcType);
        mainGLWidget->repaint();
    }
}
//-------------------------------------------------------------

void MainWindow::SlotReceiveFormAboCalcClose()
{
    dlgAboReport.InitDialog(); // ?

    GradModel.SetIsDrawAbonents(false);
    mainGLWidget->repaint();

//    emit SignalStateToNormal();
    StateMachine.ToNormal();
}
//-------------------------------------------------------------

void MainWindow::SlotReceiveAddTimePointToReport(int t)
{
    dlgAboReport.AddTimePoint(t);
}
//-------------------------------------------------------------

void MainWindow::SlotReceiveShowAboReport(TargetFuncTypeEnum funcType)
{
    try
    {
        dlgAboReport.CalcTable(GradModel.GetActiveConfig(),
                               GradModel.TargetFuncSettingsGlobal.IsUseLineBetweenTwoPoints,
                               GradModel.GetNodesType(),
                               funcType);

        dlgAboReport.setWindowTitle
                (QString("iActiveConfig = %1").arg(GradModel.Get_iCurConfig()));

        dlgAboReport.exec();
    }
    catch (const std::out_of_range &e)
    {
        QMessageBox::critical(this, "Error", QString("Active config is not selected\n") + e.what());
        return;
    }
}
//-------------------------------------------------------------

void MainWindow::SendLegendIsolines()
{
    try
    {
        const auto & li = GradModel.GetCurrentConfig().GetLegendIsolines();
        formLegendIsolines.ReceiveLegendIsolines(li);
    }
    catch (std::exception & e)
    {
        // nothing
    }
}
//-------------------------------------------------------------

void MainWindow::SlotReceive_iCurConfigChanged(int i)
{
    UpdateCurNodeCoordsOnLabel();

    lbl_iCurConfig->setText("iCurConfig: " + QString().setNum(i));

    SendLegendIsolines();
}
//-------------------------------------------------------------

void MainWindow::SlotReceiveNodeCoords(int n, double x, double y, double z)
{
    lblCoord->setText("Node " + QString().setNum(n) + ": {" +
                        QString().setNum(x) + "; " +
                        QString().setNum(y) + "; " +
                        QString().setNum(z) + "}");
}
//-------------------------------------------------------------

void MainWindow::Init_UI_AccordingGlobalSettings()
{
    ui->actionWorld_Show_Grid->setChecked(GradModel.GetWhatShow().ShowGrid);
    ui->actionWorld_Show_Cones->setChecked(GradModel.GetWhatShow().ShowCones);
    ui->actionWorld_Show_Ellipses->setChecked(GradModel.GetWhatShow().ShowEllipses);
    ui->actionWorld_Show_Radii->setChecked(GradModel.GetWhatShow().ShowRadii);
    ui->actionWorld_Show_Lines_Between_SN_and_Points->setChecked(GradModel.GetWhatShow().ShowLinesBetweenSNandPoints);
    ui->actionWorld_Show_Area_For_Random_Nodes->setChecked(GradModel.GetWhatShow().ShowAreaForRandomNodes);
    ui->actionWorld_Show_Area_For_Grad_Descent->setChecked(GradModel.GetWhatShow().ShowAreaForGradDesc);
    ui->actionWorld_Show_Points_on_Radii->setChecked(GradModel.GetWhatShow().ShowPointsOnRadii);

    ui->actionWorld_Show_Isolines_of_Access_Rate->setChecked(GradModel.GetWhatShow().ShowIsolinesOfAccessRate);

    ui->actionWorld_Show_Routes->setChecked(GradModel.GetWhatShow().ShowRoutes);
}
//-------------------------------------------------------------

void MainWindow::on_actionWorld_Show_Legend_Isolines_triggered()
{
    formLegendIsolines.show();
}
//-------------------------------------------------------------

void MainWindow::on_actionEdit_Set_Random_Node_Coords_triggered()
{
    if (WorkMode == WorkModeType::GradWork)
    {
        GradModel.SetRandomNodeCoords();
        mainGLWidget->repaint();
    }
}
//-------------------------------------------------------------

void MainWindow::on_actionEdit_Set_Node_Coords_by_Peaks_triggered()
{
    if (WorkMode == WorkModeType::GradWork)
    {
        GradModel.SetNodeCoordsByPeaks();
        mainGLWidget->repaint();
    }
}
//-------------------------------------------------------------


