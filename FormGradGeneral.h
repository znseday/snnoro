#ifndef FORMGRADGENERAL_H
#define FORMGRADGENERAL_H

#include <QWidget>
//#include <QKeyEvent>
class QKeyEvent;

//#include "GradConfig.h"
class MyConfig;

#include "FormGradConfigDetails.h"

namespace Ui {
class FormGradGeneral;
}

class FormGradGeneral : public QWidget
{
    Q_OBJECT

public:
    FormGradGeneral() = delete;
    explicit FormGradGeneral(const std::vector<MyConfig> &_configs, QWidget *parent = nullptr);
    ~FormGradGeneral();

    bool ShowTable(/*const std::vector<MyConfig> &_configs*/);

private:
    Ui::FormGradGeneral *ui;

    const std::vector<MyConfig> &Configs;
    void InitTable(size_t NodeCount);

    FormGradConfigDetails formGradConfigDetails;

protected:

    void keyPressEvent(QKeyEvent *e) override;

private slots:
    void on_actionGo_to_details_triggered();
};

#endif // FORMGRADGENERAL_H
