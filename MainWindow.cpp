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

    lblGlX->setFixedWidth(120);
    lblGlY->setFixedWidth(120);
    lblGlZ->setFixedWidth(120);
    lblWorldX->setFixedWidth(150);
    lblWorldY->setFixedWidth(150);
    lblWorldZ->setFixedWidth(150);
    lbl_iCurConfig->setFixedWidth(120);

    ui->statusbar->addWidget(lblGlX);
    ui->statusbar->addWidget(lblGlY);
    ui->statusbar->addWidget(lblGlZ);
    ui->statusbar->addWidget(lblWorldX);
    ui->statusbar->addWidget(lblWorldY);
    ui->statusbar->addWidget(lblWorldZ);
    ui->statusbar->addWidget(lbl_iCurConfig);

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


    connect(&formAboCalc, SIGNAL(SignalSendAboTime(int)),
            this, SLOT(SlotReceiveAboTime(int)));

    connect(&formAboCalc, SIGNAL(SignalFormAboCalcClose()),
            this, SLOT(SlotReceiveFormAboCalcClose()));


    connect(&formAboCalc, SIGNAL(SignalAddTimePointToReport(int)),
            this, SLOT(SlotReceiveAddTimePointToReport(int)));

    connect(&formAboCalc, SIGNAL(SignalShowAboReport()),
            this, SLOT(SlotReceiveShowAboReport()));

    mainGLWidget->setMouseTracking(true);

    dlgAboReport.InitDialog();

//    on_actionFileStart_Grad_Descent_triggered();

//    on_actionFileOpen_Grad_Descent_triggered();
}
//-------------------------------------------------------------

void MainWindow::AfterShow()
{
    //on_actionFileOpen_Grad_Descent_triggered();
    on_actionWorld_Show_Coords_toggled(true);

    on_actionFileOpen_Grad_Descent_triggered();
}
//-------------------------------------------------------------

int MainWindow::GetGLWidth() const
{
    return mainGLWidget->width();
}

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

void MainWindow::on_actionFileExit_triggered()
{
    QApplication::closeAllWindows();
//    this->close();
}
//-------------------------------------------------------------

