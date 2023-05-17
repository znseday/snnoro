#include "FormLegendIsolines.h"
#include "ui_FormLegendIsolines.h"



FormLegendIsolines::FormLegendIsolines(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLegendIsolines)
{
    ui->setupUi(this);
}
//-------------------------------------------------------------

FormLegendIsolines::~FormLegendIsolines()
{
    delete ui;
}
//-------------------------------------------------------------

void FormLegendIsolines::ReceiveLegendIsolines(const LegendIsolinesType &_li)
{
    int rowCount = _li.size();

    this->setWindowTitle("color count = " + QString().setNum(rowCount));

    ui->tableLegend->setRowCount(rowCount);

    ui->tableLegend->clear(); // to TEST !!!!!!

    for (int i = 0; i < rowCount; ++i)
    {
        QTableWidgetItem *pItem = new QTableWidgetItem;
        pItem->setBackground(QBrush(_li.at(i).first));
        pItem->setFlags(Qt::ItemIsEnabled /*| Qt::ItemIsSelectable*/);
        ui->tableLegend->setItem(i, 0, pItem);

        QTableWidgetItem *pItem2 = new QTableWidgetItem(QString().setNum(_li.at(i).second));
        ui->tableLegend->setItem(i, 1, pItem2);
    }

}
//-------------------------------------------------------------
