#ifndef FORMCONFIGINFO_H
#define FORMCONFIGINFO_H

#include <QWidget>
#include <QKeyEvent>

#include "Export.h"


namespace Ui {
class FormConfigInfo;
}

class FormConfigInfo : public QWidget
{
    Q_OBJECT

public:
    explicit FormConfigInfo(QWidget *parent = nullptr);
    ~FormConfigInfo();

    void Set_nConfig(int _nConfig);

    void ShowTable();

private:
    Ui::FormConfigInfo *ui;

    int nConfig = -1;

    bool firstInit = true;
    size_t n = 0;

    void InitTable(const ExportDetailedInfo &_detailedInfo);

protected:

    void keyPressEvent(QKeyEvent *e) override;

public slots:

    void slotReceiveDetailedInfo(ExportDetailedInfo _detailedInfo);

};

#endif // FORMCONFIGINFO_H
