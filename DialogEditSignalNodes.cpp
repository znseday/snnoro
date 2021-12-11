#include "DialogEditSignalNodes.h"
#include "ui_DialogEditSignalNodes.h"

#include <QMessageBox>

DialogEditSignalNodes::DialogEditSignalNodes(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogEditSignalNodes)
{
    ui->setupUi(this);
}
//----------------------------------------------------------

DialogEditSignalNodes::~DialogEditSignalNodes()
{
    delete ui;
}
//----------------------------------------------------------

void DialogEditSignalNodes::InitDialog_ForAll(SignalNodeType _snt, const std::vector<SignalNode> &_signalNodes)
{
//    ui->Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
   // ui->Table->setFlags(  ui->Table->flags() & ~Qt::ItemIsEditable);

    ui->Table->clear();

    ui->Table->setRowCount(1 + _signalNodes.size());

    if (_snt == SignalNodeType::Sphere)
        ui->Table->setColumnCount(2);
    else if (_snt == SignalNodeType::Cone)
        ui->Table->setColumnCount(3);
    else
        QMessageBox::critical(this, "Error", "SignalNodeType is Unknown");

    ui->Table->setItem(0, 0, new QTableWidgetItem("n"));
    ui->Table->setColumnWidth(0, 60);
    ui->Table->item(0, 0)->setFlags( ui->Table->item(0, 0)->flags() & ~Qt::ItemIsEditable );

    ui->Table->setItem(0, 1, new QTableWidgetItem("R, m"));
    ui->Table->setColumnWidth(1, 100);
    ui->Table->item(0, 1)->setFlags( ui->Table->item(0, 1)->flags() & ~Qt::ItemIsEditable );

    if (_snt == SignalNodeType::Cone)
    {
        ui->Table->setItem(0, 2, new QTableWidgetItem("Betha, deg"));
        ui->Table->setColumnWidth(2, 100);
        ui->Table->item(0, 2)->setFlags( ui->Table->item(0, 2)->flags() & ~Qt::ItemIsEditable );
    }

    for (size_t i = 0; i < _signalNodes.size(); ++i)
    {
        ui->Table->setItem(1 + i, 0, new QTableWidgetItem(QString().setNum( i )));
        ui->Table->setItem(1 + i, 1, new QTableWidgetItem(QString().setNum( _signalNodes.at(i).R )));

        //ui->Table->setItem(1 + i, 2, new QTableWidgetItem(QString().setNum( _signalNodes.at(i).Alpha )));
        if (_snt == SignalNodeType::Cone)
            ui->Table->setItem(1 + i, 2, new QTableWidgetItem(QString().setNum( qRadiansToDegrees(_signalNodes.at(i).Beta) )));

        ui->Table->item(1 + i, 0)->setFlags( ui->Table->item(1 + i, 0)->flags() & ~Qt::ItemIsEditable );
    }
}
//----------------------------------------------------------

void DialogEditSignalNodes::InitDialog_ForCurrent(SignalNodeType _snt, const std::vector<SignalNode> &_signalNodes,
                                                  const Relief3D & _relief)
{

    const auto &_area = _relief.GetArea();

    ui->lblAreaInfo->setText("Area: l = " + QString().setNum(_area.left()) +
                             "; r = " + QString().setNum(_area.right()) +
                             "; b = " + QString().setNum(_area.bottom()) +
                             "; t = " + QString().setNum(_area.top()) );

    InitDialog_ForAll(_snt, _signalNodes);

    if (_snt == SignalNodeType::Sphere)
        ui->Table->setColumnCount(5);
    else if (_snt == SignalNodeType::Cone)
    {
        ui->Table->setColumnCount(7);

//        ui->Table->setItem(0, 2, new QTableWidgetItem("Betha, deg"));
//        ui->Table->setColumnWidth(2, 100);
//        ui->Table->item(0, 2)->setFlags( ui->Table->item(0, 2)->flags() & ~Qt::ItemIsEditable );

        ui->Table->setItem(0, 3, new QTableWidgetItem("Alpha, deg"));
        ui->Table->setColumnWidth(3, 100);
        ui->Table->item(0, 3)->setFlags( ui->Table->item(0, 3)->flags() & ~Qt::ItemIsEditable );
    }
    else
        QMessageBox::critical(this, "Error", "SignalNodeType is Unknown");

    int offCol = 0;
    if (_snt == SignalNodeType::Cone)
        offCol = 2;

    ui->Table->setItem(0, 2 + offCol, new QTableWidgetItem("x, m"));
    ui->Table->setColumnWidth(2 + offCol, 100);
    ui->Table->item(0, 2 + offCol)->setFlags( ui->Table->item(0, 2 + offCol)->flags() & ~Qt::ItemIsEditable );

    ui->Table->setItem(0, 3 + offCol, new QTableWidgetItem("y, m"));
    ui->Table->setColumnWidth(3 + offCol, 100);
    ui->Table->item(0, 3 + offCol)->setFlags( ui->Table->item(0, 3 + offCol)->flags() & ~Qt::ItemIsEditable );

    ui->Table->setItem(0, 4 + offCol, new QTableWidgetItem("z, m"));
    ui->Table->setColumnWidth(4 + offCol, 100);
    ui->Table->item(0, 4 + offCol)->setFlags( ui->Table->item(0, 4 + offCol)->flags() & ~Qt::ItemIsEditable );

    for (size_t i = 0; i < _signalNodes.size(); ++i)
    {
        //ui->Table->setItem(1 + i, 0, new QTableWidgetItem(QString().setNum( i )));
        //ui->Table->setItem(1 + i, 1, new QTableWidgetItem(QString().setNum( _signalNodes.at(i).R )));

        ui->Table->setItem(1 + i, 2 + offCol, new QTableWidgetItem(QString().setNum( _signalNodes.at(i).Pos.x() )));
        ui->Table->setItem(1 + i, 3 + offCol, new QTableWidgetItem(QString().setNum( _signalNodes.at(i).Pos.y() )));

        ui->Table->setItem(1 + i, 4 + offCol, new QTableWidgetItem(QString().setNum( _signalNodes.at(i).Pos.z() )));

        if (_snt == SignalNodeType::Cone)
            ui->Table->setItem(1 + i, 3, new QTableWidgetItem(QString().setNum( qRadiansToDegrees(_signalNodes.at(i).Alpha) )));

        //ui->Table->item(1 + i, 0)->setFlags( ui->Table->item(1 + i, 0)->flags() & ~Qt::ItemIsEditable );
    }
}
//----------------------------------------------------------

