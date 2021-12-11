#include "FormSettingsSelector.h"
#include "ui_FormSettingsSelector.h"

#include <QDir>

FormSettingsSelector::FormSettingsSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormSettingsSelector)
{
    ui->setupUi(this);

    QStringList list = QDir(QDir::current().absolutePath()).entryList(QStringList()<< "*.json", QDir::Files);

    //foreach(QString f, list)
    //{
        ui->listWidget->insertItems(0, list);
    //}


}

FormSettingsSelector::~FormSettingsSelector()
{
    delete ui;
}
