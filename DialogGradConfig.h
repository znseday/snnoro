#ifndef DIALOGGRADCONFIG_H
#define DIALOGGRADCONFIG_H

#include <QDialog>

#include "GradModel.h"

namespace Ui {
class DialogGradConfig;
}

class DialogGradConfig : public QDialog
{
    Q_OBJECT

public:
    explicit DialogGradConfig(QWidget *parent = nullptr);
    ~DialogGradConfig();

    void InitDialog(const MyGradModel &_gm);
    void ReInitGradDesc(tf_gd_lib::GradDescent &_gd);
    void ReInitTargetFuncSettings(TargetFuncSettingsStruct &_targetFuncSettings);
    void ReInitGradModel(MyGradModel &_gm);

private:

    Ui::DialogGradConfig *ui;



};

#endif // DIALOGGRADCONFIG_H
