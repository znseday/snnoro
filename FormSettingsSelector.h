#ifndef FORMSETTINGSSELECTOR_H
#define FORMSETTINGSSELECTOR_H

#include <QDialog>

namespace Ui {
class FormSettingsSelector;
}

class FormSettingsSelector : public QDialog
{
    Q_OBJECT

public:
    explicit FormSettingsSelector(QWidget *parent = nullptr);
    ~FormSettingsSelector();

    Ui::FormSettingsSelector * GetUI() {return ui;}

//private:
    Ui::FormSettingsSelector *ui;
};

//#include <QListWidgetItem>

//class SettingsFileNameHolder : public QObject
//{
//Q_OBJECT
//private:
//    QString fn;
//public:
//    const QString & GetFileName() {return fn;}
//public slots:
//    void SlotReceiveListItem(QListWidgetItem *wgtItem)
//    {
//        fn = wgtItem->text();
//    }
//};

#endif // FORMSETTINGSSELECTOR_H
