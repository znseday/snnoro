#ifndef DIALOGNEWGRADCONFIG_H
#define DIALOGNEWGRADCONFIG_H

#include <QDialog>

class MyGradModel;

namespace Ui {
class DialogNewGradConfig;
}

class DialogNewGradConfig : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNewGradConfig(QWidget *parent = nullptr);
    ~DialogNewGradConfig();

//    const QString &GetReliefFileName() const;
//    int GetPopulationSize() const;
//    int GetSignalNideCount() const;
//    double GetSignalNodeR() const;

    bool CreateNewGradModel(MyGradModel &_gradModel);

private slots:
    void on_btnReliefFileBrowse_clicked();

private:
    Ui::DialogNewGradConfig *ui;
};

#endif // DIALOGNEWGRADCONFIG_H
