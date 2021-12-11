#include "FormConfigInfo.h"
#include "ui_FormConfigInfo.h"

#include <QTableWidgetItem>
#include <QClipboard>

#include "Model.h"

static constexpr int stGT = 2;

#include <iostream>
using namespace std;

FormConfigInfo::FormConfigInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormConfigInfo)
{
    ui->setupUi(this);

    ui->Table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->Table->setRowCount(2);
    ui->Table->setColumnCount(10);

}
//----------------------------------------------------------

void FormConfigInfo::Set_nConfig(int _nConfig)
{
    nConfig = _nConfig;
    setWindowTitle("Configuration: " + QString().setNum(_nConfig));
}
//----------------------------------------------------------

void FormConfigInfo::InitTable(const ExportDetailedInfo &_detailedInfo)
{
    const int offsetNC = _detailedInfo.PeopleCoords.size();
    const int offsetSP = offsetNC + _detailedInfo.NodeCoords.size();
    const int offsetDTNN = offsetSP + _detailedInfo.SignalPowers.size();

    ui->Table->clear();

    ui->Table->setColumnCount(stGT + 3*_detailedInfo.PeopleCoords.size() + _detailedInfo.NodeCoords.size());

    ui->Table->setItem(1, 0, new QTableWidgetItem("n"));
    ui->Table->setColumnWidth(0, 14);

    ui->Table->setItem(1, 1, new QTableWidgetItem("Time"));
    ui->Table->setColumnWidth(1, 60);

    // Координаты абонентов
    ui->Table->setItem(0, stGT, new QTableWidgetItem("People Coords"));
    ui->Table->setSpan(0, stGT, 1, _detailedInfo.PeopleCoords.size());
    ui->Table->item(0, stGT)->setTextAlignment(Qt::AlignCenter);
    for (size_t i = 0; i < _detailedInfo.PeopleCoords.size(); ++i)
    {
        ui->Table->setItem(1,     stGT + i, new QTableWidgetItem("Person " + QString().setNum(i)));
        ui->Table->setColumnWidth(stGT + i, 110);
    }

    // Координаты точек доступа (nodes)
    ui->Table->setItem(0, stGT + offsetNC, new QTableWidgetItem("Node Coords"));
    ui->Table->setSpan(0, stGT + offsetNC, 1, _detailedInfo.NodeCoords.size());
    ui->Table->item(0,    stGT + offsetNC)->setTextAlignment(Qt::AlignCenter);
    for (size_t i = 0; i < _detailedInfo.NodeCoords.size(); ++i)
    {
        ui->Table->setItem(1,     stGT + i + offsetNC, new QTableWidgetItem("Node " + QString().setNum(i)));
        ui->Table->setColumnWidth(stGT + i + offsetNC, 110);
    }

    // Уровень доступа для абонентов
    ui->Table->setItem(0, stGT + offsetSP, new QTableWidgetItem("Signal Power"));
    ui->Table->setSpan(0, stGT + offsetSP, 1, _detailedInfo.SignalPowers.size());
    ui->Table->item(0,    stGT + offsetSP)->setTextAlignment(Qt::AlignCenter);
    for (size_t i = 0; i < _detailedInfo.SignalPowers.size(); ++i)
    {
        ui->Table->setItem(1,     stGT + i + offsetSP, new QTableWidgetItem("Pers " + QString().setNum(i)));
        ui->Table->setColumnWidth(stGT + i + offsetSP, 50);
    }

    // Расстояние до ближайшей точки
    ui->Table->setItem(0, stGT + offsetDTNN, new QTableWidgetItem("Dist To Nearest Node"));
    ui->Table->setSpan(0, stGT + offsetDTNN, 1, _detailedInfo.DistToNearestNode.size());
    ui->Table->item(0,    stGT + offsetDTNN)->setTextAlignment(Qt::AlignCenter);
    for (size_t i = 0; i < _detailedInfo.DistToNearestNode.size(); ++i)
    {
        ui->Table->setItem(1,     stGT + i + offsetDTNN, new QTableWidgetItem("Pers " + QString().setNum(i)));
        ui->Table->setColumnWidth(stGT + i + offsetDTNN, 50);
    }
}
//----------------------------------------------------------

