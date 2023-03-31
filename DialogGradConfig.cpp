#include "DialogGradConfig.h"
#include "ui_DialogGradConfig.h"

#include <QString>
#include <QMessageBox>
#include <QFileDialog>

#include "GradModel.h"
#include "GradDescSaveLoad.h"

void DialogGradConfig::InitTargetFuncSettingsDialog(const TargetFuncSettingsStruct &_targetFuncSettings)
{
    ui->EditAarf->setText(QString().setNum(_targetFuncSettings.Aarf));
    ui->EditA2->setText(QString().setNum(_targetFuncSettings.A2));
    ui->EditR_nodeOverlap->setText(QString().setNum(_targetFuncSettings.R_nodeOverlap));
    ui->Edit_k_step_ot->setText(QString().setNum(_targetFuncSettings.k_step_ot));
    ui->Edit_offX->setText(QString().setNum(_targetFuncSettings.offX));
    ui->Edit_p->setText(QString().setNum(_targetFuncSettings.p));
    ui->chbIsUseCoveredFlag->setChecked(_targetFuncSettings.IsUseCoveredFlag);
    ui->chbIsUseLineBetweenTwoPoints->setChecked(_targetFuncSettings.IsUseLineBetweenTwoPoints);

    ui->EditTargetFuncGlobalFile->setText(_targetFuncSettings.FileName);
}
//----------------------------------------------------------

void DialogGradConfig::InitGradDescDialog(const tf_gd_lib::GradDescent &_gd)
{
    ui->EditMinEta->setText(QString().setNum((double)_gd.GetMin_Eta()));
    ui->EditFirstJump->setText(QString().setNum((double)_gd.GetEta_FirstJump()));
    ui->Edit_k_mult->setText(QString().setNum((double)_gd.GetEta_k_inc()));
    ui->Edit_k_div->setText(QString().setNum((double)_gd.GetEta_k_dec()));
    ui->EditAlpha->setText(QString().setNum((double)_gd.GetAlpha()));
    ui->Edit_eps->setText(QString().setNum((double)_gd.GetEps()));
    ui->EditCallbackFreq->setText(QString().setNum((double)_gd.GetCallBackFreq()));
    ui->chbFinMethod->setChecked((double)_gd.GetFinDifMethod());
    ui->EditMaxIters->setText(QString().setNum((double)_gd.GetMaxIters()));
    ui->EditMaxTime->setText(QString().setNum((double)_gd.GetMaxTime()));
}
//----------------------------------------------------------

DialogGradConfig::DialogGradConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogGradConfig)
{
    ui->setupUi(this);
}
//----------------------------------------------------------

DialogGradConfig::~DialogGradConfig()
{
    delete ui;
}
//----------------------------------------------------------

