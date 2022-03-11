#ifndef DIALOGABOREPORT_H
#define DIALOGABOREPORT_H

#include <QDialog>
#include <set>

class MyConfig;
//class MyGradModel;

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
    void AddTimePoint(int t/*const MyConfig &_cnfg*/);

    void CalcTable(const MyConfig &_cnfg, bool _isUseLineBetweenTwoPoints);

private:
    Ui::DialogAboReport *ui;

    std::set<int> TimePoints;

    // QPaintDevice interface
//public:
//    QPaintEngine *paintEngine() const override;

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // DIALOGABOREPORT_H
