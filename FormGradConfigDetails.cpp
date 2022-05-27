#include "FormGradConfigDetails.h"
#include "ui_FormGradConfigDetails.h"


#include "Export.h"
#include "GradConfig.h"

#include <QMessageBox>
#include <QKeyEvent>

//static constexpr int stGT = 1;
//static constexpr int enGT = 4;

FormGradConfigDetails::FormGradConfigDetails(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormGradConfigDetails)
{
    ui->setupUi(this);

    InitTable();
}
//----------------------------------------------------------

FormGradConfigDetails::~FormGradConfigDetails()
{
    delete ui;
}
//----------------------------------------------------------

void FormGradConfigDetails::InitTable()
{
    ui->Table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->Table->clear();

    ui->Table->setRowCount(1);
    ui->Table->setColumnCount(5);

    ui->Table->setItem(0, 0, new QTableWidgetItem("Node"));
    ui->Table->item(   0, 0)->setTextAlignment(Qt::AlignCenter);
    ui->Table->setColumnWidth(0, 40);

    ui->Table->setItem(0, 1, new QTableWidgetItem("Route"));
    ui->Table->item(   0, 1)->setTextAlignment(Qt::AlignCenter);
    ui->Table->setColumnWidth(1, 40);

    ui->Table->setItem(0, 2, new QTableWidgetItem("Point"));
    ui->Table->item(   0, 2)->setTextAlignment(Qt::AlignCenter);
    ui->Table->setColumnWidth(2, 40);

    ui->Table->setItem(0, 3, new QTableWidgetItem("Access Rate"));
    ui->Table->item(   0, 3)->setTextAlignment(Qt::AlignCenter);
    ui->Table->setColumnWidth(3, 90);

    ui->Table->setItem(0, 4, new QTableWidgetItem("Dist/R"));
    ui->Table->item(   0, 4)->setTextAlignment(Qt::AlignCenter);
    ui->Table->setColumnWidth(4, 90);
}
//----------------------------------------------------------

void FormGradConfigDetails::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_C && QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
        //cout << __PRETTY_FUNCTION__ << endl;

        CopySelectedTableToClipboard(ui->Table);
    }
}
//----------------------------------------------------------

bool FormGradConfigDetails::ShowTable(const MyConfig &_config, size_t number)
{
    InitTable();

    this->setWindowTitle("Config " + QString().setNum(number) + " details");

    const size_t NodeCount = _config.GetNodes().size();

    int row;
    for (size_t iNode = 0; iNode < NodeCount; ++iNode)
    {
        for (const auto & b : _config.GetNodes().at(iNode).Bonds)
        {
            row = ui->Table->rowCount();
            ui->Table->setRowCount(row + 1);

            ui->Table->setItem(row, 0, new QTableWidgetItem(QString().setNum(iNode)));
            ui->Table->setItem(row, 1, new QTableWidgetItem(QString().setNum(b.iRoute)));
            ui->Table->setItem(row, 2, new QTableWidgetItem(QString().setNum(b.iPoint)));
            ui->Table->setItem(row, 3, new QTableWidgetItem(QString().setNum(b.arf)));
            ui->Table->setItem(row, 4, new QTableWidgetItem(QString().setNum(b.relDist)));
        }
    }

//    ui->Table->setRowCount(3 + _configs.size());

//    for (size_t i = 0; i < _configs.size(); ++i)
//    {
//        ui->Table->setItem(i+3, 0, new QTableWidgetItem(QString().setNum(i)));

//        for (size_t j = 0; j < NodeCount; ++j)
//        {
//            ui->Table->setItem(3 + i, stGT + 2*j + 0,
//                               new QTableWidgetItem(QString().setNum(_configs[i].GetNodes().at(j).Pos.x())));

//            ui->Table->setItem(3 + i, stGT + 2*j + 1,
//                               new QTableWidgetItem(QString().setNum(_configs[i].GetNodes().at(j).Pos.y())));
//        }

//        ui->Table->setItem(i+3, stGT + 2*NodeCount + 0,
//                           new QTableWidgetItem(QString().setNum(_configs[i].GetStats().PercentOfCovered)));
//        ui->Table->setItem(i+3, stGT + 2*NodeCount + 1,
//                           new QTableWidgetItem(QString().setNum(_configs[i].GetStats().UncoveredCount)));
//        ui->Table->setItem(i+3, stGT + 2*NodeCount + 2,
//                           new QTableWidgetItem(QString().setNum(_configs[i].GetStats().LastCostPhase1)));
//        ui->Table->setItem(i+3, stGT + 2*NodeCount + 3,
//                           new QTableWidgetItem(_configs[i].GetStats().LastCostPhase2 ?
//                                                QString().setNum(_configs[i].GetStats().LastCostPhase2) : QString("-")
//                                                                     ));
//    }

    return true;
}
//----------------------------------------------------------