void FormConfigInfo::slotReceiveDetailedInfo(ExportDetailedInfo _detailedInfo)
{
    const int offsetNC = _detailedInfo.PeopleCoords.size();
    const int offsetSP = offsetNC + _detailedInfo.NodeCoords.size();
    const int offsetDTNN = offsetSP + _detailedInfo.SignalPowers.size();

    if (firstInit)
    {
        InitTable(_detailedInfo);

        firstInit = false;
        return;
    }

    if (_detailedInfo.nConfig != nConfig)
        return;


    int curRow = ui->Table->rowCount();
//    ui->Table->insertRow(curRow);
    ui->Table->setRowCount(curRow+1);

//    static size_t n = 0;
    n++;

    ui->Table->setItem(curRow, 0, new QTableWidgetItem(QString().setNum(n)));
    ui->Table->setItem(curRow, 1, new QTableWidgetItem(QString().setNum(_detailedInfo.Time, 'f', 6)));

    // Координаты абонентов
    for (size_t i = 0; i < _detailedInfo.PeopleCoords.size(); ++i)
    {
        ui->Table->setItem(curRow, stGT + i, new QTableWidgetItem(QVector2D_ToQString(_detailedInfo.PeopleCoords[i])));
    }

    // Координаты точек доступа (nodes)
    for (size_t i = 0; i < _detailedInfo.NodeCoords.size(); ++i)
    {
        ui->Table->setItem(curRow, stGT + i + offsetNC, new QTableWidgetItem(QVector2D_ToQString(_detailedInfo.NodeCoords[i])));
    }

    // Уровень доступа для абонентов
    for (size_t i = 0; i < _detailedInfo.SignalPowers.size(); ++i)
    {
        ui->Table->setItem(curRow, stGT + i + offsetSP, new QTableWidgetItem(QString().setNum(_detailedInfo.SignalPowers[i], 'f', 3)));
    }

    // Расстояние до ближайшей точки
    for (size_t i = 0; i < _detailedInfo.DistToNearestNode.size(); ++i)
    {
        ui->Table->setItem(curRow, stGT + i + offsetDTNN, new QTableWidgetItem(QString().setNum(_detailedInfo.DistToNearestNode[i], 'f', 3)));
    }
}
//----------------------------------------------------------

void FormConfigInfo::ShowTable()
{
    const auto & _detailedInfo = Model::Instance().GetExportAllDetailedInfo().at(nConfig);

    if (_detailedInfo.empty() )
        return;

    InitTable(_detailedInfo.front());

    ui->Table->setRowCount(_detailedInfo.size() + 2);

    const int offsetNC = _detailedInfo.front().PeopleCoords.size();
    const int offsetSP = offsetNC + _detailedInfo.front().NodeCoords.size();
    const int offsetDTNN = offsetSP + _detailedInfo.front().SignalPowers.size();

    size_t n = 0;
    for (const auto & OneLine : _detailedInfo)
    {
        ui->Table->setItem(n+2, 0, new QTableWidgetItem(QString().setNum(n)));
        ui->Table->setItem(n+2, 1, new QTableWidgetItem(QString().setNum(OneLine.Time, 'f', 6)));

        // Координаты абонентов
        for (size_t i = 0; i < OneLine.PeopleCoords.size(); ++i)
        {
            ui->Table->setItem(n+2, stGT + i, new QTableWidgetItem(QVector2D_ToQString(OneLine.PeopleCoords[i])));
        }

        // Координаты точек доступа (nodes)
        for (size_t i = 0; i < OneLine.NodeCoords.size(); ++i)
        {
            ui->Table->setItem(n+2, stGT + i + offsetNC, new QTableWidgetItem(QVector2D_ToQString(OneLine.NodeCoords[i])));
        }

        // Уровень доступа для абонентов
        for (size_t i = 0; i < OneLine.SignalPowers.size(); ++i)
        {
            ui->Table->setItem(n+2, stGT + i + offsetSP, new QTableWidgetItem(QString().setNum(OneLine.SignalPowers[i], 'f', 3)));
        }

        // Расстояние до ближайшей точки
        for (size_t i = 0; i < OneLine.DistToNearestNode.size(); ++i)
        {
            ui->Table->setItem(n+2, stGT + i + offsetDTNN, new QTableWidgetItem(QString().setNum(OneLine.DistToNearestNode[i], 'f', 3)));
        }

        n++;
    }
}
//----------------------------------------------------------

FormConfigInfo::~FormConfigInfo()
{
    cout << __PRETTY_FUNCTION__ << endl;

    delete ui;
}
//----------------------------------------------------------

void FormConfigInfo::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_C && QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
        //cout << __PRETTY_FUNCTION__ << endl;

        CopySelectedTableToClipboard(ui->Table);
    }
}
//----------------------------------------------------------