void DialogGradConfig::InitDialog(const MyGradModel &_gm)
{
    InitGradDescDialog(_gm.ProtoGradDesc);
    ui->EditGradDescFile->setText(_gm.GetGradDescFileName());

    InitTargetFuncSettingsDialog(_gm.TargetFuncSettingsGlobal);

    if (_gm.GetNodesType() == SignalNodeType::Sphere)
        ui->rbSignalSphere->setChecked(true);
    else if (_gm.GetNodesType() == SignalNodeType::Cone)
        ui->rbSignalCone->setChecked(true);
    else
        QMessageBox::warning(this, "Warning", "SignalNodeType is Unknown");

    ui->cbTargetFuncFirstPhase->clear();
    ui->cbTargetFuncSecondPhase->clear();

    for (const auto & item : _gm.GetTargetFunctions())
    {
        ui->cbTargetFuncFirstPhase->addItem(QString().fromStdString(item.first));
        ui->cbTargetFuncSecondPhase->addItem(QString().fromStdString(item.first));
    }

    auto it_TargetFunc_1 = _gm.GetTargetFunctions().find(_gm.GetActiveTargetFuncFirstPhase());
    if (it_TargetFunc_1 == _gm.GetTargetFunctions().end())
        QMessageBox::warning(this, "Warning", "TargetFunctionFirstPhase is Unknown!");
    else
        ui->cbTargetFuncFirstPhase->setCurrentText(QString().fromStdString(it_TargetFunc_1->first));

    auto it_TargetFunc_2 = _gm.GetTargetFunctions().find(_gm.GetActiveTargetFuncSecondPhase());
    if (it_TargetFunc_2 == _gm.GetTargetFunctions().end())
        QMessageBox::warning(this, "Warning", "TargetFunctionSecondPhase is Unknown!");
    else
        ui->cbTargetFuncSecondPhase->setCurrentText(QString().fromStdString(it_TargetFunc_2->first));

    ui->EditGrid_dx->setText(QString().setNum(_gm.GetGridSettings().dx));
    ui->EditGrid_dy->setText(QString().setNum(_gm.GetGridSettings().dy));
    ui->EditGrid_nDetails->setText(QString().setNum(_gm.GetGridSettings().nDetails));


    switch (_gm.GetBoundsRandCoords().BoundsType)
    {
    case BoundsTypeEnum::AllArea:
        ui->rbBoundsRandCoordsAllArea->setChecked(true);
        break;
    case BoundsTypeEnum::ByRoutes:
        ui->rbBoundsRandCoordsByRoutes->setChecked(true);
        break;
    case BoundsTypeEnum::Selected:
        ui->rbBoundsRandCoordsSelected->setChecked(true);
        break;
    default:
        throw std::runtime_error("Unknown BoundsTypeEnum in DialogGradConfig::InitDialog");
    }
    ui->EditBoundsRandCoordsStartX->setText(QString().setNum(_gm.GetBoundsRandCoords().SelXstart));
    ui->EditBoundsRandCoordsStartY->setText(QString().setNum(_gm.GetBoundsRandCoords().SelYstart));
    ui->EditBoundsRandCoordsWidth->setText(QString().setNum(_gm.GetBoundsRandCoords().SelW));
    ui->EditBoundsRandCoordsHeight->setText(QString().setNum(_gm.GetBoundsRandCoords().SelH));


    switch (_gm.GetBoundsGradDesc().BoundsType)
    {
    case BoundsTypeEnum::AllArea:
        ui->rbBoundsGradDescAllArea->setChecked(true);
        break;
    case BoundsTypeEnum::ByRoutes:
        ui->rbBoundsGradDescByRoutes->setChecked(true);
        break;
    case BoundsTypeEnum::Selected:
        ui->rbBoundsGradDescSelected->setChecked(true);
        break;
    default:
        throw std::runtime_error("Unknown BoundsTypeEnum in DialogGradConfig::InitDialog");
    }
    ui->EditBoundsGradDescStartX->setText(QString().setNum(_gm.GetBoundsGradDesc().SelXstart));
    ui->EditBoundsGradDescStartY->setText(QString().setNum(_gm.GetBoundsGradDesc().SelYstart));
    ui->EditBoundsGradDescWidth->setText(QString().setNum(_gm.GetBoundsGradDesc().SelW));
    ui->EditBoundsGradDescHeight->setText(QString().setNum(_gm.GetBoundsGradDesc().SelH));
}
//------------------------------------------------------------------

void DialogGradConfig::ReInitGradDesc(tf_gd_lib::GradDescent &_gd)
{
    _gd.SetMin_Eta(ui->EditMinEta->text().toDouble());
    _gd.SetEta_FirstJump(ui->EditFirstJump->text().toDouble());
    _gd.SetEta_k_inc(ui->Edit_k_mult->text().toDouble());
    _gd.SetEta_k_dec(ui->Edit_k_div->text().toDouble());
    _gd.SetAlpha(ui->EditAlpha->text().toDouble());
    _gd.SetEps(ui->Edit_eps->text().toDouble());
    _gd.SetCallBackFreq(ui->EditCallbackFreq->text().toInt());
    _gd.SetFinDifMethod(ui->chbFinMethod->isChecked());
    _gd.SetMaxIters(ui->EditMaxIters->text().toInt());
    _gd.SetMaxTime(ui->EditMaxTime->text().toDouble());
}
//------------------------------------------------------------------

