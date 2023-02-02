#ifndef DIALOGABOREPORT_H
#define DIALOGABOREPORT_H

#include <QDialog>
#include <set>
#include "TypesAndUtils.h"

enum class SignalNodeType;

class MyConfig;

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
    void AddTimePoint(int t);

    void CalcTable(const MyConfig &_cnfg, bool _isUseLineBetweenTwoPoints,
                   const SignalNodeType &_snt, TargetFuncTypeEnum funcType);

private:
    Ui::DialogAboReport *ui;

    std::set<int> TimePoints;

protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // DIALOGABOREPORT_H
