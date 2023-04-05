#include "DialogAboReport.h"
#include "ui_DialogAboReport.h"

#include "GradConfig.h"

#include <QKeyEvent>

#include "Export.h"

DialogAboReport::DialogAboReport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAboReport)
{
    ui->setupUi(this);
}
//----------------------------------------------------------

DialogAboReport::~DialogAboReport()
{
    delete ui;
}
//----------------------------------------------------------

void DialogAboReport::InitDialog()
{
    TimePoints.clear();

    ui->Table->clear();
    ui->Table->setRowCount(2);
    ui->Table->setColumnCount(4);

    ui->Table->setItem(0, 0, new QTableWidgetItem("t, h"));
    ui->Table->setColumnWidth(0, 60);
    ui->Table->item(0, 0)->setFlags( ui->Table->item(0, 0)->flags() & ~Qt::ItemIsEditable );

    ui->Table->setItem(0, 1, new QTableWidgetItem("Route Name"));
    ui->Table->setColumnWidth(1, 120);
    ui->Table->item(0, 1)->setFlags( ui->Table->item(0, 1)->flags() & ~Qt::ItemIsEditable );

    ui->Table->setItem(0, 2, new QTableWidgetItem("iAbo"));
    ui->Table->setColumnWidth(2, 100);
    ui->Table->item(0, 2)->setFlags( ui->Table->item(0, 2)->flags() & ~Qt::ItemIsEditable );

    ui->Table->setItem(0, 3, new QTableWidgetItem("AccessRate, ?"));
    ui->Table->setColumnWidth(3, 100);
    ui->Table->item(0, 3)->setFlags( ui->Table->item(0, 3)->flags() & ~Qt::ItemIsEditable );

}
//----------------------------------------------------------

void DialogAboReport::AddTimePoint(int t /*const MyConfig &_cnfg*/)
{
    TimePoints.emplace(t);
}
//----------------------------------------------------------

// Подумать: может, скопировать только одну нужную конфигурацию,
// чтобы ее было не жалко портить при расчете accessrate
// void DialogAboReport::CalcTable(MyGradModel &_gradModel)
void DialogAboReport::CalcTable(const MyConfig &_cnfg,
                                bool _isUseLineBetweenTwoPoints,
                                const SignalNodeType &_snt,
                                TargetFuncTypeEnum funcType)
{
    MyConfig tempConfig(_cnfg);

    ui->Table->setRowCount(1 + TimePoints.size()*tempConfig.RoutesAccess().size());

    int i = 0;
    for (const auto & t : TimePoints)
    {
        for (auto & r : tempConfig.RoutesAccess())
        {
            r.CalcAbonentPos(t);
        }

        tempConfig.CalcAccessRateForAbos(_isUseLineBetweenTwoPoints, _snt, funcType);

        ui->Table->setItem(1 + i, 0, new QTableWidgetItem(QString().setNum(t/3600.0, 'f', 3)));
        ui->Table->item(1 + i, 0)->setFlags( ui->Table->item(1 + i, 0)->flags() & ~Qt::ItemIsEditable );


        for (auto & r : tempConfig.RoutesAccess())
        {
//            ui->Table->item(1 + i, 0)->setFlags( ui->Table->item(1 + i, 0)->flags() & ~Qt::ItemIsEditable );


            ui->Table->setItem(1 + i, 1, new QTableWidgetItem(r.GetName()));
            ui->Table->item(1 + i, 1)->setFlags( ui->Table->item(1 + i, 1)->flags() & ~Qt::ItemIsEditable );

            ui->Table->setItem(1 + i, 2, new QTableWidgetItem(QString().setNum(0)));
            ui->Table->item(1 + i, 2)->setFlags( ui->Table->item(1 + i, 2)->flags() & ~Qt::ItemIsEditable );

            auto ar = r.GetAbonent().accessRate;
            ui->Table->setItem(1 + i, 3, new QTableWidgetItem(QString().setNum((double)ar)));
            ui->Table->item(1 + i, 3)->setFlags( ui->Table->item(1 + i, 3)->flags() & ~Qt::ItemIsEditable );

            ++i;
        }

    }
}
//----------------------------------------------------------

void DialogAboReport::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_C && QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
        //cout << __PRETTY_FUNCTION__ << endl;

        CopySelectedTableToClipboard(ui->Table);
    }
}
//----------------------------------------------------------