void DialogGradConfig::ReInitTargetFuncSettings(TargetFuncSettingsStruct &_targetFuncSettings)
{
    _targetFuncSettings.Aarf = ui->EditAarf->text().toDouble();
    _targetFuncSettings.A2 = ui->EditA2->text().toDouble();
    _targetFuncSettings.R_nodeOverlap = ui->EditR_nodeOverlap->text().toDouble();
    _targetFuncSettings.k_step_ot = ui->Edit_k_step_ot->text().toDouble();
    _targetFuncSettings.offX = ui->Edit_offX->text().toDouble();
    _targetFuncSettings.p = ui->Edit_p->text().toDouble();
    _targetFuncSettings.IsUseCoveredFlag = ui->chbIsUseCoveredFlag->isChecked();
    _targetFuncSettings.IsUseLineBetweenTwoPoints = ui->chbIsUseLineBetweenTwoPoints->isChecked(); 

    _targetFuncSettings.FileName = ui->EditTargetFuncGlobalFile->text();
}
//------------------------------------------------------------------

void DialogGradConfig::ReInitGradModel(MyGradModel &_gm)
{
    if (ui->rbSignalCone->isChecked())
        _gm.SetNodesType(SignalNodeType::Cone);
    else if (ui->rbSignalSphere->isChecked())
        _gm.SetNodesType(SignalNodeType::Sphere);
    else
        _gm.SetNodesType(SignalNodeType::Unknown);


    auto it_TargetFunc_1 = _gm.GetTargetFunctions().find(ui->cbTargetFuncFirstPhase->currentText().toStdString());
    if (it_TargetFunc_1 == _gm.GetTargetFunctions().end())
        QMessageBox::warning(this, "Warning", "TargetFunctionFirstPhase is Unknown!");
    else
        ui->cbTargetFuncFirstPhase->setCurrentText(QString().fromStdString(it_TargetFunc_1->first));

    auto it_TargetFunc_2 = _gm.GetTargetFunctions().find(ui->cbTargetFuncSecondPhase->currentText().toStdString());
    if (it_TargetFunc_2 == _gm.GetTargetFunctions().end())
        QMessageBox::warning(this, "Warning", "TargetFunctionSecondPhase is Unknown!");
    else
        ui->cbTargetFuncSecondPhase->setCurrentText(QString().fromStdString(it_TargetFunc_2->first));

    _gm.SetActiveTargetFuncFirstPhase(ui->cbTargetFuncFirstPhase->currentText().toStdString());
    _gm.SetActiveTargetFuncSecondPhase(ui->cbTargetFuncSecondPhase->currentText().toStdString());

    _gm.SetGradDescFileName(ui->EditGradDescFile->text());



    _gm.SetGridSettings({ui->EditGrid_dx->text().toDouble(),
                        ui->EditGrid_dy->text().toDouble(),
                        ui->EditGrid_nDetails->text().toInt()});


    BoundsStruct bs;
    if (ui->rbBoundsRandCoordsAllArea->isChecked())
        bs.BoundsType = BoundsTypeEnum::AllArea;
    else if (ui->rbBoundsRandCoordsByRoutes->isChecked())
        bs.BoundsType = BoundsTypeEnum::ByRoutes;
    else if (ui->rbBoundsRandCoordsSelected->isChecked())
        bs.BoundsType = BoundsTypeEnum::Selected;
    else
        throw std::runtime_error("Unknown BoundsTypeEnum in DialogGradConfig::ReInitGradModel");

    bs.SelXstart = ui->EditBoundsRandCoordsStartX->text().toDouble();
    bs.SelYstart = ui->EditBoundsRandCoordsStartY->text().toDouble();
    bs.SelW = ui->EditBoundsRandCoordsWidth->text().toDouble();
    bs.SelH = ui->EditBoundsRandCoordsHeight->text().toDouble();
    _gm.SetBoundsRandCoords(bs);


    if (ui->rbBoundsGradDescAllArea->isChecked())
        bs.BoundsType = BoundsTypeEnum::AllArea;
    else if (ui->rbBoundsGradDescByRoutes->isChecked())
        bs.BoundsType = BoundsTypeEnum::ByRoutes;
    else if (ui->rbBoundsGradDescSelected->isChecked())
        bs.BoundsType = BoundsTypeEnum::Selected;
    else
        throw std::runtime_error("Unknown BoundsTypeEnum in DialogGradConfig::ReInitGradModel");

    bs.SelXstart = ui->EditBoundsGradDescStartX->text().toDouble();
    bs.SelYstart = ui->EditBoundsGradDescStartY->text().toDouble();
    bs.SelW = ui->EditBoundsGradDescWidth->text().toDouble();
    bs.SelH = ui->EditBoundsGradDescHeight->text().toDouble();
    _gm.SetBoundsGradDesc(bs);
}
//------------------------------------------------------------------

