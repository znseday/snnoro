#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Model.h"
#include "View.h"

#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

#include <limits>

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

    lblGlX->setFixedWidth(120);
    lblGlY->setFixedWidth(120);
    lblGlZ->setFixedWidth(120);
    lblWorldX->setFixedWidth(150);
    lblWorldY->setFixedWidth(150);
    lblWorldZ->setFixedWidth(150);

    ui->statusbar->addWidget(lblGlX);
    ui->statusbar->addWidget(lblGlY);
    ui->statusbar->addWidget(lblGlZ);
    ui->statusbar->addWidget(lblWorldX);
    ui->statusbar->addWidget(lblWorldY);
    ui->statusbar->addWidget(lblWorldZ);

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

    mainGLWidget->setFocus();

    formGeneral = new FormGeneral();
    formGeneral->resize(1100, 500);
    QObject::connect(&Model::Instance(), SIGNAL(signalSendGeneralData(ExportGeneralData)),
                     formGeneral, SLOT(slotReceiveGeneralData(ExportGeneralData)));


    for (size_t i = 0; i < 4; ++i)
    {
        formConfigInfo[i] = new FormConfigInfo();
        formConfigInfo[i]->resize(1350, 500);
        formConfigInfo[i]->Set_nConfig(i);

        QObject::connect(&Model::Instance(), SIGNAL(signalSendDetailedInfo(ExportDetailedInfo)),
                         formConfigInfo[i], SLOT(slotReceiveDetailedInfo(ExportDetailedInfo)));

    }

    formGradGeneral = new FormGradGeneral(GradModel.GetConfigs());

    formGradGeneral->setWindowTitle("Grad descent Results");

    //on_actionFileStart_Grad_Descent_triggered();


    connect(&formAboCalc, SIGNAL(SignalSendAboTime(int)),
            this, SLOT(SlotReceiveAboTime(int)));

    mainGLWidget->setMouseTracking(true);
}
//-------------------------------------------------------------

void MainWindow::AfterShow()
{
    //on_actionFileOpen_Grad_Descent_triggered();
    on_actionWorld_Show_Coords_toggled(true);
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
    if (formGeneral)
        delete formGeneral;

    for (size_t i = 0; i < 4; ++i)
        if (formConfigInfo[i])
            delete formConfigInfo[i];

    delete ui;
}
//-------------------------------------------------------------

void MainWindow::on_actionFileExit_triggered()
{
    QApplication::closeAllWindows();

//    this->close();
}
//-------------------------------------------------------------

void MainWindow::on_actionFileStart_Old_triggered()
{
    // Запустить всю историю вместо main

    Model::Instance().start();

    View& view = View::Instance();
    view.initialize();
    view.setModel(&Model::Instance());
    view.reshape(GetGLWidth(), GetGLHeight());
    view.update();

    WorkMode = WorkModeType::OldWork;

    mainGLWidget->StartMainTimer();
    ui->actionFileStart_Old->setEnabled(false);
    ui->actionFileOpen_Grad_Descent->setEnabled(false);
    ui->actionFileNew_Grad_Config->setEnabled(false);

    ui->actionOldTablesGeneral->setEnabled(true);
    ui->actionOldTablesConfig_0->setEnabled(true);
    ui->actionOldTablesConfig_1->setEnabled(true);
    ui->actionOldTablesConfig_2->setEnabled(true);
    ui->actionOldTablesConfig_3->setEnabled(true);

    ui->menuGrad->setEnabled(false);
    ui->menuEdit->setEnabled(false);
    ui->menuWorld->setEnabled(false);
}
//-------------------------------------------------------------

void MainWindow::on_actionOldTablesGeneral_triggered()
{
    // Отобразить общую таблицу

    formGeneral->ShowTable();
    formGeneral->show();
}
//-------------------------------------------------------------

void MainWindow::on_actionOldTablesConfig_0_triggered()
{
    // Отобразить детализированную таблицу для конфигурации 0 
    formConfigInfo[0]->ShowTable();
    formConfigInfo[0]->show();
}

void MainWindow::on_actionOldTablesConfig_1_triggered()
{
    // Отобразить детализированную таблицу для конфигурации 1
    formConfigInfo[1]->ShowTable();
    formConfigInfo[1]->show();
}

void MainWindow::on_actionOldTablesConfig_2_triggered()
{
    // Отобразить детализированную таблицу для конфигурации 2
    formConfigInfo[2]->ShowTable();
    formConfigInfo[2]->show();
}

void MainWindow::on_actionOldTablesConfig_3_triggered()
{
    // Отобразить детализированную таблицу для конфигурации 3
    formConfigInfo[3]->ShowTable();
    formConfigInfo[3]->show();
}
//-------------------------------------------------------------

