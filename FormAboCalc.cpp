#include "FormAboCalc.h"
#include "ui_FormAboCalc.h"

FormAboCalc::FormAboCalc(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormAboCalc)
{
    ui->setupUi(this);
}

FormAboCalc::~FormAboCalc()
{
    delete ui;
}

void FormAboCalc::on_btnToReport_clicked()
{
    qDebug() << __PRETTY_FUNCTION__;

}

void FormAboCalc::on_EditMaxTime_editingFinished()
{
    qDebug() << __PRETTY_FUNCTION__ << "!!!!!!!!!!!!!!!!!!!!!!";
    QString strMaxTime = ui->EditMaxTime->text();
    bool isOk = false;
    double maxTime = strMaxTime.toDouble(&isOk);

    if (!isOk)
    {
        maxTime = 24*3600;
        ui->EditMaxTime->setText("24");
        ui->SliderTime->setMaximum(maxTime);
        return;
    }

    ui->SliderTime->setMaximum(maxTime*3600);
    if (maxTime < ui->EditCurTime->text().toDouble())
    {
        ui->EditCurTime->setText(QString().setNum(maxTime, 'f', 3));
    }

//    qDebug() <<  ui->SliderTime->value();
}


void FormAboCalc::on_EditMaxTime_textChanged(const QString &str)
{
    // включая программное изменение содержимого едита
    qDebug() << __PRETTY_FUNCTION__ << "str =" << str;
}


void FormAboCalc::on_EditMaxTime_textEdited(const QString &str)
{
    qDebug() << __PRETTY_FUNCTION__ << "str =" << str;
}


void FormAboCalc::on_btnOk_clicked()
{
    qDebug() << __PRETTY_FUNCTION__;
    this->hide();
}


void FormAboCalc::showEvent([[maybe_unused]] QShowEvent *event)
{
    qDebug() << __PRETTY_FUNCTION__;

    ui->EditMaxTime->setText("24");
    ui->SliderTime->setMaximum(24*3600);

    ui->EditCurTime->setText("0");
    ui->SliderTime->setValue(0*3600);
}

void FormAboCalc::on_SliderTime_sliderMoved(int position)
{
    ui->EditCurTime->setText(QString().setNum(position/3600.0, 'f', 3));

//    emit SignalSendAboTime(position);
}


void FormAboCalc::on_EditCurTime_editingFinished()
{
    qDebug() << __PRETTY_FUNCTION__ << "!!!!!!!!!!!!!!!!!!!!!!";
    QString strCurTime = ui->EditCurTime->text();
    bool isOk = false;
    double curTime = strCurTime.toDouble(&isOk);

    if (!isOk)
    {
        curTime = 0*3600;
        ui->EditCurTime->setText("0");
        ui->SliderTime->setValue(curTime);
//        ui->SliderTime->setMaximum(curTime);
        return;
    }

    curTime *= 3600;
    if (curTime < ui->SliderTime->maximum())
    {
        ui->SliderTime->setValue(curTime);
       //ui->EditCurTime->setText(QString().setNum(maxTime, 'f', 3));
    }
    else
    {
        ui->EditCurTime->setText(QString().setNum(ui->SliderTime->maximum()/3600.0, 'f', 3));
        ui->SliderTime->setValue(ui->SliderTime->maximum());
    }

}

void FormAboCalc::on_SliderTime_valueChanged(int value)
{
    emit SignalSendAboTime(value);
}

