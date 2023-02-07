#include "DialogNewGradConfig.h"
#include "ui_DialogNewGradConfig.h"

#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

#include "GradModel.h"

DialogNewGradConfig::DialogNewGradConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewGradConfig)
{
    ui->setupUi(this);
}
//----------------------------------------------------------

DialogNewGradConfig::~DialogNewGradConfig()
{
    delete ui;
}
//----------------------------------------------------------

bool DialogNewGradConfig::CreateNewGradModel(MyGradModel &_gradModel)
{
    QString reliefFileName = ui->EditReliefFile->text();

    if (!_gradModel.LoadReliefFromFile(reliefFileName))
    {
        //throw std::runtime_error("Relief File Not Found");
        QMessageBox::critical(this, "Error", "Relief File Not Found or Corrupted");
        return false;
    }

    bool isOk = false;
    size_t signalNodeCount = ui->EditSignalNodeCount->text().toInt(&isOk);
    if (!isOk)
    {
        QMessageBox::critical(this, "Error", "SignalNode Count is incorrect!");
        return false;
    }

    isOk = false;
    double R = ui->EditSignalNodeR->text().toDouble(&isOk);
    if (!isOk)
    {
        QMessageBox::critical(this, "Error", "SignalNode R is incorrect!");
        return false;
    }

    isOk = false;
    double Beta = ui->EditSignalNodeBetha->text().toDouble(&isOk);
    if (!isOk || Beta < 0 || Beta > 180)
    {
        QMessageBox::critical(this, "Error", "SignalNode Betha is incorrect!");
        return false;
    }

    _gradModel.ReCreateSignalNodes(signalNodeCount, R, qDegreesToRadians(Beta));

    isOk = false;
    size_t configCount = ui->EditPopulationSize->text().toInt(&isOk);
    if (!isOk)
    {
        QMessageBox::critical(this, "Error", "Population Size is incorrect!");
        return false;
    }

    _gradModel.CreatePopulation(configCount);

    _gradModel.ReCreateReliefListsAndBuildRelief();

    _gradModel.MarkAsNotSaved();

    return true;
}
//----------------------------------------------------------

void DialogNewGradConfig::on_btnReliefFileBrowse_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Open Relief File", ".", "Relief Files (" + ReliefsExtension + ")");

    if (fileName == "")
    {
        qDebug() << "Relief File not selected";
        return;
    }

    ui->EditReliefFile->setText(fileName);
}
//----------------------------------------------------------
