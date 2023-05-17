#ifndef FORMLEGENDISOLINES_H
#define FORMLEGENDISOLINES_H

#include <QWidget>

namespace Ui {
class FormLegendIsolines;
}

#include "TypesAndUtils.h"

class FormLegendIsolines : public QWidget
{
    Q_OBJECT

public:
    explicit FormLegendIsolines(QWidget *parent = nullptr);
    ~FormLegendIsolines();

    void ReceiveLegendIsolines(const LegendIsolinesType &_li);

private:
    Ui::FormLegendIsolines *ui;
};

#endif // FORMLEGENDISOLINES_H
