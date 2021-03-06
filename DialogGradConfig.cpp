#include "DialogGradConfig.h"
#include "ui_DialogGradConfig.h"

#include <QString>
#include <QMessageBox>

#include "GradModel.h"

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
    ui->EditMinEta->setText(QString().setNum(_gm.ProtoGradDesc.GetMin_Eta()));
    ui->EditFirstJump->setText(QString().setNum(_gm.ProtoGradDesc.GetEta_FirstJump()));
    ui->Edit_k_mult->setText(QString().setNum(_gm.ProtoGradDesc.GetEta_k_inc()));
    ui->Edit_k_div->setText(QString().setNum(_gm.ProtoGradDesc.GetEta_k_dec()));
    ui->EditAlpha->setText(QString().setNum(_gm.ProtoGradDesc.GetAlpha()));
    ui->Edit_eps->setText(QString().setNum(_gm.ProtoGradDesc.GetEps()));
    ui->EditCallbackFreq->setText(QString().setNum(_gm.ProtoGradDesc.GetCallBackFreq()));
    ui->chbFinMethod->setChecked(_gm.ProtoGradDesc.GetFinDifMethod());
    ui->EditMaxIters->setText(QString().setNum(_gm.ProtoGradDesc.GetMaxIters()));
    ui->EditMaxTime->setText(QString().setNum(_gm.ProtoGradDesc.GetMaxTime()));

    ui->EditAarf->setText(QString().setNum(_gm.TargetFuncSettingsGlobal.Aarf));
    ui->EditA2->setText(QString().setNum(_gm.TargetFuncSettingsGlobal.A2));
    ui->EditR_nodeOverlap->setText(QString().setNum(_gm.TargetFuncSettingsGlobal.R_nodeOverlap));
    ui->Edit_k_step_ot->setText(QString().setNum(_gm.TargetFuncSettingsGlobal.k_step_ot));
    ui->Edit_offX->setText(QString().setNum(_gm.TargetFuncSettingsGlobal.offX));
    ui->Edit_p->setText(QString().setNum(_gm.TargetFuncSettingsGlobal.p));
    ui->chbIsUseCoveredFlag->setChecked(_gm.TargetFuncSettingsGlobal.IsUseCoveredFlag);

    ui->chbIsUseLineBetweenTwoPoints->setChecked(_gm.TargetFuncSettingsGlobal.IsUseLineBetweenTwoPoints);

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

    auto it_TargetFunc_1 = _gm.GetTargetFunctions().find(_gm.TargetFuncSettingsGlobal.ActiveTargetFuncFirstPhase);
    if (it_TargetFunc_1 == _gm.GetTargetFunctions().end())
        QMessageBox::warning(this, "Warning", "TargetFunctionFirstPhase is Unknown!");
    else
        ui->cbTargetFuncFirstPhase->setCurrentText(QString().fromStdString(it_TargetFunc_1->first));

    auto it_TargetFunc_2 = _gm.GetTargetFunctions().find(_gm.TargetFuncSettingsGlobal.ActiveTargetFuncSecondPhase);
    if (it_TargetFunc_2 == _gm.GetTargetFunctions().end())
        QMessageBox::warning(this, "Warning", "TargetFunctionSecondPhase is Unknown!");
    else
        ui->cbTargetFuncSecondPhase->setCurrentText(QString().fromStdString(it_TargetFunc_2->first));

//    if (_gm.TargetFuncSettingsGlobal.TargetFuncType == TargetFuncEnum::Additive)
//        ui->rbTargetFuncAdditive->setChecked(true);
//    else if (_gm.TargetFuncSettingsGlobal.TargetFuncType == TargetFuncEnum::Probabilistic)
//        ui->rbTargetFuncProbabilistic->setChecked(true);
//    else
//        QMessageBox::warning(this, "Warning", "TargetFuncType is Unknown");

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

    _gm.TargetFuncSettingsGlobal.ActiveTargetFuncFirstPhase = ui->cbTargetFuncFirstPhase->currentText().toStdString();
    _gm.TargetFuncSettingsGlobal.ActiveTargetFuncSecondPhase = ui->cbTargetFuncSecondPhase->currentText().toStdString();

//    if (ui->rbTargetFuncAdditive->isChecked())
//        _gm.TargetFuncSettingsGlobal.TargetFuncType = TargetFuncEnum::Additive;
//    else if (ui->rbTargetFuncProbabilistic->isChecked())
//        _gm.TargetFuncSettingsGlobal.TargetFuncType = TargetFuncEnum::Probabilistic;
//    else
//        _gm.TargetFuncSettingsGlobal.TargetFuncType = TargetFuncEnum::Empty;
}
//------------------------------------------------------------------