void DialogEditSignalNodes::ChangeSignalNodesParameters_ForAll(SignalNodeType _snt, std::vector<SignalNode> &_signalNodes)
{
    for (size_t i = 0; i < _signalNodes.size(); ++i)
    {
        bool isOk = false;
        double R = ui->Table->item(1 + i, 1)->text().toDouble(&isOk);
        if (isOk)
            _signalNodes.at(i).R = R;
        else
            qDebug() << "R is not saved for " << i << " signal Node";

        if (_snt == SignalNodeType::Cone)
        {
//            isOk = false;
//            double Alpha = ui->Table->item(1 + i, 2)->text().toDouble(&isOk);
//            if (isOk)
//                _signalNodes.at(i).Alpha = Alpha;
//            else
//                qDebug() << "Alpha is not saved for " << i << " signal Node";

            isOk = false;
            double Beta = ui->Table->item(1 + i, 2)->text().toDouble(&isOk);
            if (isOk)
                _signalNodes.at(i).Beta = qDegreesToRadians(Beta);
            else
                qDebug() << "Betha is not saved for " << i << " signal Node";
        }
    }
}
//----------------------------------------------------------

void DialogEditSignalNodes::ChangeSignalNodesParameters_ForCurrent(SignalNodeType _snt, std::vector<SignalNode> &_signalNodes,
                                                                   const Relief3D & _relief)
{
//    ChangeSignalNodesParameters_ForAll(SignalNodeType::Sphere, _signalNodes); // специально передаем Sphere, чтобы взять только R
    ChangeSignalNodesParameters_ForAll(_snt, _signalNodes); // специально передаем Sphere, чтобы взять только R

    int offCol = 0;
    if (_snt == SignalNodeType::Cone)
        offCol = 2;

    for (size_t i = 0; i < _signalNodes.size(); ++i)
    {
//        bool isOk = false;
//        double R = ui->Table->item(1 + i, 1)->text().toDouble(&isOk);
//        if (isOk)
//            _signalNodes.at(i).R = R;
//        else
//            qDebug() << "R is not saved for " << i << " signal Node";

        bool isOk_x = false;
        double x = ui->Table->item(1 + i, 2 + offCol)->text().toDouble(&isOk_x);

        bool isOk_y = false;
        double y = ui->Table->item(1 + i, 3 + offCol)->text().toDouble(&isOk_y);

        if (isOk_x && isOk_y)
        {
            if ( !_signalNodes.at(i).SetCoordForPos(_relief, {(float)x, (float)y, 0.0}) )
            {
                qDebug() << "x and y were correct according the Area for " << i << " signal Node";
            }
        }
        else
        {
            qDebug() << "x and y are not saved for " << i << " signal Node";
        }

        if (_snt == SignalNodeType::Cone)
        {
            bool isOk_Aplpha = false;
            double Alpha = ui->Table->item(1 + i, 3)->text().toDouble(&isOk_Aplpha);
            if (!isOk_Aplpha || Alpha < -360 || Alpha > 360)
            {
                qDebug() << "Alpha was not saved for " << i << " signal Node";
            }
            else
            {
                _signalNodes.at(i).Alpha = qDegreesToRadians(Alpha);
            }
        }

    }
}
//----------------------------------------------------------