void MainWindow::on_actionFileOpen_Grad_Descent_triggered()
{
    if (!CheckIsSavedAndSaveIfNecessary())
        return;

    QString fileName = QFileDialog::getOpenFileName(this,
        "Open Config file", ".", "Config Files (*.json)");


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

    ui->actionFileSave_Grad_Config->setEnabled(true);
    ui->actionFileSave_Grad_Config_As->setEnabled(true);

    ui->actionEdit_Delete_Route->setEnabled(true);
    ui->actionEdit_Edit_Signal_Nodes_for_All->setEnabled(true);
    ui->actionEdit_Edit_Signal_Nodes_for_Current->setEnabled(true);
    ui->actionEdit_Change_Count_of_Nodes->setEnabled(true);

    ui->actionEdit_Edit_Routes->setEnabled(true);
    ui->actionWorld_Show_Abonents->setEnabled(true);

    GradModel.SetWidthAndHeight(mainGLWidget->width(), mainGLWidget->height());

    GradModel.Set_nDraws(35);
    mainGLWidget->repaint();
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

void MainWindow::on_actionGradSwitch_Show_One_All_triggered()
{
    if (WorkMode == WorkModeType::GradWork)
    {
        GradModel.SwitchDrawOnlyOne();
        mainGLWidget->repaint();
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

void MainWindow::on_actionGradSetDraw4_triggered()
{
    GradModel.Set_nDraws(4);
    GradModel.SetDrawOnlyOne(false);
    mainGLWidget->repaint();
}

void MainWindow::on_actionGradSetDraw6_triggered()
{
    GradModel.Set_nDraws(6);
    GradModel.SetDrawOnlyOne(false);
    mainGLWidget->repaint();
}

void MainWindow::on_actionGradSetDraw9_triggered()
{
    GradModel.Set_nDraws(9);
    GradModel.SetDrawOnlyOne(false);
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionGradStart_Phase_1_triggered()
{   
    this->setWindowTitle(QApplication::applicationName() + " - Calculating Phase 1... ");
    ui->actionGradStop->setEnabled(true);
    GradModel.StartGradDescent_Phase_1(mainGLWidget);

    mainGLWidget->repaint();
    this->setWindowTitle(QApplication::applicationName());
}
//-------------------------------------------------------------

void MainWindow::on_actionGradStop_triggered()
{
    GradModel.CancelGradDescent();
    ui->actionGradStop->setEnabled(false);
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

        mainGLWidget->repaint();
    }
    else
    {
        // Rejected
    }
}
//-------------------------------------------------------------

void MainWindow::on_actionGradStart_Phase_2_triggered()
{
    this->setWindowTitle(QApplication::applicationName() + " - Calculating Phase 2...");
    ui->actionGradStop->setEnabled(true);
    GradModel.StartGradDescent_Phase_2(mainGLWidget);

    mainGLWidget->repaint();
    this->setWindowTitle(QApplication::applicationName());
}
//-------------------------------------------------------------

void MainWindow::on_actionGradRemove_Uncovered_triggered()
{
    GradModel.RemoveUncovered(mainGLWidget);
}
//-------------------------------------------------------------

void MainWindow::on_actionGradChange_Population_size_triggered()
{
    DialogNewPopulationSize.InitDialog(GradModel.GetPopulationSize());

    if (DialogNewPopulationSize.exec() == QDialog::Accepted)
    {       
        GradModel.CreatePopulation(DialogNewPopulationSize.GetNewPopulationSize());
        GradModel.MarkAsNotSaved();
        this->repaint();
    }
    else
    {
        // Rejected
    }
}
//-------------------------------------------------------------

void MainWindow::on_actionShow_Tables_triggered()
{
    formGradGeneral->ShowTable(/*GradModel.GetConfigs()*/);
    formGradGeneral->show();
}
//-------------------------------------------------------------

void MainWindow::on_actionGradCalc_Bonds_triggered()
{
    GradModel.CalcBonds();
}
//-------------------------------------------------------------

void MainWindow::on_actionRelief_Relief_Creator_triggered()
{
    formRelief.show();
}
//-------------------------------------------------------------

void MainWindow::on_actionFileSave_Grad_Config_triggered()
{
    if ( GradModel.GetFileName() == "" )
    {
        QString fileName = QFileDialog::getSaveFileName(this,
            "Save Config file", ".", "Config Files (*.json)");

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

void MainWindow::on_actionWorld_Show_Coords_toggled(bool _toggled)
{
    mainGLWidget->SetIsShowCoordsAlways(_toggled);
}
//-------------------------------------------------------------

void MainWindow::SlotReceiveWorldCoords(double wx, double wy, double wz, bool wExists)
{
    if (wExists) // Пытаемся отловить ошибку, когда в интерполяцию рельефа залетает nan
    {
//        if (wx == std::numeric_limits<double>::quiet_NaN() ||
//            wx == std::numeric_limits<double>::signaling_NaN() )
        if (std::isnan(wx))
        {
            throw std::logic_error("wx == nan in SlotReceiveWorldCoords");
        }
        if (std::isnan(wy))
        {
            throw std::logic_error("wy == nan in SlotReceiveWorldCoords");
        }
        if (std::isnan(wz))
        {
            throw std::logic_error("wz == nan in SlotReceiveWorldCoords");
        }
    }

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
    }
    else
        qDebug() << "Warning: SlotReceiveRouteDeleted: isDeleted == false";

    if (ui->actionWorld_Show_Coords->isChecked())
        WorldMode = WorldModeType::ShowCoords;
    else
        WorldMode = WorldModeType::Nothing;
}
//-------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent *event)
{
//    if (event->spontaneous())
//    {
//        QMessageBox::information(this, "test spontaneous", "test spontaneous");
//    }

    formAboCalc.close();

    if ( !GradModel.GetIsSaved() )
    {
        auto res = QMessageBox::question(this, "Question",
                                         "Grad Config file is not saved. Would you like to save it?",
                                         QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (res == QMessageBox::Yes)
        {
            // try save
            on_actionFileSave_Grad_Config_triggered();
            if (IsGradDescFileSavedSuccessfully)
            {
                event->accept();
                return;
            }
            else
            {
                event->ignore();
                return;
            }
        }
        else if (res == QMessageBox::No)
        {
            event->accept();
            return;
        }
        else if (res == QMessageBox::Cancel)
        {
            event->ignore();
            return;
        }
        else
        {
            QMessageBox::critical(this, "Error", "Something wrong with QMessageBox::question result");
            event->accept();
        }
    }
    else
    {
        event->accept();
    }
}
//-------------------------------------------------------------

void MainWindow::on_actionEdit_Add_New_Route_triggered()
{
    if ( !GradModel.AddNewRoute() )
        return;

    WorldMode = WorldModeType::AddingRoutePoints;

    ui->actionEdit_Finish_Route->setEnabled(true);
    ui->actionEdit_Add_New_Route->setEnabled(false);
}
//-------------------------------------------------------------

void MainWindow::on_actionEdit_Finish_Route_triggered()
{
    GradModel.FinishRoute();
    GradModel.CreatePopulation(GradModel.GetPopulationSize());
    GradModel.MarkAsNotSaved();

    ui->actionEdit_Finish_Route->setEnabled(false);
    ui->actionEdit_Add_New_Route->setEnabled(true);

    if (ui->actionWorld_Show_Coords->isChecked())
        WorldMode = WorldModeType::ShowCoords;
    else
        WorldMode = WorldModeType::Nothing;
}
//-------------------------------------------------------------

void MainWindow::on_actionFileSave_Grad_Config_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Config file", ".", "Config Files (*.json)");

    if (fileName == "")
    {
        qDebug() << "Warning: File name is not set. Abort saving.";
        return;
    }
    GradModel.ChangeFileName(fileName);

    TryToSaveGradDescToFile();
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
            // try save
            on_actionFileSave_Grad_Config_triggered();
            if (IsGradDescFileSavedSuccessfully)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (res == QMessageBox::No)
        {
            return true;
        }
        else if (res == QMessageBox::Cancel)
        {
            return false;
        }
        else
        {
            QMessageBox::critical(this, "Error", "Something wrong with QMessageBox::question result");
            return false;
        }
    }
    else
    {
        return true;
    }
}
//-------------------------------------------------------------

void MainWindow::on_actionFileNew_Grad_Config_triggered()
{
    if (!CheckIsSavedAndSaveIfNecessary())
        return;

    // Здесь сделать что-нибудь еще: новая конфигурация
    //DialogGradConfigNew.InitDialog(GradModel);

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

    ui->actionFileSave_Grad_Config->setEnabled(true);
    ui->actionFileSave_Grad_Config_As->setEnabled(true);

    ui->actionEdit_Delete_Route->setEnabled(true);
    ui->actionEdit_Edit_Signal_Nodes_for_All->setEnabled(true);
    ui->actionEdit_Edit_Signal_Nodes_for_Current->setEnabled(true);
    ui->actionEdit_Change_Count_of_Nodes->setEnabled(true);

    ui->actionEdit_Edit_Routes->setEnabled(true);
    ui->actionWorld_Show_Abonents->setEnabled(true);

    GradModel.Set_nDraws(35);
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionEdit_Delete_Route_triggered()
{
    WorldMode = WorldModeType::DeletingRoute;
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

void MainWindow::on_actionEdit_Edit_Signal_Nodes_for_All_triggered()
{
    DialogSignalNodesEdit.InitDialog_ForAll(GradModel.GetNodesType(), GradModel.GetSignalNodes());

    if (DialogSignalNodesEdit.exec() == QDialog::Accepted)
    {
        DialogSignalNodesEdit.ChangeSignalNodesParameters_ForAll(GradModel.GetNodesType(), GradModel.SignalNodesDirectAccess());

        GradModel.ApplySignalNodesToAllConfigs();
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
        DialogSignalNodesEdit.ChangeSignalNodesParameters_ForCurrent(GradModel.GetNodesType(), GradModel.CurrentConfigAccess().NodesAccess(), GradModel.GetRelief());

        //GradModel.ApplySignalNodesToAllConfigs();
        mainGLWidget->repaint();
    }
    else
    {
        // Rejected
    }
}
//-------------------------------------------------------------

void MainWindow::on_actionTwoLines_triggered()
{
    GradModel.TestTwoLines();
}
//-------------------------------------------------------------

void MainWindow::on_actionGradStart_Phase_1_for_Current_Config_triggered()
{
    this->setWindowTitle(QApplication::applicationName() + " - Calculating Phase 1 for Current Config... ");
    ui->actionGradStop->setEnabled(true);
    GradModel.StartGradDescent_Phase_1_for_Current(mainGLWidget);

    mainGLWidget->repaint();
    this->setWindowTitle(QApplication::applicationName());
}
//-------------------------------------------------------------

void MainWindow::on_actionGradStart_Phase_2_for_Current_Config_triggered()
{
    this->setWindowTitle(QApplication::applicationName() + " - Calculating Phase 1 for Current Config... ");
    ui->actionGradStop->setEnabled(true);
    GradModel.StartGradDescent_Phase_2_for_Current(mainGLWidget);

    mainGLWidget->repaint();
    this->setWindowTitle(QApplication::applicationName());
}
//-------------------------------------------------------------

void MainWindow::on_actionEdit_Edit_Routes_triggered()
{
    DialogRoutesEdit.InitDialog(GradModel.GetRoutes());

    if (DialogRoutesEdit.exec() == QDialog::Accepted)
    {
        DialogRoutesEdit.ChangeRoutes(GradModel.RoutesDirectAccess());

        GradModel.ApplyRoutesToAllConfigs(NeedToSave::Need);
        mainGLWidget->repaint();
    }
    else
    {
        // Rejected
    }
}
//-------------------------------------------------------------

void MainWindow::on_actionWorld_Show_Abonents_triggered()
{
    if (WorkMode != WorkModeType::GradWork)
        return;


    GradModel.SetIsDrawAbonents(true);
    formAboCalc.show();
//    SlotReceiveAboTime(0);

    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::SlotReceiveAboTime(int t) // in sec
{
    qDebug() << "t =" << t << " - " << t/3600.0;

    if (WorkMode == WorkModeType::GradWork)
    {
        GradModel.CalcAbonentsPos(t);
        GradModel.ApplyRoutesToAllConfigs(NeedToSave::DoNotNeed);
        GradModel.ReCalcAboAccessRate();
        mainGLWidget->repaint();
    }
}
//-------------------------------------------------------------

void MainWindow::SlotReceiveFormAboCalcClose()
{
    dlgAboReport.InitDialog(); // ?

    GradModel.SetIsDrawAbonents(false);
    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::SlotReceiveAddTimePointToReport(int t)
{
    dlgAboReport.AddTimePoint(t);
}
//-------------------------------------------------------------

void MainWindow::SlotReceiveShowAboReport()
{
    try
    {
        dlgAboReport.CalcTable(GradModel.GetActiveConfig(), GradModel.TargetFuncSettingsGlobal.IsUseLineBetweenTwoPoints,
                               GradModel.GetNodesType());

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

void MainWindow::SlotReceive_iCurConfigChanged(int i)
{
    lbl_iCurConfig->setText("iCurConfig: " + QString().setNum(i));
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

    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::on_actionDebug_Get_Last_Cost_For_Current_triggered()
{
    GradModel.TestGetLastCostForCurrent();
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




