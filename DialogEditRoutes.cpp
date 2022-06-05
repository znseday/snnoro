#include "DialogEditRoutes.h"
#include "ui_DialogEditRoutes.h"

#include "Route.h"

DialogEditRoutes::DialogEditRoutes(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogEditRoutes)
{
    ui->setupUi(this);
}

DialogEditRoutes::~DialogEditRoutes()
{
    delete ui;
}

void DialogEditRoutes::InitDialog(const std::vector<Route> &_routes)
{
    ui->Table->clear();
    ui->Table->setRowCount(1 + _routes.size());
    ui->Table->setColumnCount(3);

    ui->Table->setItem(0, 0, new QTableWidgetItem("n"));
    ui->Table->setColumnWidth(0, 60);
    ui->Table->item(0, 0)->setFlags( ui->Table->item(0, 0)->flags() & ~Qt::ItemIsEditable );

    ui->Table->setItem(0, 1, new QTableWidgetItem("Name"));
    ui->Table->setColumnWidth(1, 100);
    ui->Table->item(0, 1)->setFlags( ui->Table->item(0, 1)->flags() & ~Qt::ItemIsEditable );

    ui->Table->setItem(0, 2, new QTableWidgetItem("V abo, m/s"));
    ui->Table->setColumnWidth(2, 100);
    ui->Table->item(0, 2)->setFlags( ui->Table->item(0, 2)->flags() & ~Qt::ItemIsEditable );

    for (size_t i = 0; i < _routes.size(); ++i)
    {
        ui->Table->setItem(1 + i, 0, new QTableWidgetItem(QString().setNum( i )));
        ui->Table->setItem(1 + i, 1, new QTableWidgetItem(_routes.at(i).GetName() ));

        ui->Table->setItem(1 + i, 2,
              new QTableWidgetItem(QString().setNum( _routes.at(i).GetAbonent().v )));

        ui->Table->item(1 + i, 0)->setFlags( ui->Table->item(1 + i, 0)->flags() & ~Qt::ItemIsEditable );
    }

}

void DialogEditRoutes::ChangeRoutes(std::vector<Route> &_routes)
{
    for (size_t i = 0; i < _routes.size(); ++i)
    {
        QString Name = ui->Table->item(1 + i, 1)->text();
        if (!Name.isEmpty())
            _routes.at(i).SetName(Name);
        else
            qDebug() << "Name is Empty for " << i << " route";


        bool isOk = false;
        double v = ui->Table->item(1 + i, 2)->text().toDouble(&isOk);
        if (isOk)
            _routes.at(i).SetVforAbonent(v);
        else
            qDebug() << "V is not saved for " << i << " route abonent";

    }
}
