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

    void on_btnShowReport_clicked();

private:
    Ui::FormAboCalc *ui;

//    DialogAboReport dlgAboReport;

protected:
    void showEvent(QShowEvent *event) override;

    void hideEvent(QHideEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

signals:
    void SignalSendAboTime(int);
    void SignalFormAboCalcClose();

    void SignalAddTimePointToReport(int);
    void SignalShowAboReport();
};

#endif // FORMABOCALC_H
