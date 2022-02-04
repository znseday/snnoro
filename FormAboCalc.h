#ifndef FORMABOCALC_H
#define FORMABOCALC_H

#include <QDialog>

namespace Ui {
class FormAboCalc;
}

class FormAboCalc : public QDialog
{
    Q_OBJECT

public:
    explicit FormAboCalc(QWidget *parent = nullptr);
    ~FormAboCalc();

private slots:
    void on_btnToReport_clicked();

    void on_EditMaxTime_editingFinished();

    void on_EditMaxTime_textChanged(const QString &str);

    void on_EditMaxTime_textEdited(const QString &str);

    void on_btnOk_clicked();

    void on_SliderTime_sliderMoved(int position);

    void on_EditCurTime_editingFinished();

    void on_SliderTime_valueChanged(int value);

private:
    Ui::FormAboCalc *ui;

protected:
    void showEvent(QShowEvent *event) override;

signals:
    void SignalSendAboTime(int);

};

#endif // FORMABOCALC_H
