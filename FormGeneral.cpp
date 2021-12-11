#include "FormGeneral.h"
#include "ui_FormGeneral.h"

#include "Model.h"

#include <QTableWidgetItem>

//#include <QHBoxLayout>
//#include <QClipboard>

#include <QMessageBox>

static constexpr int stGT = 2;

#include <iostream>
using namespace std;

FormGeneral::FormGeneral(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormGeneral)
{
    ui->setupUi(this);

  //  ui->Table->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    InitTable();

//    ui->Table->setRowCount(ui->Table->rowCount()+1);
   // setLayout(hLayout);
}
//----------------------------------------------------------

void FormGeneral::InitTable()
{
    ui->Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
   // ui->Table->setFlags(  ui->Table->flags() & ~Qt::ItemIsEditable);

    ui->Table->clear();

    ui->Table->setRowCount(2);
    ui->Table->setColumnCount(stGT + 4*GeneralTableCount);

    ui->Table->setItem(0, stGT + 0*GeneralTableCount, new QTableWidgetItem("1st place"));
    ui->Table->setItem(0, stGT + 1*GeneralTableCount, new QTableWidgetItem("2nd place"));
    ui->Table->setItem(0, stGT + 2*GeneralTableCount, new QTableWidgetItem("3rd place"));
    ui->Table->setItem(0, stGT + 3*GeneralTableCount, new QTableWidgetItem("4th place"));

    ui->Table->setSpan(0, stGT + 0*GeneralTableCount, 1, 4);
    ui->Table->item(0,    stGT + 0*GeneralTableCount)->setTextAlignment(Qt::AlignCenter);

    ui->Table->setSpan(0, stGT + 1*GeneralTableCount, 1, 4);
    ui->Table->item(0,    stGT + 1*GeneralTableCount)->setTextAlignment(Qt::AlignCenter);

    ui->Table->setSpan(0, stGT + 2*GeneralTableCount, 1, 4);
    ui->Table->item(0,    stGT + 2*GeneralTableCount)->setTextAlignment(Qt::AlignCenter);

    ui->Table->setSpan(0, stGT + 3*GeneralTableCount, 1, 4);
    ui->Table->item(0,    stGT + 3*GeneralTableCount)->setTextAlignment(Qt::AlignCenter);


    ui->Table->setItem(1, 0, new QTableWidgetItem("n"));
    ui->Table->setColumnWidth(0, 14);

    ui->Table->setItem(1, 1, new QTableWidgetItem("Time"));
    ui->Table->setColumnWidth(1, 60);

    for (size_t i = 0; i < GeneralTableCount; ++i)
    {

        ui->Table->setItem(1,     stGT + 0  + i*GeneralTableCount, new QTableWidgetItem("id"));
        ui->Table->setColumnWidth(stGT + 0  + i*GeneralTableCount, 20);

        ui->Table->setItem(1,     stGT + 1  + i*GeneralTableCount, new QTableWidgetItem("w"));
        ui->Table->setColumnWidth(stGT + 1  + i*GeneralTableCount, 60);

        ui->Table->setItem(1,     stGT + 2  + i*GeneralTableCount, new QTableWidgetItem("w0"));
        ui->Table->setColumnWidth(stGT + 2  + i*GeneralTableCount, 60);

        ui->Table->setItem(1,     stGT + 3  + i*GeneralTableCount, new QTableWidgetItem("last op"));
        ui->Table->setColumnWidth(stGT + 3  + i*GeneralTableCount, 60);
    }
}
//----------------------------------------------------------

void FormGeneral::keyPressEvent(QKeyEvent *e)
{
//    cout << __PRETTY_FUNCTION__ << endl;

    if (e->key() == Qt::Key_C && QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
        cout << __PRETTY_FUNCTION__ << endl;

        CopySelectedTableToClipboard(ui->Table);
    }
}
//----------------------------------------------------------

void FormGeneral::slotReceiveGeneralData(ExportGeneralData _generalData)
{
    int curRow = ui->Table->rowCount();
//    ui->Table->insertRow(curRow);

    ui->Table->setRowCount(curRow+1);

    static size_t n = 0;
    n++;

    ui->Table->setItem(curRow, 0, new QTableWidgetItem(QString().setNum(n)));
    ui->Table->setItem(curRow, 1, new QTableWidgetItem(QString().setNum(_generalData.Time, 'f', 6)));

    if (GeneralTableCount != _generalData.GeneralInfos.size())
        throw std::runtime_error("GeneralTableCount != _generalData.GeneralInfos.size()");

    for (size_t i = 0; i < GeneralTableCount; ++i)
    {
        ui->Table->setItem(curRow, stGT + 0 + i*GeneralTableCount, new QTableWidgetItem(QString().setNum(_generalData.GeneralInfos[i].id)));

        ui->Table->setItem(curRow, stGT + 1 + i*GeneralTableCount, new QTableWidgetItem(QString().setNum(_generalData.GeneralInfos[i].w, 'f', 6)));

        ui->Table->setItem(curRow, stGT + 2 + i*GeneralTableCount, new QTableWidgetItem(QString().setNum(_generalData.GeneralInfos[i].w0, 'f', 6)));

        ui->Table->setItem(curRow, stGT + 3 + i*GeneralTableCount, new QTableWidgetItem(_generalData.GeneralInfos[i].LastOp));
    }
}
//----------------------------------------------------------

void FormGeneral::ShowTable()
{
    InitTable();

    const auto & AllGeneralData = Model::Instance().GetExportAllGeneralData();

    ui->Table->setRowCount(AllGeneralData.size() + 2);

    size_t n = 0;
    for (const auto & OneLine : AllGeneralData)
    {
        ui->Table->setItem(n+2, 0, new QTableWidgetItem(QString().setNum(n+1)));
        ui->Table->setItem(n+2, 1, new QTableWidgetItem(QString().setNum(OneLine.Time, 'f', 6)));

        for (size_t i = 0; i < GeneralTableCount; ++i)
        {
            ui->Table->setItem(n+2, stGT + 0 + i*GeneralTableCount, new QTableWidgetItem(QString().setNum(OneLine.GeneralInfos[i].id)));

            ui->Table->setItem(n+2, stGT + 1 + i*GeneralTableCount, new QTableWidgetItem(QString().setNum(OneLine.GeneralInfos[i].w, 'f', 6)));

            ui->Table->setItem(n+2, stGT + 2 + i*GeneralTableCount, new QTableWidgetItem(QString().setNum(OneLine.GeneralInfos[i].w0, 'f', 6)));

            ui->Table->setItem(n+2, stGT + 3 + i*GeneralTableCount, new QTableWidgetItem(OneLine.GeneralInfos[i].LastOp));
        }
        n++;
    }
}
//----------------------------------------------------------

FormGeneral::~FormGeneral()
{
    //QMessageBox::information(nullptr, "Ttile", __PRETTY_FUNCTION__);

    delete ui;
}
//----------------------------------------------------------