void DialogGradConfig::on_btnTargetFuncGlobalOpenFile_clicked()
{
    QString fn = QFileDialog::getOpenFileName(nullptr, "Select file",
                                              ui->EditTargetFuncGlobalFile->text(),
                                              "TargetFunc Settings Files (" + SettingsTFExtension + ")");

    if (fn.isEmpty())
    {
        qDebug() << "Selection TargetFunc Settings File canceled by user";
        return;
    }

    ui->EditTargetFuncGlobalFile->setText(fn);

    TargetFuncSettingsStruct tempTargetFuncSettings;
    if (!tempTargetFuncSettings.LoadFromFile(fn))
    {
        qDebug() << "Select TargetFunc Settings cannot be loaded";
        return;
    }

    InitTargetFuncSettingsDialog(tempTargetFuncSettings);
}
//------------------------------------------------------------------

void DialogGradConfig::on_btnTargetFuncGlobalSave_clicked()
{
    QString fn = ui->EditTargetFuncGlobalFile->text();
    if (fn.isEmpty()) {
        on_btnTargetFuncGlobalSaveAs_clicked();
        return;
    }

    TargetFuncSettingsStruct tempTargetFuncSettings;
    ReInitTargetFuncSettings(tempTargetFuncSettings);
    tempTargetFuncSettings.SaveToFile(fn);
}
//------------------------------------------------------------------

void DialogGradConfig::on_btnTargetFuncGlobalSaveAs_clicked()
{
    QString fn = QFileDialog::getSaveFileName(nullptr, "Select TargetFunc file",
                                               ui->EditTargetFuncGlobalFile->text(),
                                               "TargetFunc Settings Files (" + SettingsTFExtension + ")");
    if (!fn.isEmpty())
    {
        TargetFuncSettingsStruct tempTargetFuncSettings;
        ReInitTargetFuncSettings(tempTargetFuncSettings);
        tempTargetFuncSettings.SaveToFile(fn);
        ui->EditTargetFuncGlobalFile->setText(fn);
    }
}
//------------------------------------------------------------------

void DialogGradConfig::on_btnGradDescOpenFile_clicked()
{
    QString fn = QFileDialog::getOpenFileName(nullptr, "Select GradDesc file",
                                              ui->EditGradDescFile->text(),
                                              "GradDesc Settings Files (" + SettingsGDExtension + ")");

    if (fn.isEmpty())
    {
        qDebug() << "Selection GradDesc Settings File canceled by user";
        return;
    }

    ui->EditGradDescFile->setText(fn);

    tf_gd_lib::GradDescent tempGradDescSettings;
    if (!GradDescLoadFromFile(tempGradDescSettings, fn))
    {
        qDebug() << "Select TargetFunc Settings cannot be loaded";
        return;
    }

    InitGradDescDialog(tempGradDescSettings);
}
//------------------------------------------------------------------

void DialogGradConfig::on_btnGradDescSave_clicked()
{
    QString fn = ui->EditGradDescFile->text();
    if (fn.isEmpty()) {
        on_btnGradDescSaveAs_clicked();
        return;
    }

    tf_gd_lib::GradDescent tempGradDescSettings;
    ReInitGradDesc(tempGradDescSettings);
    GradDescSaveToFile(tempGradDescSettings, fn);
}
//------------------------------------------------------------------

void DialogGradConfig::on_btnGradDescSaveAs_clicked()
{
    QString fn = QFileDialog::getSaveFileName(nullptr, "Select GradDesc file",
                                               ui->EditGradDescFile->text(),
                                               "GradDesc Settings Files (" + SettingsGDExtension + ")");
    if (!fn.isEmpty())
    {
        tf_gd_lib::GradDescent tempGradDescSettings;
        ReInitGradDesc(tempGradDescSettings);
        GradDescSaveToFile(tempGradDescSettings, fn);
        ui->EditGradDescFile->setText(fn);
    }
}
//------------------------------------------------------------------

