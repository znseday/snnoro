#include "FormGradGeneral.h"
#include "ui_FormGradGeneral.h"

#include "Export.h"
#include "GradConfig.h"

#include <QMessageBox>
#include <QKeyEvent>

static constexpr int stGT = 1;
static constexpr int enGT = 4;

FormGradGeneral::FormGradGeneral(const std::vector<MyConfig> &_configs, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormGradGeneral), Configs(_configs)
{
    ui->setupUi(this);

    ui->Table->addAction(ui->actionGo_to_details);

    formGradConfigDetails.setWindowTitle("Config Details");
    //InitTable();
}
//----------------------------------------------------------

FormGradGeneral::~FormGradGeneral()
{
    delete ui;
}
//----------------------------------------------------------

void FormGradGeneral::InitTable(size_t NodeCount)
{
    ui->Table->setEditTriggers(QAbstractItemView::NoEditTriggers);

   // ui->Table->setFlags(  ui->Table->flags() & ~Qt::ItemIsEditable);

    ui->Table->clear();

    ui->Table->setRowCount(3);
    ui->Table->setColumnCount(stGT + 2*NodeCount + enGT);

    ui->Table->setSpan(0, 0, 3, 1);
    ui->Table->setItem(0, 0, new QTableWidgetItem("n"));
    ui->Table->item(0, 0)->setTextAlignment(Qt::AlignCenter);
    ui->Table->setColumnWidth(0, 14);

    ui->Table->setSpan(0, stGT, 1, 2*NodeCount);
    ui->Table->setItem(0, stGT, new QTableWidgetItem("Node Number and Its Coords"));
    ui->Table->item(0,    stGT)->setTextAlignment(Qt::AlignCenter);
    //ui->Table->setColumnWidth(0, 14);

    for (size_t i = 0; i < NodeCount; ++i)
    {
        ui->Table->setSpan(1, stGT + 2*i, 1, 2);
        ui->Table->setItem(1, stGT + 2*i, new QTableWidgetItem(QString().setNum(i)));
        ui->Table->item(1,    stGT + 2*i)->setTextAlignment(Qt::AlignCenter);
    }

    for (size_t i = 0; i < 2*NodeCount; ++i)
    {
        ui->Table->setItem(2, stGT + i, new QTableWidgetItem(i%2?"y":"x"));
        ui->Table->item(2,    stGT + i)->setTextAlignment(Qt::AlignCenter);
    }

    ui->Table->setSpan(0, stGT + 2*NodeCount + 0, 3, 1);
    ui->Table->setItem(0, stGT + 2*NodeCount + 0, new QTableWidgetItem("% covered"));
    ui->Table->item(   0, stGT + 2*NodeCount + 0)->setTextAlignment(Qt::AlignCenter);
    ui->Table->setColumnWidth(stGT + 2*NodeCount + 0, 60);

    ui->Table->setSpan(0, stGT + 2*NodeCount + 1, 3, 1);
    ui->Table->setItem(0, stGT + 2*NodeCount + 1, new QTableWidgetItem("Count Uncovered"));
    ui->Table->item(   0, stGT + 2*NodeCount + 1)->setTextAlignment(Qt::AlignCenter);
    ui->Table->setColumnWidth(stGT + 2*NodeCount + 1, 80);

    ui->Table->setItem(2, stGT + 2*NodeCount + 2, new QTableWidgetItem("Phase 1"));
    ui->Table->item(   2, stGT + 2*NodeCount + 2)->setTextAlignment(Qt::AlignCenter);
    ui->Table->setItem(2, stGT + 2*NodeCount + 3, new QTableWidgetItem("Phase 2"));
    ui->Table->item(   2, stGT + 2*NodeCount + 3)->setTextAlignment(Qt::AlignCenter);

    ui->Table->setSpan(0, stGT + 2*NodeCount + 2, 2, 2);
    ui->Table->setItem(0, stGT + 2*NodeCount + 2, new QTableWidgetItem("Target Func Value"));
    ui->Table->item(   0, stGT + 2*NodeCount + 2)->setTextAlignment(Qt::AlignCenter);
    ui->Table->setColumnWidth(stGT + 2*NodeCount + 2, 120);

}
//----------------------------------------------------------

void FormGradGeneral::keyPressEvent(QKeyEvent *e)
{
//    cout << __PRETTY_FUNCTION__ << endl;

    if (e->key() == Qt::Key_C && QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
        //cout << __PRETTY_FUNCTION__ << endl;

        CopySelectedTableToClipboard(ui->Table);
    }
}
//----------------------------------------------------------

bool FormGradGeneral::ShowTable(/*const std::vector<MyConfig> &_configs*/)
{
    if (Configs.empty())
        return false;

    const size_t NodeCount = Configs.front().GetNodes().size();

    InitTable(NodeCount);

    ui->Table->setRowCount(3 + Configs.size());

    for (size_t i = 0; i < Configs.size(); ++i)
    {
        ui->Table->setItem(i+3, 0, new QTableWidgetItem(QString().setNum(i)));

        for (size_t j = 0; j < NodeCount; ++j)
        {
            ui->Table->setItem(3 + i, stGT + 2*j + 0,
                               new QTableWidgetItem(QString().setNum(Configs[i].GetNodes().at(j).Pos.x())));

            ui->Table->setItem(3 + i, stGT + 2*j + 1,
                               new QTableWidgetItem(QString().setNum(Configs[i].GetNodes().at(j).Pos.y())));
        }

        ui->Table->setItem(i+3, stGT + 2*NodeCount + 0,
                           new QTableWidgetItem(QString().setNum(Configs[i].GetStats().PercentOfCovered)));
        ui->Table->setItem(i+3, stGT + 2*NodeCount + 1,
                           new QTableWidgetItem(QString().setNum(Configs[i].GetStats().UncoveredCount)));
        ui->Table->setItem(i+3, stGT + 2*NodeCount + 2,
                           new QTableWidgetItem(QString().setNum(Configs[i].GetStats().LastCostPhase1)));
        ui->Table->setItem(i+3, stGT + 2*NodeCount + 3,
                           new QTableWidgetItem(Configs[i].GetStats().LastCostPhase2 ?
                                                QString().setNum(Configs[i].GetStats().LastCostPhase2) : QString("-")
                                                                     ));
    }

    return true;
}
//----------------------------------------------------------

void FormGradGeneral::on_actionGo_to_details_triggered()
{
    QModelIndexList list = ui->Table->selectionModel()->selectedIndexes();

    if (list.isEmpty())
        return;

    int row = list.first().row() - 3;
    //QMessageBox::information(this, "Selected", "For " + QString().setNum(row));

    formGradConfigDetails.ShowTable(Configs.at(row), row);
    formGradConfigDetails.show();
}
//----------------------------------------------------------
