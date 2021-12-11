#ifndef FORMGRADCONFIGDETAILS_H
#define FORMGRADCONFIGDETAILS_H

#include <QWidget>
#include <QKeyEvent>

#include "GradConfig.h"

namespace Ui {
class FormGradConfigDetails;
}

class FormGradConfigDetails : public QWidget
{
    Q_OBJECT

public:
    explicit FormGradConfigDetails(QWidget *parent = nullptr);
    ~FormGradConfigDetails();

    bool ShowTable(const MyConfig &_config, size_t number);

private:
    Ui::FormGradConfigDetails *ui;

    void InitTable();

protected:

    void keyPressEvent(QKeyEvent *e) override;
};

#endif // FORMGRADCONFIGDETAILS_H
