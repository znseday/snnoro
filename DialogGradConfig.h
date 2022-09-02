#ifndef DIALOGGRADCONFIG_H
#define DIALOGGRADCONFIG_H

#include "Types.h"
#include <QDialog>

//#include "GradModel.h"
class MyGradModel;
class TargetFuncSettingsStruct;
#include "UnitGradDescent.h"

namespace Ui {
class DialogGradConfig;
}

class DialogGradConfig : public QDialog
{
    Q_OBJECT

private:

    void InitTargetFuncSettingsDialog(const TargetFuncSettingsStruct &_targetFuncSettings);

public:
    explicit DialogGradConfig(QWidget *parent = nullptr);
    ~DialogGradConfig();

    void InitDialog(const MyGradModel &_gm);
    void ReInitGradDesc(tf_gd_lib::GradDescent &_gd);
    void ReInitTargetFuncSettings(TargetFuncSettingsStruct &_targetFuncSettings);
    void ReInitGradModel(MyGradModel &_gm);

private slots:
    void on_btnTargetFuncGlobalOpenFile_clicked();

    void on_btnTargetFuncGlobalSave_clicked();

    void on_btnTargetFuncGlobalSaveAs_clicked();

private:

    Ui::DialogGradConfig *ui;

};

#endif // DIALOGGRADCONFIG_H
