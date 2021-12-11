#include "DialogChangePopulationSize.h"
#include "ui_DialogChangePopulationSize.h"

DialogChangePopulationSize::DialogChangePopulationSize(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogChangePopulationSize)
{
    ui->setupUi(this);
}
//----------------------------------------------------------

DialogChangePopulationSize::~DialogChangePopulationSize()
{
    delete ui;
}
//----------------------------------------------------------

void DialogChangePopulationSize::InitDialog(int _populationSize)
{
    ui->EditNewPopulationSize->setText(QString().setNum(_populationSize));
}
//----------------------------------------------------------

int DialogChangePopulationSize::GetNewPopulationSize()
{
    return ui->EditNewPopulationSize->text().toInt();
}
//----------------------------------------------------------
