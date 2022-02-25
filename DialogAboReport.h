#ifndef DIALOGABOREPORT_H
#define DIALOGABOREPORT_H

#include <QDialog>
#include <set>

//class MyConfig;
class MyGradModel;

namespace Ui {
class DialogAboReport;
}

class DialogAboReport : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAboReport(QWidget *parent = nullptr);
    ~DialogAboReport();

    void InitDialog();
    void AddTimePoint(double t/*const MyConfig &_cnfg*/);

    void CalcTable(MyGradModel &_gradModel);

private:
    Ui::DialogAboReport *ui;

    std::set<double> TimePoints;
};

#endif // DIALOGABOREPORT_H