void MainWindow::on_actionFileOpen_Grad_Descent_triggered()
{
    // Решение статической задачи градиентном спуском
    //GradModel.SetWidthAndHeight(this->width(), this->height());

    if (!CheckIsSavedAndSaveIfNecessary())
        return;

    QString fileName = QFileDialog::getOpenFileName(this,
        "Open Config file", ".", "Config Files (*.json)");

    if (!GradModel.LoadFromFile(fileName))
    {
        qDebug() << (fileName + " not found");
        return;
    }

    WorkMode = WorkModeType::GradWord;
    ui->actionFileStart_Old->setEnabled(false);

    ui->actionFileSave_Grad_Config->setEnabled(true);
    ui->actionFileSave_Grad_Config_As->setEnabled(true);

    ui->actionEdit_Delete_Route->setEnabled(true);
    ui->actionEdit_Edit_Signal_Nodes_for_All->setEnabled(true);
    ui->actionEdit_Edit_Signal_Nodes_for_Current->setEnabled(true);

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
    if (WorkMode == WorkModeType::GradWord)
    {
        GradModel.SwitchDrawOnlyOne();
        mainGLWidget->repaint();
    }
    else if (WorkMode == WorkModeType::OldWork)
    {
        if (Model::Instance().nCView > 1)
        {
            Model::Instance().nCView = 1;
        }
        else // 1
        {
            Model::Instance().nCView = 4;
        }
    }
}
//-------------------------------------------------------------

void MainWindow::on_actionGradSwitch_Pespective_for_Current_triggered()
{
    if (WorkMode == WorkModeType::GradWord)
    {
        GradModel.SwitchPerspective();
        mainGLWidget->repaint();
    }
    else if (WorkMode == WorkModeType::OldWork)
    {
        Model::Instance().fDrawCfg3d = !Model::Instance().fDrawCfg3d;
    }
}
//-------------------------------------------------------------

void MainWindow::on_actionGradSwitch_Pespective_for_All_triggered()
{
    if (WorkMode == WorkModeType::GradWord)
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
//    DialogGradSettings.show();
//    DialogGradSettings.exec();

    DialogGradSettings.InitDialog(GradModel);

    if (DialogGradSettings.exec() == QDialog::Accepted)
    {
        DialogGradSettings.ReInitGradDesc(GradModel.ProtoGradDesc);
        DialogGradSettings.ReInitTargetFuncSettings(GradModel.TargetFuncSettings);

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
    //QMessageBox::information(this, "on_actionWorld_Show_Coords_toggled", QString().setNum(_toggled));

//    mainGLWidget->setMouseTracking(true);
    mainGLWidget->SetIsShowCoordsAlways(_toggled);

//    if (_toggled)
//    {
//        mainGLWidget->setMouseTracking(true);
////        if (WorldMode == WorldModeType::Nothing)
////            WorldMode = WorldModeType::ShowCoords;
//    }
//    else
//    {
//        mainGLWidget->setMouseTracking(true);
    //    }
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
//        if (wy == std::numeric_limits<double>::quiet_NaN() ||
//            wy == std::numeric_limits<double>::signaling_NaN() )
        if (std::isnan(wy))
        {
            throw std::logic_error("wy == nan in SlotReceiveWorldCoords");
        }
//        if (wz == std::numeric_limits<double>::quiet_NaN() ||
//            wz == std::numeric_limits<double>::signaling_NaN() )
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

//    if ( !GradModel.SaveToFile() )
//    {
//        QMessageBox::critical(this, "Error", "Failure to save GradDesc File");
//        return;
//    }
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
//    WorkMode = WorkModeType::GradWord;
    ui->actionFileStart_Old->setEnabled(false);

    if (!CheckIsSavedAndSaveIfNecessary())
        return;

//    GradModel.NewGradModelBulk();
//    GradModel.SetWidthAndHeight(mainGLWidget->width(), mainGLWidget->height());

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
        // Rejected
        return;
    }

    WorkMode = WorkModeType::GradWord;
    GradModel.MarkAsNotSaved();

    ui->actionFileSave_Grad_Config->setEnabled(true);
    ui->actionFileSave_Grad_Config_As->setEnabled(true);

    ui->actionEdit_Delete_Route->setEnabled(true);
    ui->actionEdit_Edit_Signal_Nodes_for_All->setEnabled(true);
    ui->actionEdit_Edit_Signal_Nodes_for_Current->setEnabled(true);

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
    //
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

        GradModel.ApplyRoutesToAllConfigs();
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
    if (WorkMode != WorkModeType::GradWord)
        return;

    // to do ???

    formAboCalc.show();


    // somewhere flag = ui->actionShow_Abonents->isChecked()

//    mainGLWidget->repaint();
}
//-------------------------------------------------------------

void MainWindow::SlotReceiveAboTime(int t) // in sec
{
    qDebug() << "t =" << t << " - " << t/3600.0;
}

