#ifndef FORMGENERAL_H
#define FORMGENERAL_H

#include <QWidget>
#include <QTableWidget>
#include <QKeyEvent>

constexpr auto GeneralTableCount = 4;

#include "Export.h"

namespace Ui {
class FormGeneral;
}

class FormGeneral : public QWidget
{
    Q_OBJECT

public:
    explicit FormGeneral(QWidget *parent = nullptr);
    /*virtual*/ ~FormGeneral();

    void ShowTable();

private:
    Ui::FormGeneral *ui;

    //QTableWidget *Tables[GeneralTableCount] = {nullptr};
    //QTableWidget *Table = nullptr;

    void InitTable();

protected:

    void keyPressEvent(QKeyEvent *e) override;

public slots:

    void slotReceiveGeneralData(ExportGeneralData _generalData);
};

#endif // FORMGENERAL_H
