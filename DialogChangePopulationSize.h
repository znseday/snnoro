#ifndef DIALOGCHANGEPOPULATIONSIZE_H
#define DIALOGCHANGEPOPULATIONSIZE_H

#include <QDialog>

namespace Ui {
class DialogChangePopulationSize;
}

class DialogChangePopulationSize : public QDialog
{
    Q_OBJECT

public:
    explicit DialogChangePopulationSize(QWidget *parent = nullptr);
    ~DialogChangePopulationSize();

    void InitDialog(int _populationSize);
    int GetNewPopulationSize();

private:
    Ui::DialogChangePopulationSize *ui;
};

#endif // DIALOGCHANGEPOPULATIONSIZE_H
